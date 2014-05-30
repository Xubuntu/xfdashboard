/*
 * applications-view: A view showing all installed applications as menu
 * 
 * Copyright 2012-2014 Stephan Haller <nomad@froevel.de>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include "applications-view.h"
#include "view.h"
#include "applications-menu-model.h"
#include "types.h"
#include "button.h"
#include "application.h"
#include "application-button.h"
#include "enums.h"
#include "drag-action.h"
#include "stylable.h"
#include "focusable.h"
#include "focus-manager.h"

/* Define this class in GObject system */
static void _xfdashboard_applications_view_focusable_iface_init(XfdashboardFocusableInterface *iface);

G_DEFINE_TYPE_WITH_CODE(XfdashboardApplicationsView,
						xfdashboard_applications_view,
						XFDASHBOARD_TYPE_VIEW,
						G_IMPLEMENT_INTERFACE(XFDASHBOARD_TYPE_FOCUSABLE, _xfdashboard_applications_view_focusable_iface_init))

/* Private structure - access only by public API if needed */
#define XFDASHBOARD_APPLICATIONS_VIEW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), XFDASHBOARD_TYPE_APPLICATIONS_VIEW, XfdashboardApplicationsViewPrivate))

struct _XfdashboardApplicationsViewPrivate
{
	/* Properties related */
	XfdashboardViewMode					viewMode;
	gfloat								spacing;
	gchar								*parentMenuIcon;
	gchar								*formatTitleOnly;
	gchar								*formatTitleDescription;

	/* Instance related */
	ClutterLayoutManager				*layout;
	XfdashboardApplicationsMenuModel	*apps;
	GarconMenuElement					*currentRootMenuElement;

	gboolean							hasFocus;
	ClutterActor						*selectedItem;
};

/* Properties */
enum
{
	PROP_0,

	PROP_VIEW_MODE,
	PROP_SPACING,

	PROP_PARENT_MENU_ICON,
	PROP_FORMAT_TITLE_ONLY,
	PROP_FORMAT_TITLE_DESCRIPTION,

	PROP_LAST
};

static GParamSpec* XfdashboardApplicationsViewProperties[PROP_LAST]={ 0, };

/* IMPLEMENTATION: Private variables and methods */

/* Forward declarations */
static void _xfdashboard_applications_view_on_item_clicked(XfdashboardApplicationsView *self, gpointer inUserData);

/* Drag of an menu item begins */
static void _xfdashboard_applications_view_on_drag_begin(ClutterDragAction *inAction,
															ClutterActor *inActor,
															gfloat inStageX,
															gfloat inStageY,
															ClutterModifierType inModifiers,
															gpointer inUserData)
{
	const gchar							*desktopName;
	ClutterActor						*dragHandle;
	ClutterStage						*stage;

	g_return_if_fail(CLUTTER_IS_DRAG_ACTION(inAction));
	g_return_if_fail(XFDASHBOARD_IS_APPLICATION_BUTTON(inActor));
	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(inUserData));

	/* Prevent signal "clicked" from being emitted on dragged icon */
	g_signal_handlers_block_by_func(inActor, _xfdashboard_applications_view_on_item_clicked, inUserData);

	/* Get stage */
	stage=CLUTTER_STAGE(clutter_actor_get_stage(inActor));

	/* Create a application icon for drag handle */
	desktopName=xfdashboard_application_button_get_desktop_filename(XFDASHBOARD_APPLICATION_BUTTON(inActor));

	dragHandle=xfdashboard_application_button_new_from_desktop_file(desktopName);
	clutter_actor_set_position(dragHandle, inStageX, inStageY);
	clutter_actor_add_child(CLUTTER_ACTOR(stage), dragHandle);

	clutter_drag_action_set_drag_handle(inAction, dragHandle);
}

/* Drag of an menu item ends */
static void _xfdashboard_applications_view_on_drag_end(ClutterDragAction *inAction,
														ClutterActor *inActor,
														gfloat inStageX,
														gfloat inStageY,
														ClutterModifierType inModifiers,
														gpointer inUserData)
{
	ClutterActor					*dragHandle;

	g_return_if_fail(CLUTTER_IS_DRAG_ACTION(inAction));
	g_return_if_fail(XFDASHBOARD_IS_APPLICATION_BUTTON(inActor));
	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(inUserData));

	/* Destroy clone of application icon used as drag handle */
	dragHandle=clutter_drag_action_get_drag_handle(inAction);
	if(dragHandle)
	{
#if CLUTTER_CHECK_VERSION(1, 14, 0)
		/* Only unset drag handle if not running Clutter in version
		 * 1.12. This prevents a critical warning message in 1.12.
		 * Later versions of Clutter are fixed already.
		 */
		clutter_drag_action_set_drag_handle(inAction, NULL);
#endif
		clutter_actor_destroy(dragHandle);
	}

	/* Allow signal "clicked" from being emitted again */
	g_signal_handlers_unblock_by_func(inActor, _xfdashboard_applications_view_on_item_clicked, inUserData);
}

/* Filter of applications data model has changed */
static void _xfdashboard_applications_view_on_parent_menu_clicked(XfdashboardApplicationsView *self, gpointer inUserData)
{
	XfdashboardApplicationsViewPrivate	*priv;
	GarconMenuElement					*element;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));

	priv=self->priv;

	/* Get associated menu element of button */
	if(priv->currentRootMenuElement &&
		GARCON_IS_MENU(priv->currentRootMenuElement))
	{
		element=GARCON_MENU_ELEMENT(garcon_menu_get_parent(GARCON_MENU(priv->currentRootMenuElement)));

		priv->currentRootMenuElement=element;
		xfdashboard_applications_menu_model_filter_by_section(priv->apps, GARCON_MENU(element));
		xfdashboard_view_scroll_to(XFDASHBOARD_VIEW(self), -1, 0);
	}
}

static void _xfdashboard_applications_view_on_item_clicked(XfdashboardApplicationsView *self, gpointer inUserData)
{
	XfdashboardApplicationsViewPrivate	*priv;
	XfdashboardApplicationButton		*button;
	GarconMenuElement					*element;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_APPLICATION_BUTTON(inUserData));

	priv=self->priv;
	button=XFDASHBOARD_APPLICATION_BUTTON(inUserData);

	/* Get associated menu element of button */
	element=xfdashboard_application_button_get_menu_element(button);

	/* If clicked item is a menu set it as new parent one */
	if(GARCON_IS_MENU(element))
	{
		priv->currentRootMenuElement=element;
		xfdashboard_applications_menu_model_filter_by_section(priv->apps, GARCON_MENU(element));
		xfdashboard_view_scroll_to(XFDASHBOARD_VIEW(self), -1, 0);
	}
		/* If clicked item is a menu item execute command of menu item clicked
		 * and quit application
		 */
		else if(GARCON_IS_MENU_ITEM(element))
		{
			/* Launch application */
			if(xfdashboard_application_button_execute(button, NULL))
			{
				/* Launching application seems to be successfuly so quit application */
				xfdashboard_application_quit();
				return;
			}
		}
}

static void _xfdashboard_applications_view_on_filter_changed(XfdashboardApplicationsView *self, gpointer inUserData)
{
	XfdashboardApplicationsViewPrivate	*priv;
	ClutterModelIter					*iterator;
	ClutterActor						*actor;
	GarconMenuElement					*menuElement=NULL;
	GarconMenu							*parentMenu=NULL;
	ClutterAction						*dragAction;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));

	priv=XFDASHBOARD_APPLICATIONS_VIEW(self)->priv;

	/* Destroy all children */
	priv->selectedItem=NULL;
	clutter_actor_destroy_all_children(CLUTTER_ACTOR(self));
	clutter_layout_manager_layout_changed(priv->layout);

	/* Get parent menu */
	if(priv->currentRootMenuElement &&
		GARCON_IS_MENU(priv->currentRootMenuElement))
	{
		parentMenu=garcon_menu_get_parent(GARCON_MENU(priv->currentRootMenuElement));
	}

	/* If menu element to filter by is not the root menu element, add an "up ..." entry */
	if(parentMenu!=NULL)
	{
		gchar					*actorText;

		/* Create and adjust of "parent menu" button to application buttons */
		actor=xfdashboard_button_new();

		if(priv->parentMenuIcon) xfdashboard_button_set_icon(XFDASHBOARD_BUTTON(actor), priv->parentMenuIcon);

		if(priv->viewMode==XFDASHBOARD_VIEW_MODE_LIST) actorText=g_markup_printf_escaped(priv->formatTitleDescription, _("Back"), _("Go back to previous menu"));
			else actorText=g_markup_printf_escaped(priv->formatTitleOnly, _("Back"));
		xfdashboard_button_set_text(XFDASHBOARD_BUTTON(actor), actorText);
		g_free(actorText);

		if(priv->viewMode==XFDASHBOARD_VIEW_MODE_LIST) xfdashboard_stylable_add_class(XFDASHBOARD_STYLABLE(actor), "view-mode-list");
			else xfdashboard_stylable_add_class(XFDASHBOARD_STYLABLE(actor), "view-mode-icon");

		/* Add to view and layout */
		clutter_actor_set_x_expand(CLUTTER_ACTOR(actor), TRUE);
		clutter_actor_set_y_expand(CLUTTER_ACTOR(actor), TRUE);
		clutter_actor_add_child(CLUTTER_ACTOR(self), CLUTTER_ACTOR(actor));
		clutter_actor_show(actor);

		g_signal_connect_swapped(actor, "clicked", G_CALLBACK(_xfdashboard_applications_view_on_parent_menu_clicked), self);

		/* Select "parent menu" automatically */
		if(priv->hasFocus)
		{
			priv->selectedItem=CLUTTER_ACTOR(actor);
			xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");
		}
	}

	/* Iterate through (filtered) data model and create actor for each entry */
	iterator=clutter_model_get_first_iter(CLUTTER_MODEL(priv->apps));
	if(iterator && CLUTTER_IS_MODEL_ITER(iterator))
	{
		while(!clutter_model_iter_is_last(iterator))
		{
			/* Get data from model */
			clutter_model_iter_get(iterator,
									XFDASHBOARD_APPLICATIONS_MENU_MODEL_COLUMN_MENU_ELEMENT, &menuElement,
									-1);

			if(!menuElement) continue;

			/* Create actor for menu element. Support drag'n'drop at actor if
			 * menu element is a menu item.
			 */
			actor=xfdashboard_application_button_new_from_menu(menuElement);
			if(priv->viewMode==XFDASHBOARD_VIEW_MODE_LIST) xfdashboard_stylable_add_class(XFDASHBOARD_STYLABLE(actor), "view-mode-list");
				else xfdashboard_stylable_add_class(XFDASHBOARD_STYLABLE(actor), "view-mode-icon");

			/* Add to view and layout */
			clutter_actor_set_x_expand(CLUTTER_ACTOR(actor), TRUE);
			clutter_actor_set_y_expand(CLUTTER_ACTOR(actor), TRUE);
			clutter_actor_add_child(CLUTTER_ACTOR(self), CLUTTER_ACTOR(actor));
			clutter_actor_show(actor);

			g_signal_connect_swapped(actor, "clicked", G_CALLBACK(_xfdashboard_applications_view_on_item_clicked), self);

			if(GARCON_IS_MENU_ITEM(menuElement))
			{
				dragAction=xfdashboard_drag_action_new_with_source(CLUTTER_ACTOR(self));
				clutter_drag_action_set_drag_threshold(CLUTTER_DRAG_ACTION(dragAction), -1, -1);
				clutter_actor_add_action(actor, dragAction);
				g_signal_connect(dragAction, "drag-begin", G_CALLBACK(_xfdashboard_applications_view_on_drag_begin), self);
				g_signal_connect(dragAction, "drag-end", G_CALLBACK(_xfdashboard_applications_view_on_drag_end), self);
			}

			/* If no item was selected (i.e. no "parent menu" item) select this one
			 * which is usually the first menu item.
			 */
			if(priv->hasFocus && !priv->selectedItem)
			{
				priv->selectedItem=CLUTTER_ACTOR(actor);
				xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");
			}

			/* Release allocated resources */
			g_object_unref(menuElement);
			menuElement=NULL;

			/* Go to next entry in model */
			iterator=clutter_model_iter_next(iterator);
		}
		g_object_unref(iterator);
	}
}

/* Application model has fully loaded */
static void _xfdashboard_applications_view_on_model_loaded(XfdashboardApplicationsView *self, gpointer inUserData)
{
	XfdashboardApplicationsViewPrivate	*priv;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));

	priv=XFDASHBOARD_APPLICATIONS_VIEW(self)->priv;

	/* Reset to root menu as menu referenced will not be available anymore
	 * and re-filter to update view
	 */
	priv->currentRootMenuElement=NULL;
	xfdashboard_applications_menu_model_filter_by_section(priv->apps, GARCON_MENU(priv->currentRootMenuElement));
}

/* The application will be resumed */
static void _xfdashboard_applications_view_on_application_resume(XfdashboardApplicationsView *self, gpointer inUserData)
{
	XfdashboardApplicationsViewPrivate	*priv=self->priv;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_APPLICATION(inUserData));

	priv=self->priv;

	/* Go to top-level entry */
	priv->currentRootMenuElement=NULL;
	xfdashboard_applications_menu_model_filter_by_section(priv->apps, NULL);
}

/* IMPLEMENTATION: Interface XfdashboardFocusable */

/* Determine if actor can get the focus */
static gboolean _xfdashboard_applications_view_focusable_can_focus(XfdashboardFocusable *inFocusable)
{
	XfdashboardApplicationsView			*self;
	XfdashboardFocusableInterface		*selfIface;
	XfdashboardFocusableInterface		*parentIface;

	g_return_val_if_fail(XFDASHBOARD_IS_FOCUSABLE(inFocusable), FALSE);
	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(inFocusable), FALSE);

	self=XFDASHBOARD_APPLICATIONS_VIEW(inFocusable);

	/* Call parent class interface function */
	selfIface=XFDASHBOARD_FOCUSABLE_GET_IFACE(inFocusable);
	parentIface=g_type_interface_peek_parent(selfIface);

	if(parentIface && parentIface->can_focus)
	{
		if(!parentIface->can_focus(inFocusable)) return(FALSE);
	}

	/* If this view is not enabled it is not focusable */
	if(!xfdashboard_view_get_enabled(XFDASHBOARD_VIEW(self))) return(FALSE);

	/* If we get here this actor can be focused */
	return(TRUE);
}

/* Set focus to actor */
static void _xfdashboard_applications_view_focusable_set_focus(XfdashboardFocusable *inFocusable)
{
	XfdashboardApplicationsView				*self;
	XfdashboardApplicationsViewPrivate		*priv;
	XfdashboardFocusableInterface			*selfIface;
	XfdashboardFocusableInterface			*parentIface;

	g_return_if_fail(XFDASHBOARD_IS_FOCUSABLE(inFocusable));
	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(inFocusable));

	self=XFDASHBOARD_APPLICATIONS_VIEW(inFocusable);
	priv=self->priv;

	/* Call parent class interface function */
	selfIface=XFDASHBOARD_FOCUSABLE_GET_IFACE(inFocusable);
	parentIface=g_type_interface_peek_parent(selfIface);

	if(parentIface && parentIface->set_focus)
	{
		parentIface->set_focus(inFocusable);
	}

	/* Reset selected item to first one */
	if(!priv->selectedItem)
	{
		priv->selectedItem=clutter_actor_get_first_child(CLUTTER_ACTOR(self));
	}

	if(priv->selectedItem)
	{
		xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");
	}

	/* This view gained focus */
	priv->hasFocus=TRUE;
}

/* Unset focus from actor */
static void _xfdashboard_applications_view_focusable_unset_focus(XfdashboardFocusable *inFocusable)
{
	XfdashboardApplicationsView				*self;
	XfdashboardApplicationsViewPrivate		*priv;
	XfdashboardFocusableInterface			*selfIface;
	XfdashboardFocusableInterface			*parentIface;

	g_return_if_fail(XFDASHBOARD_IS_FOCUSABLE(inFocusable));
	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(inFocusable));

	self=XFDASHBOARD_APPLICATIONS_VIEW(inFocusable);
	priv=self->priv;

	/* Call parent class interface function */
	selfIface=XFDASHBOARD_FOCUSABLE_GET_IFACE(inFocusable);
	parentIface=g_type_interface_peek_parent(selfIface);

	if(parentIface && parentIface->set_focus)
	{
		parentIface->unset_focus(inFocusable);
	}

	/* Unstyle selected item */
	if(priv->selectedItem)
	{
		xfdashboard_stylable_remove_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");
	}

	/* This view lost focus */
	priv->hasFocus=FALSE;
}

/* Virtual function "handle_key_event" was called */
static gboolean _xfdashboard_applications_view_focusable_handle_key_event_at_icon_mode(XfdashboardApplicationsView *self,
																						const ClutterEvent *inEvent)
{
	XfdashboardApplicationsViewPrivate	*priv;
	gint								numberChildren, rows, columns;
	gint								selectionIndex, newSelectionIndex;
	gfloat								lastX, lastY;
	ClutterActorIter					iter;
	ClutterActor						*child;
	ClutterActorBox						childBox;
	ClutterActor						*newSelection;
	gint								i;
	gboolean							gotColumns;

	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self), CLUTTER_EVENT_PROPAGATE);

	priv=self->priv;

	/* Determine number of rows and columns and total number of children */
	numberChildren=rows=columns=0;
	selectionIndex=-1;
	lastX=lastY=0.0f;
	gotColumns=FALSE;
	clutter_actor_iter_init(&iter, CLUTTER_ACTOR(self));
	while(clutter_actor_iter_next(&iter, &child))
	{
		/* Only count visible children */
		if(!CLUTTER_ACTOR_IS_VISIBLE(child)) continue;

		/* If x coordinate of this child is smaller than the one of last child
		 * or if y coordinate is larger then a new row begins and we know
		 * the number of columns.
		 */
		clutter_actor_get_allocation_box(child, &childBox);
		if(!gotColumns &&
			(childBox.x1<lastX || childBox.y1>lastY))
		{
			columns=numberChildren;
			gotColumns=TRUE;
		}
		lastX=childBox.x1;
		lastY=childBox.y1;

		/* If current child is the selected one remember its index */
		if(child==priv->selectedItem) selectionIndex=numberChildren;

		/* Count total number of children */
		numberChildren++;
	}

	if(gotColumns)
	{
		rows=1+(numberChildren/columns);
	}
		else if(numberChildren>0)
		{
			rows=1;
			columns=numberChildren;
		}

	/* Determine index of new item to select */
	if(selectionIndex<0)
	{
		g_warning(_("Current selected item not found. Selecting first item."));
		selectionIndex=0;
	}

	switch(inEvent->key.keyval)
	{
		case CLUTTER_KEY_Up:
			newSelectionIndex=selectionIndex-columns;
			if(newSelectionIndex<0)
			{
				newSelectionIndex+=(rows*columns);
				while(newSelectionIndex>=numberChildren && newSelectionIndex>selectionIndex)
				{
					newSelectionIndex-=columns;
				}
			}
			break;

		case CLUTTER_KEY_Down:
			newSelectionIndex=selectionIndex+columns;
			if(newSelectionIndex>=numberChildren)
			{
				newSelectionIndex=(selectionIndex % columns);
				if(newSelectionIndex>=numberChildren) newSelectionIndex=selectionIndex;
			}
			break;

		case CLUTTER_KEY_Left:
			newSelectionIndex=selectionIndex-1;
			if(newSelectionIndex<0 ||
				(newSelectionIndex/columns)!=(selectionIndex/columns))
			{
				newSelectionIndex=((selectionIndex/columns)*columns)+columns-1;
			}
			break;

		case CLUTTER_KEY_Right:
			newSelectionIndex=selectionIndex+1;
			if(newSelectionIndex>=numberChildren ||
				(newSelectionIndex/columns)!=(selectionIndex/columns))
			{
				newSelectionIndex=(selectionIndex/columns)*columns;
			}
			break;

		default:
			newSelectionIndex=selectionIndex;
			break;
	}

	/* If selection did not changed do nothing */
	if(newSelectionIndex==selectionIndex) return(CLUTTER_EVENT_STOP);

	/* Find new item to select */
	i=0;
	newSelection=NULL;
	clutter_actor_iter_init(&iter, CLUTTER_ACTOR(self));
	while(clutter_actor_iter_next(&iter, &child) &&
			!newSelection)
	{
		/* Only count visible children */
		if(!CLUTTER_ACTOR_IS_VISIBLE(child)) continue;

		/* Check for child to lookup */
		if(i==newSelectionIndex)
		{
			newSelection=child;
			break;
		}

		/* Continue with next child */
		i++;
	}

	/* If selection did not changed do nothing */
	if(!newSelection || newSelection==priv->selectedItem) return(CLUTTER_EVENT_STOP);

	/* Unstyle current selection */
	xfdashboard_stylable_remove_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");

	/* Remember and style new selection */
	priv->selectedItem=newSelection;
	xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");

	/* Ensure new selection is visible */
	xfdashboard_view_ensure_visible(XFDASHBOARD_VIEW(self), newSelection);

	/* Event handled */
	return(CLUTTER_EVENT_STOP);
}

static gboolean _xfdashboard_applications_view_focusable_handle_key_event_at_list_mode(XfdashboardApplicationsView *self,
																						const ClutterEvent *inEvent)
{
	XfdashboardApplicationsViewPrivate		*priv;
	ClutterActor							*newSelection;

	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self), CLUTTER_EVENT_PROPAGATE);

	priv=self->priv;
	newSelection=NULL;

	/* Get new selected item */
	if(inEvent->key.keyval==CLUTTER_KEY_Up)
	{
		/* Get previous item to select */
		if(priv->selectedItem) newSelection=clutter_actor_get_previous_sibling(priv->selectedItem);

		/* If there is no previous item, e.g. at begin of list, select last item in list */
		if(!newSelection) newSelection=clutter_actor_get_last_child(CLUTTER_ACTOR(self));
	}

	if(inEvent->key.keyval==CLUTTER_KEY_Down)
	{
		/* Get next item to select */
		if(priv->selectedItem) newSelection=clutter_actor_get_next_sibling(priv->selectedItem);

		/* If there is no next item, e.g. at end of list, select first item in list */
		if(!newSelection) newSelection=clutter_actor_get_first_child(CLUTTER_ACTOR(self));
	}

	/* If selection did not changed do nothing */
	if(!newSelection || newSelection==priv->selectedItem) return(CLUTTER_EVENT_STOP);

	/* Unstyle current selection */
	xfdashboard_stylable_remove_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");

	/* Remember and style new selection */
	priv->selectedItem=newSelection;
	xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");

	/* Ensure new selection is visible */
	xfdashboard_view_ensure_visible(XFDASHBOARD_VIEW(self), newSelection);

	/* Event handled */
	return(CLUTTER_EVENT_STOP);
}

static gboolean _xfdashboard_applications_view_focusable_handle_key_event(XfdashboardFocusable *inFocusable,
																			const ClutterEvent *inEvent)
{
	XfdashboardApplicationsView				*self;
	XfdashboardApplicationsViewPrivate		*priv;
	gboolean								handledEvent;

	g_return_val_if_fail(XFDASHBOARD_IS_FOCUSABLE(inFocusable), CLUTTER_EVENT_PROPAGATE);
	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(inFocusable), CLUTTER_EVENT_PROPAGATE);

	self=XFDASHBOARD_APPLICATIONS_VIEW(inFocusable);
	priv=self->priv;

	/* Handle key events when key was released */
	if(clutter_event_type(inEvent)==CLUTTER_KEY_RELEASE)
	{
		/* Start selected application on ENTER */
		switch(inEvent->key.keyval)
		{
			case CLUTTER_KEY_Return:
			case CLUTTER_KEY_KP_Enter:
			case CLUTTER_KEY_ISO_Enter:
				if(priv->selectedItem)
				{
					if(XFDASHBOARD_IS_APPLICATION_BUTTON(priv->selectedItem))
					{
						_xfdashboard_applications_view_on_item_clicked(self, XFDASHBOARD_APPLICATION_BUTTON(priv->selectedItem));
					}
						else if(XFDASHBOARD_IS_BUTTON(priv->selectedItem))
						{
							_xfdashboard_applications_view_on_parent_menu_clicked(self, XFDASHBOARD_BUTTON(priv->selectedItem));
						}
				}
				return(CLUTTER_EVENT_STOP);
		}

		/* Move selection if an arrow key was pressed */
		if(priv->viewMode==XFDASHBOARD_VIEW_MODE_LIST)
		{
			handledEvent=_xfdashboard_applications_view_focusable_handle_key_event_at_list_mode(self, inEvent);
		}
			else
			{
				handledEvent=_xfdashboard_applications_view_focusable_handle_key_event_at_icon_mode(self, inEvent);
			}

		if(handledEvent==CLUTTER_EVENT_STOP) return(handledEvent);
	}

	/* We did not handle this event */
	return(CLUTTER_EVENT_PROPAGATE);
}

/* Interface initialization
 * Set up default functions
 */
void _xfdashboard_applications_view_focusable_iface_init(XfdashboardFocusableInterface *iface)
{
	iface->can_focus=_xfdashboard_applications_view_focusable_can_focus;
	iface->set_focus=_xfdashboard_applications_view_focusable_set_focus;
	iface->unset_focus=_xfdashboard_applications_view_focusable_unset_focus;
	iface->handle_key_event=_xfdashboard_applications_view_focusable_handle_key_event;
}

/* IMPLEMENTATION: GObject */

/* Dispose this object */
static void _xfdashboard_applications_view_dispose(GObject *inObject)
{
	XfdashboardApplicationsView			*self=XFDASHBOARD_APPLICATIONS_VIEW(inObject);
	XfdashboardApplicationsViewPrivate	*priv=self->priv;

	/* Release allocated resources */
	if(priv->layout) priv->layout=NULL;

	if(priv->apps)
	{
		g_object_unref(priv->apps);
		priv->apps=NULL;
	}

	if(priv->parentMenuIcon)
	{
		g_free(priv->parentMenuIcon);
		priv->parentMenuIcon=NULL;
	}

	if(priv->formatTitleDescription)
	{
		g_free(priv->formatTitleDescription);
		priv->formatTitleDescription=NULL;
	}

	if(priv->formatTitleOnly)
	{
		g_free(priv->formatTitleOnly);
		priv->formatTitleOnly=NULL;
	}

	/* Call parent's class dispose method */
	G_OBJECT_CLASS(xfdashboard_applications_view_parent_class)->dispose(inObject);
}

/* Set/get properties */
static void _xfdashboard_applications_view_set_property(GObject *inObject,
														guint inPropID,
														const GValue *inValue,
														GParamSpec *inSpec)
{
	XfdashboardApplicationsView				*self;

	self=XFDASHBOARD_APPLICATIONS_VIEW(inObject);

	switch(inPropID)
	{
		case PROP_VIEW_MODE:
			xfdashboard_applications_view_set_view_mode(self, (XfdashboardViewMode)g_value_get_enum(inValue));
			break;

		case PROP_SPACING:
			xfdashboard_applications_view_set_spacing(self, g_value_get_float(inValue));
			break;

		case PROP_PARENT_MENU_ICON:
			xfdashboard_applications_view_set_parent_menu_icon(self, g_value_get_string(inValue));
			break;

		case PROP_FORMAT_TITLE_ONLY:
			xfdashboard_applications_view_set_format_title_only(self, g_value_get_string(inValue));
			break;

		case PROP_FORMAT_TITLE_DESCRIPTION:
			xfdashboard_applications_view_set_format_title_description(self, g_value_get_string(inValue));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(inObject, inPropID, inSpec);
			break;
	}
}

static void _xfdashboard_applications_view_get_property(GObject *inObject,
														guint inPropID,
														GValue *outValue,
														GParamSpec *inSpec)
{
	XfdashboardApplicationsView				*self;
	XfdashboardApplicationsViewPrivate		*priv;

	self=XFDASHBOARD_APPLICATIONS_VIEW(inObject);
	priv=self->priv;

	switch(inPropID)
	{
		case PROP_VIEW_MODE:
			g_value_set_enum(outValue, priv->viewMode);
			break;

		case PROP_SPACING:
			g_value_set_float(outValue, priv->spacing);
			break;

		case PROP_PARENT_MENU_ICON:
			g_value_set_string(outValue, priv->parentMenuIcon);
			break;

		case PROP_FORMAT_TITLE_ONLY:
			g_value_set_string(outValue, priv->formatTitleOnly);
			break;

		case PROP_FORMAT_TITLE_DESCRIPTION:
			g_value_set_string(outValue, priv->formatTitleDescription);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(inObject, inPropID, inSpec);
			break;
	}
}

/* Class initialization
 * Override functions in parent classes and define properties
 * and signals
 */
static void xfdashboard_applications_view_class_init(XfdashboardApplicationsViewClass *klass)
{
	XfdashboardActorClass	*actorClass=XFDASHBOARD_ACTOR_CLASS(klass);
	GObjectClass			*gobjectClass=G_OBJECT_CLASS(klass);

	/* Override functions */
	gobjectClass->dispose=_xfdashboard_applications_view_dispose;
	gobjectClass->set_property=_xfdashboard_applications_view_set_property;
	gobjectClass->get_property=_xfdashboard_applications_view_get_property;

	/* Set up private structure */
	g_type_class_add_private(klass, sizeof(XfdashboardApplicationsViewPrivate));

	/* Define properties */
	XfdashboardApplicationsViewProperties[PROP_VIEW_MODE]=
		g_param_spec_enum("view-mode",
							_("View mode"),
							_("The view mode used in this view"),
							XFDASHBOARD_TYPE_VIEW_MODE,
							XFDASHBOARD_VIEW_MODE_LIST,
							G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	XfdashboardApplicationsViewProperties[PROP_SPACING]=
		g_param_spec_float("spacing",
							_("Spacing"),
							_("Spacing between each element in view"),
							0.0f, G_MAXFLOAT,
							0.0f,
							G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	XfdashboardApplicationsViewProperties[PROP_PARENT_MENU_ICON]=
		g_param_spec_string("parent-menu-icon",
								_("Parent menu icon"),
								_("Name of icon to use for 'go-back-to-parent-menu' entries"),
								NULL,
								G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	XfdashboardApplicationsViewProperties[PROP_FORMAT_TITLE_ONLY]=
		g_param_spec_string("format-title-only",
								_("Format title only"),
								_("Format string used when only title is display"),
								NULL,
								G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	XfdashboardApplicationsViewProperties[PROP_FORMAT_TITLE_DESCRIPTION]=
		g_param_spec_string("format-title-description",
								_("Format title and description"),
								_("Format string used when title and description is display. First argument is title and second one is description."),
								NULL,
								G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(gobjectClass, PROP_LAST, XfdashboardApplicationsViewProperties);

	/* Define stylable properties */
	xfdashboard_actor_install_stylable_property(actorClass, XfdashboardApplicationsViewProperties[PROP_VIEW_MODE]);
	xfdashboard_actor_install_stylable_property(actorClass, XfdashboardApplicationsViewProperties[PROP_SPACING]);
	xfdashboard_actor_install_stylable_property(actorClass, XfdashboardApplicationsViewProperties[PROP_PARENT_MENU_ICON]);
	xfdashboard_actor_install_stylable_property(actorClass, XfdashboardApplicationsViewProperties[PROP_FORMAT_TITLE_ONLY]);
	xfdashboard_actor_install_stylable_property(actorClass, XfdashboardApplicationsViewProperties[PROP_FORMAT_TITLE_DESCRIPTION]);
}

/* Object initialization
 * Create private structure and set up default values
 */
static void xfdashboard_applications_view_init(XfdashboardApplicationsView *self)
{
	XfdashboardApplicationsViewPrivate	*priv;
	XfdashboardApplication				*application;

	self->priv=priv=XFDASHBOARD_APPLICATIONS_VIEW_GET_PRIVATE(self);

	/* Set up default values */
	priv->apps=XFDASHBOARD_APPLICATIONS_MENU_MODEL(xfdashboard_applications_menu_model_new());
	priv->currentRootMenuElement=NULL;
	priv->viewMode=-1;
	priv->spacing=0.0f;
	priv->parentMenuIcon=NULL;
	priv->formatTitleOnly=g_strdup("%s");
	priv->formatTitleDescription=g_strdup("%s\n%s");
	priv->hasFocus=FALSE;
	priv->selectedItem=NULL;

	/* Set up view */
	xfdashboard_view_set_internal_name(XFDASHBOARD_VIEW(self), "applications");
	xfdashboard_view_set_name(XFDASHBOARD_VIEW(self), _("Applications"));
	xfdashboard_view_set_icon(XFDASHBOARD_VIEW(self), GTK_STOCK_HOME);

	/* Set up actor */
	xfdashboard_actor_set_can_focus(XFDASHBOARD_ACTOR(self), TRUE);

	xfdashboard_view_set_fit_mode(XFDASHBOARD_VIEW(self), XFDASHBOARD_FIT_MODE_HORIZONTAL);
	xfdashboard_applications_view_set_view_mode(self, XFDASHBOARD_VIEW_MODE_LIST);
	clutter_model_set_sorting_column(CLUTTER_MODEL(priv->apps), XFDASHBOARD_APPLICATIONS_MENU_MODEL_COLUMN_TITLE);

	/* Connect signals */
	g_signal_connect_swapped(priv->apps, "filter-changed", G_CALLBACK(_xfdashboard_applications_view_on_filter_changed), self);
	g_signal_connect_swapped(priv->apps, "loaded", G_CALLBACK(_xfdashboard_applications_view_on_model_loaded), self);

	/* Connect signal to application */
	application=xfdashboard_application_get_default();
	g_signal_connect_swapped(application, "resume", G_CALLBACK(_xfdashboard_applications_view_on_application_resume), self);
}

/* Get/set view mode of view */
XfdashboardViewMode xfdashboard_applications_view_get_view_mode(XfdashboardApplicationsView *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self), XFDASHBOARD_VIEW_MODE_LIST);

	return(self->priv->viewMode);
}

void xfdashboard_applications_view_set_view_mode(XfdashboardApplicationsView *self, const XfdashboardViewMode inMode)
{
	XfdashboardApplicationsViewPrivate	*priv;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));
	g_return_if_fail(inMode<=XFDASHBOARD_VIEW_MODE_ICON);

	priv=self->priv;

	/* Set value if changed */
	if(priv->viewMode!=inMode)
	{
		/* Set value */
		if(priv->layout)
		{
			clutter_actor_set_layout_manager(CLUTTER_ACTOR(self), NULL);
			priv->layout=NULL;
		}

		priv->viewMode=inMode;

		/* Set new layout manager */
		switch(priv->viewMode)
		{
			case XFDASHBOARD_VIEW_MODE_LIST:
				priv->layout=clutter_box_layout_new();
				clutter_box_layout_set_orientation(CLUTTER_BOX_LAYOUT(priv->layout), CLUTTER_ORIENTATION_VERTICAL);
				clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(priv->layout), priv->spacing);
				clutter_actor_set_layout_manager(CLUTTER_ACTOR(self), priv->layout);
				break;

			case XFDASHBOARD_VIEW_MODE_ICON:
				priv->layout=clutter_flow_layout_new(CLUTTER_FLOW_HORIZONTAL);
				clutter_flow_layout_set_column_spacing(CLUTTER_FLOW_LAYOUT(priv->layout), priv->spacing);
				clutter_flow_layout_set_row_spacing(CLUTTER_FLOW_LAYOUT(priv->layout), priv->spacing);
				clutter_flow_layout_set_homogeneous(CLUTTER_FLOW_LAYOUT(priv->layout), TRUE);
				clutter_actor_set_layout_manager(CLUTTER_ACTOR(self), priv->layout);
				break;

			default:
				g_assert_not_reached();
		}

		/* Rebuild view */
		_xfdashboard_applications_view_on_filter_changed(self, NULL);

		/* Notify about property change */
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardApplicationsViewProperties[PROP_VIEW_MODE]);
	}
}

/* Get/set spacing between elements */
gfloat xfdashboard_applications_view_get_spacing(XfdashboardApplicationsView *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self), 0.0f);

	return(self->priv->spacing);
}

void xfdashboard_applications_view_set_spacing(XfdashboardApplicationsView *self, const gfloat inSpacing)
{
	XfdashboardApplicationsViewPrivate		*priv;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));
	g_return_if_fail(inSpacing>=0.0f);

	priv=self->priv;

	/* Set value if changed */
	if(priv->spacing!=inSpacing)
	{
		/* Set value */
		priv->spacing=inSpacing;

		/* Update layout manager */
		switch(priv->viewMode)
		{
			case XFDASHBOARD_VIEW_MODE_LIST:
				clutter_box_layout_set_spacing(CLUTTER_BOX_LAYOUT(priv->layout), priv->spacing);
				break;

			case XFDASHBOARD_VIEW_MODE_ICON:
				clutter_flow_layout_set_column_spacing(CLUTTER_FLOW_LAYOUT(priv->layout), priv->spacing);
				clutter_flow_layout_set_row_spacing(CLUTTER_FLOW_LAYOUT(priv->layout), priv->spacing);
				break;

			default:
				g_assert_not_reached();
		}

		/* Notify about property change */
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardApplicationsViewProperties[PROP_SPACING]);
	}
}

/* Implementation: Public API */

/* Get/set icon name for 'go-back-to-parent-menu' entries */
const gchar* xfdashboard_applications_view_get_parent_menu_icon(XfdashboardApplicationsView *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self), NULL);

	return(self->priv->parentMenuIcon);
}

void xfdashboard_applications_view_set_parent_menu_icon(XfdashboardApplicationsView *self, const gchar *inIconName)
{
	XfdashboardApplicationsViewPrivate		*priv;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));

	priv=self->priv;

	/* Set value if changed */
	if(g_strcmp0(priv->parentMenuIcon, inIconName)!=0)
	{
		/* Set value */
		if(priv->parentMenuIcon)
		{
			g_free(priv->parentMenuIcon);
			priv->parentMenuIcon=NULL;
		}

		if(inIconName) priv->parentMenuIcon=g_strdup(inIconName);

		/* Update actor */
		_xfdashboard_applications_view_on_filter_changed(self, NULL);

		/* Notify about property change */
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardApplicationsViewProperties[PROP_PARENT_MENU_ICON]);
	}
}

/* Get/set format string to use when displaying only title */
const gchar* xfdashboard_applications_view_get_format_title_only(XfdashboardApplicationsView *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self), NULL);

	return(self->priv->formatTitleOnly);
}

void xfdashboard_applications_view_set_format_title_only(XfdashboardApplicationsView *self, const gchar *inFormat)
{
	XfdashboardApplicationsViewPrivate		*priv;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));
	g_return_if_fail(inFormat);

	priv=self->priv;

	/* Set value if changed */
	if(g_strcmp0(priv->formatTitleOnly, inFormat)!=0)
	{
		/* Set value */
		if(priv->formatTitleOnly) g_free(priv->formatTitleOnly);
		priv->formatTitleOnly=g_strdup(inFormat);

		/* Update view only if view mode is list which uses this format string */
		if(priv->viewMode==XFDASHBOARD_VIEW_MODE_ICON) _xfdashboard_applications_view_on_filter_changed(self, NULL);

		/* Notify about property change */
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardApplicationsViewProperties[PROP_FORMAT_TITLE_ONLY]);
	}
}

/* Get/set format string to use when displaying title and description */
const gchar* xfdashboard_applications_view_get_format_title_description(XfdashboardApplicationsView *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self), NULL);

	return(self->priv->formatTitleDescription);
}

void xfdashboard_applications_view_set_format_title_description(XfdashboardApplicationsView *self, const gchar *inFormat)
{
	XfdashboardApplicationsViewPrivate		*priv;

	g_return_if_fail(XFDASHBOARD_IS_APPLICATIONS_VIEW(self));
	g_return_if_fail(inFormat);

	priv=self->priv;

	/* Set value if changed */
	if(g_strcmp0(priv->formatTitleDescription, inFormat)!=0)
	{
		/* Set value */
		if(priv->formatTitleDescription) g_free(priv->formatTitleDescription);
		priv->formatTitleDescription=g_strdup(inFormat);

		/* Update view only if view mode is list which uses this format string */
		if(priv->viewMode==XFDASHBOARD_VIEW_MODE_LIST) _xfdashboard_applications_view_on_filter_changed(self, NULL);

		/* Notify about property change */
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardApplicationsViewProperties[PROP_FORMAT_TITLE_DESCRIPTION]);
	}
}
