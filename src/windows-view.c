/*
 * windows-view: A view showing visible windows
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

#include "windows-view.h"

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include "live-window.h"
#include "scaled-table-layout.h"
#include "stage.h"
#include "application.h"
#include "view.h"
#include "drop-action.h"
#include "quicklaunch.h"
#include "application-button.h"
#include "window-tracker.h"
#include "image-content.h"
#include "utils.h"
#include "focusable.h"

/* Define this class in GObject system */
static void _xfdashboard_windows_view_focusable_iface_init(XfdashboardFocusableInterface *iface);

G_DEFINE_TYPE_WITH_CODE(XfdashboardWindowsView,
						xfdashboard_windows_view,
						XFDASHBOARD_TYPE_VIEW,
						G_IMPLEMENT_INTERFACE(XFDASHBOARD_TYPE_FOCUSABLE, _xfdashboard_windows_view_focusable_iface_init))

/* Private structure - access only by public API if needed */
#define XFDASHBOARD_WINDOWS_VIEW_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), XFDASHBOARD_TYPE_WINDOWS_VIEW, XfdashboardWindowsViewPrivate))

struct _XfdashboardWindowsViewPrivate
{
	/* Properties related */
	XfdashboardWindowTrackerWorkspace	*workspace;
	gfloat								spacing;
	gboolean							preventUpscaling;

	/* Instance related */
	XfdashboardWindowTracker			*windowTracker;
	ClutterLayoutManager				*layout;
	ClutterActor						*selectedItem;
};

/* Properties */
enum
{
	PROP_0,

	PROP_WORKSPACE,
	PROP_SPACING,
	PROP_PREVENT_UPSCALING,

	PROP_LAST
};

static GParamSpec* XfdashboardWindowsViewProperties[PROP_LAST]={ 0, };

/* Forward declaration */
static XfdashboardLiveWindow* _xfdashboard_windows_view_create_actor(XfdashboardWindowsView *self, XfdashboardWindowTrackerWindow *inWindow);
static void _xfdashboard_windows_view_set_active_workspace(XfdashboardWindowsView *self, XfdashboardWindowTrackerWorkspace *inWorkspace);

/* IMPLEMENTATION: Private variables and methods */
#define DEFAULT_VIEW_ICON			GTK_STOCK_FULLSCREEN
#define DEFAULT_DRAG_HANDLE_SIZE	32.0f

/* Check if window should be shown */
static gboolean _xfdashboard_windows_view_is_visible_window(XfdashboardWindowsView *self,
																XfdashboardWindowTrackerWindow *inWindow)
{
	XfdashboardWindowsViewPrivate		*priv;

	g_return_val_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self), FALSE);
	g_return_val_if_fail(XFDASHBOARD_IS_WINDOW_TRACKER_WINDOW(inWindow), FALSE);

	priv=self->priv;

	/* Determine if windows should be shown depending on its state */
	if(xfdashboard_window_tracker_window_is_skip_pager(inWindow) ||
		xfdashboard_window_tracker_window_is_skip_tasklist(inWindow) ||
		(priv->workspace && !xfdashboard_window_tracker_window_is_visible_on_workspace(inWindow, priv->workspace)) ||
		xfdashboard_window_tracker_window_is_stage(inWindow))
	{
		return(FALSE);
	}

	/* If we get here the window should be shown */
	return(TRUE);
}

/* Find live window actor by window */
static XfdashboardLiveWindow* _xfdashboard_windows_view_find_by_window(XfdashboardWindowsView *self,
																		XfdashboardWindowTrackerWindow *inWindow)
{
	XfdashboardLiveWindow	*liveWindow;
	ClutterActor			*child;
	ClutterActorIter		iter;

	g_return_val_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self), NULL);
	g_return_val_if_fail(XFDASHBOARD_IS_WINDOW_TRACKER_WINDOW(inWindow), NULL);

	/* Iterate through list of current actors and find the one for requested window */
	clutter_actor_iter_init(&iter, CLUTTER_ACTOR(self));
	while(clutter_actor_iter_next(&iter, &child))
	{
		if(!XFDASHBOARD_IS_LIVE_WINDOW(child)) continue;

		liveWindow=XFDASHBOARD_LIVE_WINDOW(child);
		if(xfdashboard_live_window_get_window(liveWindow)==inWindow) return(liveWindow);
	}

	/* If we get here we did not find the window and we return NULL */
	return(NULL);
}

/* Drag of an actor to this view as drop target begins */
static gboolean _xfdashboard_windows_view_on_drop_begin(XfdashboardWindowsView *self,
														XfdashboardDragAction *inDragAction,
														gpointer inUserData)
{
	ClutterActor					*dragSource;
	ClutterActor					*draggedActor;
	gboolean						canHandle;

	g_return_val_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self), FALSE);
	g_return_val_if_fail(XFDASHBOARD_IS_DRAG_ACTION(inDragAction), FALSE);
	g_return_val_if_fail(XFDASHBOARD_IS_DROP_ACTION(inUserData), FALSE);

	canHandle=FALSE;

	/* Get source where dragging started and actor being dragged */
	dragSource=xfdashboard_drag_action_get_source(inDragAction);
	draggedActor=xfdashboard_drag_action_get_actor(inDragAction);

	/* Check if we can handle dragged actor from given source */
	if(XFDASHBOARD_IS_QUICKLAUNCH(dragSource) &&
		XFDASHBOARD_IS_APPLICATION_BUTTON(draggedActor))
	{
		canHandle=TRUE;
	}

	/* Return TRUE if we can handle dragged actor in this drop target
	 * otherwise FALSE
	 */
	return(canHandle);
}

/* Dragged actor was dropped on this drop target */
static void _xfdashboard_windows_view_on_drop_drop(XfdashboardWindowsView *self,
													XfdashboardDragAction *inDragAction,
													gfloat inX,
													gfloat inY,
													gpointer inUserData)
{
	XfdashboardWindowsViewPrivate		*priv;
	ClutterActor						*draggedActor;
	GAppLaunchContext					*context;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_DRAG_ACTION(inDragAction));
	g_return_if_fail(XFDASHBOARD_IS_DROP_ACTION(inUserData));

	priv=self->priv;

	/* Get dragged actor */
	draggedActor=xfdashboard_drag_action_get_actor(inDragAction);
	g_return_if_fail(XFDASHBOARD_IS_APPLICATION_BUTTON(draggedActor));

	/* Launch application being dragged here */
	context=xfdashboard_create_app_context(priv->workspace);
	xfdashboard_application_button_execute(XFDASHBOARD_APPLICATION_BUTTON(draggedActor), context);
	g_object_unref(context);
}

/* Active workspace was changed */
static void _xfdashboard_windows_view_on_active_workspace_changed(XfdashboardWindowsView *self,
																	XfdashboardWindowTrackerWorkspace *inPrevWorkspace,
																	XfdashboardWindowTrackerWorkspace *inNewWorkspace,
																	gpointer inUserData)
{
	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));

	/* Update window list */
	_xfdashboard_windows_view_set_active_workspace(self, inNewWorkspace);
}

/* A window was opened */
static void _xfdashboard_windows_view_on_window_opened(XfdashboardWindowsView *self,
														XfdashboardWindowTrackerWindow *inWindow,
														gpointer inUserData)
{
	XfdashboardLiveWindow				*liveWindow;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_WINDOW_TRACKER_WINDOW(inWindow));

	/* Check if window is visible on this workspace */
	if(!_xfdashboard_windows_view_is_visible_window(self, inWindow)) return;

	/* Create actor */
	liveWindow=_xfdashboard_windows_view_create_actor(self, inWindow);
	if(liveWindow) clutter_actor_insert_child_below(CLUTTER_ACTOR(self), CLUTTER_ACTOR(liveWindow), NULL);
}

/* A window was closed */
static void _xfdashboard_windows_view_on_window_closed(XfdashboardWindowsView *self,
														XfdashboardWindowTrackerWindow *inWindow,
														gpointer inUserData)
{
	XfdashboardWindowsViewPrivate		*priv;
	XfdashboardLiveWindow				*liveWindow;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_WINDOW_TRACKER_WINDOW(inWindow));

	priv=self->priv;

	/* Find live window for window just being closed and destroy it */
	liveWindow=_xfdashboard_windows_view_find_by_window(self, inWindow);
	if(G_LIKELY(liveWindow))
	{
		/* Move selection to next window as this one (the selected one)
		 * will be destroyed.
		 */
		if(priv->selectedItem &&
			(gpointer)liveWindow==(gpointer)priv->selectedItem)
		{
			/* Get next selectable window */
			priv->selectedItem=clutter_actor_get_next_sibling(priv->selectedItem);

			/* If no selectable window follows this one, select first one */
			if(!priv->selectedItem)
			{
				priv->selectedItem=clutter_actor_get_first_child(CLUTTER_ACTOR(self));
			}

			/* If either next selectable window or first one was found,
			 * style it.
			 */
			if(priv->selectedItem)
			{
				xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem),
														"selected");
			}
		}

		/* Destroy actor */
		clutter_actor_destroy(CLUTTER_ACTOR(liveWindow));
	}
}

/* A live window was clicked */
static void _xfdashboard_windows_view_on_window_clicked(XfdashboardWindowsView *self,
														gpointer inUserData)
{
	XfdashboardLiveWindow				*liveWindow;
	XfdashboardWindowTrackerWindow		*window;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_LIVE_WINDOW(inUserData));

	liveWindow=XFDASHBOARD_LIVE_WINDOW(inUserData);

	/* Activate clicked window */
	window=XFDASHBOARD_WINDOW_TRACKER_WINDOW(xfdashboard_live_window_get_window(liveWindow));
	xfdashboard_window_tracker_window_activate(window);

	/* Quit application */
	xfdashboard_application_quit();
}

/* The close button of a live window was clicked */
static void _xfdashboard_windows_view_on_window_close_clicked(XfdashboardWindowsView *self,
																gpointer inUserData)
{
	XfdashboardLiveWindow				*liveWindow;
	XfdashboardWindowTrackerWindow		*window;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_LIVE_WINDOW(inUserData));

	liveWindow=XFDASHBOARD_LIVE_WINDOW(inUserData);

	/* Close clicked window */
	window=XFDASHBOARD_WINDOW_TRACKER_WINDOW(xfdashboard_live_window_get_window(liveWindow));
	xfdashboard_window_tracker_window_close(window);
}

/* A window was moved or resized */
static void _xfdashboard_windows_view_on_window_geometry_changed(XfdashboardWindowsView *self,
																	gpointer inUserData)
{
	XfdashboardLiveWindow				*liveWindow;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_LIVE_WINDOW(inUserData));

	liveWindow=XFDASHBOARD_LIVE_WINDOW(inUserData);

	/* Force a relayout to reflect new size of window */
	clutter_actor_queue_relayout(CLUTTER_ACTOR(liveWindow));
}

/* A window was hidden or shown */
static void _xfdashboard_windows_view_on_window_visibility_changed(XfdashboardWindowsView *self,
																	gboolean inIsVisible,
																	gpointer inUserData)
{
	XfdashboardWindowsViewPrivate		*priv;
	XfdashboardLiveWindow				*liveWindow;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_LIVE_WINDOW(inUserData));

	priv=self->priv;
	liveWindow=XFDASHBOARD_LIVE_WINDOW(inUserData);

	/* If window is shown, show it in window list - otherwise hide it.
	 * We should not destroy the live window actor as the window might
	 * get visible again.
	 */
	if(inIsVisible) clutter_actor_show(CLUTTER_ACTOR(liveWindow));
		else
		{
			/* Move selection to next window as this one (the selected one)
			 * will be hidden and it does not make sense to keep a hidden
			 * window selected ;)
			 */
			if(priv->selectedItem &&
				(gpointer)liveWindow==(gpointer)priv->selectedItem)
			{
				/* Unstyle still selected window */
				xfdashboard_stylable_remove_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem),
															"selected");

				/* Get next selectable window */
				priv->selectedItem=clutter_actor_get_next_sibling(priv->selectedItem);

				/* If no selectable window follows this one, select first one */
				if(!priv->selectedItem)
				{
					priv->selectedItem=clutter_actor_get_first_child(CLUTTER_ACTOR(self));
				}

				/* If either next selectable window or first one was found,
				 * style it.
				 */
				if(priv->selectedItem)
				{
					xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem),
															"selected");
				}
			}

			/* Hide actor */
			clutter_actor_hide(CLUTTER_ACTOR(liveWindow));
		}
}

/* A window changed workspace or was pinned to all workspaces */
static void _xfdashboard_windows_view_on_window_workspace_changed(XfdashboardWindowsView *self,
																	gpointer inUserData)
{
	XfdashboardWindowsViewPrivate		*priv;
	XfdashboardLiveWindow				*liveWindow;
	XfdashboardWindowTrackerWindow		*window;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(XFDASHBOARD_IS_LIVE_WINDOW(inUserData));

	priv=self->priv;
	liveWindow=XFDASHBOARD_LIVE_WINDOW(inUserData);

	/* If window is neither on this workspace nor pinned then destroy it */
	window=xfdashboard_live_window_get_window(liveWindow);
	if(!xfdashboard_window_tracker_window_is_pinned(window) &&
		xfdashboard_window_tracker_window_get_workspace(window)!=priv->workspace)
	{
		/* Move selection to next window as this one (the selected one)
		 * will be destroyed.
		 */
		if(priv->selectedItem &&
			(gpointer)liveWindow==(gpointer)priv->selectedItem)
		{
			/* Get next selectable window */
			priv->selectedItem=clutter_actor_get_next_sibling(priv->selectedItem);

			/* If no selectable window follows this one, select first one */
			if(!priv->selectedItem)
			{
				priv->selectedItem=clutter_actor_get_first_child(CLUTTER_ACTOR(self));
			}

			/* If either next selectable window or first one was found,
			 * style it.
			 */
			if(priv->selectedItem)
			{
				xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem),
														"selected");
			}
		}

		/* Destroy actor */
		clutter_actor_destroy(CLUTTER_ACTOR(liveWindow));
	}
}

/* Drag of a live window begins */
static void _xfdashboard_windows_view_on_drag_begin(ClutterDragAction *inAction,
													ClutterActor *inActor,
													gfloat inStageX,
													gfloat inStageY,
													ClutterModifierType inModifiers,
													gpointer inUserData)
{
	ClutterActor					*dragHandle;
	ClutterStage					*stage;
	GdkPixbuf						*windowIcon;
	ClutterImage					*image;
	XfdashboardLiveWindow			*liveWindow;

	g_return_if_fail(CLUTTER_IS_DRAG_ACTION(inAction));
	g_return_if_fail(XFDASHBOARD_IS_LIVE_WINDOW(inActor));
	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(inUserData));

	liveWindow=XFDASHBOARD_LIVE_WINDOW(inActor);

	/* Prevent signal "clicked" from being emitted on dragged icon */
	g_signal_handlers_block_by_func(inActor, _xfdashboard_windows_view_on_window_clicked, inUserData);

	/* Get stage */
	stage=CLUTTER_STAGE(clutter_actor_get_stage(inActor));

	/* Create a application icon for drag handle */
	windowIcon=xfdashboard_window_tracker_window_get_icon(xfdashboard_live_window_get_window(liveWindow));
	image=xfdashboard_image_content_new_for_pixbuf(windowIcon);

	dragHandle=xfdashboard_background_new();
	clutter_actor_set_position(dragHandle, inStageX, inStageY);
	clutter_actor_set_size(dragHandle, DEFAULT_DRAG_HANDLE_SIZE, DEFAULT_DRAG_HANDLE_SIZE);
	xfdashboard_background_set_image(XFDASHBOARD_BACKGROUND(dragHandle), image);
	clutter_actor_add_child(CLUTTER_ACTOR(stage), dragHandle);

	clutter_drag_action_set_drag_handle(inAction, dragHandle);

	g_object_unref(image);
}

/* Drag of a live window ends */
static void _xfdashboard_windows_view_on_drag_end(ClutterDragAction *inAction,
													ClutterActor *inActor,
													gfloat inStageX,
													gfloat inStageY,
													ClutterModifierType inModifiers,
													gpointer inUserData)
{
	ClutterActor					*dragHandle;

	g_return_if_fail(CLUTTER_IS_DRAG_ACTION(inAction));
	g_return_if_fail(XFDASHBOARD_IS_LIVE_WINDOW(inActor));
	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(inUserData));

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
	g_signal_handlers_unblock_by_func(inActor, _xfdashboard_windows_view_on_window_clicked, inUserData);
}

/* Create actor for wnck-window and connect signals */
static XfdashboardLiveWindow* _xfdashboard_windows_view_create_actor(XfdashboardWindowsView *self,
																		XfdashboardWindowTrackerWindow *inWindow)
{
	ClutterActor	*actor;
	ClutterAction	*dragAction;

	g_return_val_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self), NULL);
	g_return_val_if_fail(XFDASHBOARD_IS_WINDOW_TRACKER_WINDOW(inWindow), NULL);

	/* Check if window opened is a stage window */
	if(xfdashboard_window_tracker_window_is_stage(inWindow))
	{
		g_debug("Will not create live-window actor for stage window.");
		return(NULL);
	}

	/* Create actor and connect signals */
	actor=xfdashboard_live_window_new();
	g_signal_connect_swapped(actor, "clicked", G_CALLBACK(_xfdashboard_windows_view_on_window_clicked), self);
	g_signal_connect_swapped(actor, "close", G_CALLBACK(_xfdashboard_windows_view_on_window_close_clicked), self);
	g_signal_connect_swapped(actor, "geometry-changed", G_CALLBACK(_xfdashboard_windows_view_on_window_geometry_changed), self);
	g_signal_connect_swapped(actor, "visibility-changed", G_CALLBACK(_xfdashboard_windows_view_on_window_visibility_changed), self);
	g_signal_connect_swapped(actor, "workspace-changed", G_CALLBACK(_xfdashboard_windows_view_on_window_workspace_changed), self);
	xfdashboard_live_window_set_window(XFDASHBOARD_LIVE_WINDOW(actor), inWindow);

	dragAction=xfdashboard_drag_action_new_with_source(CLUTTER_ACTOR(self));
	clutter_drag_action_set_drag_threshold(CLUTTER_DRAG_ACTION(dragAction), -1, -1);
	clutter_actor_add_action(actor, dragAction);
	g_signal_connect(dragAction, "drag-begin", G_CALLBACK(_xfdashboard_windows_view_on_drag_begin), self);
	g_signal_connect(dragAction, "drag-end", G_CALLBACK(_xfdashboard_windows_view_on_drag_end), self);

	return(XFDASHBOARD_LIVE_WINDOW(actor));
}

/* Set active screen */
static void _xfdashboard_windows_view_set_active_workspace(XfdashboardWindowsView *self,
															XfdashboardWindowTrackerWorkspace *inWorkspace)
{
	XfdashboardWindowsViewPrivate			*priv;
	GList									*windowsList;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(inWorkspace==NULL || XFDASHBOARD_IS_WINDOW_TRACKER_WORKSPACE(inWorkspace));

	priv=XFDASHBOARD_WINDOWS_VIEW(self)->priv;

	/* Do not anything if workspace is the same as before */
	if(inWorkspace==priv->workspace) return;

	/* Set new workspace */
	priv->workspace=inWorkspace;

	/* Destroy all actors */
	clutter_actor_destroy_all_children(CLUTTER_ACTOR(self));
	priv->selectedItem=NULL;

	/* Create live window actors for new workspace */
	if(priv->workspace!=NULL)
	{
		/* Get list of all windows open */
		windowsList=xfdashboard_window_tracker_get_windows(priv->windowTracker);

		/* Iterate through list of window (from last to first), check if window
		 * is visible and create actor for it if it is.
		 */
		windowsList=g_list_last(windowsList);
		while(windowsList)
		{
			XfdashboardWindowTrackerWindow	*window=XFDASHBOARD_WINDOW_TRACKER_WINDOW(windowsList->data);
			XfdashboardLiveWindow			*liveWindow;

			/* Check if window is visible on this workspace */
			if(_xfdashboard_windows_view_is_visible_window(self, window))
			{
				/* Create actor */
				liveWindow=_xfdashboard_windows_view_create_actor(XFDASHBOARD_WINDOWS_VIEW(self), window);
				if(liveWindow) clutter_actor_add_child(CLUTTER_ACTOR(self), CLUTTER_ACTOR(liveWindow));
			}

			/* Next window */
			windowsList=g_list_previous(windowsList);
		}
	}

	/* Notify about property change */
	g_object_notify_by_pspec(G_OBJECT(self), XfdashboardWindowsViewProperties[PROP_WORKSPACE]);
}

/* IMPLEMENTATION: Interface XfdashboardFocusable */

/* Determine if actor can get the focus */
static gboolean _xfdashboard_windows_view_focusable_can_focus(XfdashboardFocusable *inFocusable)
{
	XfdashboardWindowsView			*self;
	XfdashboardFocusableInterface	*selfIface;
	XfdashboardFocusableInterface	*parentIface;

	g_return_val_if_fail(XFDASHBOARD_IS_FOCUSABLE(inFocusable), FALSE);
	g_return_val_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(inFocusable), FALSE);

	self=XFDASHBOARD_WINDOWS_VIEW(inFocusable);

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
static void _xfdashboard_windows_view_focusable_set_focus(XfdashboardFocusable *inFocusable)
{
	XfdashboardWindowsView			*self;
	XfdashboardWindowsViewPrivate	*priv;
	XfdashboardFocusableInterface	*selfIface;
	XfdashboardFocusableInterface	*parentIface;

	g_return_if_fail(XFDASHBOARD_IS_FOCUSABLE(inFocusable));
	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(inFocusable));

	self=XFDASHBOARD_WINDOWS_VIEW(inFocusable);
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
}

/* Unset focus from actor */
static void _xfdashboard_windows_view_focusable_unset_focus(XfdashboardFocusable *inFocusable)
{
	XfdashboardWindowsView			*self;
	XfdashboardWindowsViewPrivate	*priv;
	XfdashboardFocusableInterface	*selfIface;
	XfdashboardFocusableInterface	*parentIface;

	g_return_if_fail(XFDASHBOARD_IS_FOCUSABLE(inFocusable));
	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(inFocusable));

	self=XFDASHBOARD_WINDOWS_VIEW(inFocusable);
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
}

/* Virtual function "handle_key_event" was called */
static gboolean _xfdashboard_windows_view_focusable_handle_key_event(XfdashboardFocusable *inFocusable,
																		const ClutterEvent *inEvent)
{
	XfdashboardWindowsView					*self;
	XfdashboardWindowsViewPrivate			*priv;

	g_return_val_if_fail(XFDASHBOARD_IS_FOCUSABLE(inFocusable), CLUTTER_EVENT_PROPAGATE);
	g_return_val_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(inFocusable), CLUTTER_EVENT_PROPAGATE);

	self=XFDASHBOARD_WINDOWS_VIEW(inFocusable);
	priv=self->priv;

	/* Handle key events when key was released */
	if(clutter_event_type(inEvent)==CLUTTER_KEY_RELEASE)
	{
		/* Activate selected window on ENTER or close window on DELETE/BACKSPACE */
		switch(inEvent->key.keyval)
		{
			case CLUTTER_KEY_Return:
			case CLUTTER_KEY_KP_Enter:
			case CLUTTER_KEY_ISO_Enter:
				if(priv->selectedItem)
				{
					_xfdashboard_windows_view_on_window_clicked(self, XFDASHBOARD_LIVE_WINDOW(priv->selectedItem));
				}
				return(CLUTTER_EVENT_STOP);

			case CLUTTER_KEY_BackSpace:
			case CLUTTER_KEY_Delete:
			case CLUTTER_KEY_KP_Delete:
				if(priv->selectedItem)
				{
					_xfdashboard_windows_view_on_window_close_clicked(self, XFDASHBOARD_LIVE_WINDOW(priv->selectedItem));
				}
				return(CLUTTER_EVENT_STOP);
		}

		/* Move selection if an arrow key was pressed */
		if(inEvent->key.keyval==CLUTTER_KEY_Left ||
			inEvent->key.keyval==CLUTTER_KEY_Right ||
			inEvent->key.keyval==CLUTTER_KEY_Up ||
			inEvent->key.keyval==CLUTTER_KEY_Down)
		{
			gint							index;
			gint							newIndex;
			gint							numberChildren;
			gint							rows;
			gint							columns;
			gint							selectionRow;
			gint							selectionColumn;
			ClutterActorIter				iter;
			ClutterActor					*child;

			/* If there is nothing selected, select first actor and return */
			if(!priv->selectedItem)
			{
				priv->selectedItem=clutter_actor_get_first_child(CLUTTER_ACTOR(self));

				if(priv->selectedItem)
				{
					xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");
				}

				return(CLUTTER_EVENT_STOP);
			}

			/* Get number of rows and columns and also get number of children
			 * of layout manager.
			 */
			numberChildren=xfdashboard_scaled_table_layout_get_number_children(XFDASHBOARD_SCALED_TABLE_LAYOUT(priv->layout));
			rows=xfdashboard_scaled_table_layout_get_rows(XFDASHBOARD_SCALED_TABLE_LAYOUT(priv->layout));
			columns=xfdashboard_scaled_table_layout_get_columns(XFDASHBOARD_SCALED_TABLE_LAYOUT(priv->layout));

			/* Get index of current selection */
			newIndex=index=0;
			clutter_actor_iter_init(&iter, CLUTTER_ACTOR(self));
			while(clutter_actor_iter_next(&iter, &child) &&
					child!=priv->selectedItem)
			{
				index++;
				newIndex++;
			}

			selectionRow=(index / columns);
			selectionColumn=(index % columns);

			/* Determine index of new selection depending on arrow key pressed */
			if(columns>1 && inEvent->key.keyval==CLUTTER_KEY_Left)
			{
				newIndex--;
				if(newIndex<(selectionRow*columns))
				{
					newIndex=(selectionRow*columns)+columns-1;
					if(newIndex>=numberChildren) newIndex=numberChildren-1;
				}
			}

			if(columns>1 && inEvent->key.keyval==CLUTTER_KEY_Right)
			{
				newIndex++;
				if(newIndex>=((selectionRow+1)*columns) ||
					newIndex>=numberChildren)
				{
					newIndex=(selectionRow*columns);
				}
			}

			if(rows>1 && inEvent->key.keyval==CLUTTER_KEY_Up)
			{
				newIndex-=columns;
				if(newIndex<0)
				{
					newIndex=((rows-1)*columns)+selectionColumn;
					if(newIndex>=numberChildren) newIndex-=columns;
				}
			}

			if(rows>1 && inEvent->key.keyval==CLUTTER_KEY_Down)
			{
				newIndex+=columns;
				if(newIndex>=numberChildren) newIndex=selectionColumn;
			}

			/* Only change selection and update the affected actors if index of
			 * new and old selection differ.
			 */
			if(newIndex==index) return(CLUTTER_EVENT_STOP);

			/* Unstyle current selection */
			xfdashboard_stylable_remove_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");

			/* Get new selection and style it */
			index=newIndex;
			clutter_actor_iter_init(&iter, CLUTTER_ACTOR(self));
			while(clutter_actor_iter_next(&iter, &priv->selectedItem) &&
					index>0)
			{
				index--;
			}

			if(priv->selectedItem)
			{
				xfdashboard_stylable_add_pseudo_class(XFDASHBOARD_STYLABLE(priv->selectedItem), "selected");
			}

			/* Event handled */
			return(CLUTTER_EVENT_STOP);
		}
	}

	/* We did not handle this event */
	return(CLUTTER_EVENT_PROPAGATE);
}

/* Interface initialization
 * Set up default functions
 */
void _xfdashboard_windows_view_focusable_iface_init(XfdashboardFocusableInterface *iface)
{
	iface->can_focus=_xfdashboard_windows_view_focusable_can_focus;
	iface->set_focus=_xfdashboard_windows_view_focusable_set_focus;
	iface->unset_focus=_xfdashboard_windows_view_focusable_unset_focus;
	iface->handle_key_event=_xfdashboard_windows_view_focusable_handle_key_event;
}

/* IMPLEMENTATION: GObject */

/* Dispose this object */
static void _xfdashboard_windows_view_dispose(GObject *inObject)
{
	XfdashboardWindowsView			*self=XFDASHBOARD_WINDOWS_VIEW(inObject);
	XfdashboardWindowsViewPrivate	*priv=XFDASHBOARD_WINDOWS_VIEW(self)->priv;

	/* Release allocated resources */
	_xfdashboard_windows_view_set_active_workspace(self, NULL);

	if(priv->layout)
	{
		priv->layout=NULL;
	}

	if(priv->windowTracker)
	{
		g_signal_handlers_disconnect_by_data(priv->windowTracker, self);
		g_object_unref(priv->windowTracker);
		priv->windowTracker=NULL;
	}

	/* Call parent's class dispose method */
	G_OBJECT_CLASS(xfdashboard_windows_view_parent_class)->dispose(inObject);
}

/* Set/get properties */
static void _xfdashboard_windows_view_set_property(GObject *inObject,
													guint inPropID,
													const GValue *inValue,
													GParamSpec *inSpec)
{
	XfdashboardWindowsView		*self=XFDASHBOARD_WINDOWS_VIEW(inObject);
	
	switch(inPropID)
	{
		case PROP_WORKSPACE:
			_xfdashboard_windows_view_set_active_workspace(self, g_value_get_object(inValue));
			break;

		case PROP_SPACING:
			xfdashboard_windows_view_set_spacing(self, g_value_get_float(inValue));
			break;

		case PROP_PREVENT_UPSCALING:
			xfdashboard_windows_view_set_prevent_upscaling(self, g_value_get_boolean(inValue));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(inObject, inPropID, inSpec);
			break;
	}
}

static void _xfdashboard_windows_view_get_property(GObject *inObject,
													guint inPropID,
													GValue *outValue,
													GParamSpec *inSpec)
{
	XfdashboardWindowsView			*self=XFDASHBOARD_WINDOWS_VIEW(inObject);
	XfdashboardWindowsViewPrivate	*priv=self->priv;

	switch(inPropID)
	{
		case PROP_WORKSPACE:
			g_value_set_object(outValue, priv->workspace);
			break;

		case PROP_SPACING:
			g_value_set_float(outValue, priv->spacing);
			break;

		case PROP_PREVENT_UPSCALING:
			g_value_set_boolean(outValue, self->priv->preventUpscaling);
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
static void xfdashboard_windows_view_class_init(XfdashboardWindowsViewClass *klass)
{
	XfdashboardActorClass	*actorClass=XFDASHBOARD_ACTOR_CLASS(klass);
	GObjectClass			*gobjectClass=G_OBJECT_CLASS(klass);

	/* Override functions */
	gobjectClass->dispose=_xfdashboard_windows_view_dispose;
	gobjectClass->set_property=_xfdashboard_windows_view_set_property;
	gobjectClass->get_property=_xfdashboard_windows_view_get_property;

	/* Set up private structure */
	g_type_class_add_private(klass, sizeof(XfdashboardWindowsViewPrivate));

	/* Define properties */
	XfdashboardWindowsViewProperties[PROP_WORKSPACE]=
		g_param_spec_object("workspace",
							_("Current workspace"),
							_("The current workspace whose windows are shown"),
							XFDASHBOARD_TYPE_WINDOW_TRACKER_WORKSPACE,
							G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

	XfdashboardWindowsViewProperties[PROP_SPACING]=
		g_param_spec_float("spacing",
							_("Spacing"),
							_("Spacing between each element in view"),
							0.0f, G_MAXFLOAT,
							0.0f,
							G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	XfdashboardWindowsViewProperties[PROP_PREVENT_UPSCALING]=
		g_param_spec_boolean("prevent-upscaling",
								_("Prevent upscaling"),
								_("Whether tthis view should prevent upsclaing any window beyond its real size"),
								FALSE,
								G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties(gobjectClass, PROP_LAST, XfdashboardWindowsViewProperties);

	/* Define stylable properties */
	xfdashboard_actor_install_stylable_property(actorClass, XfdashboardWindowsViewProperties[PROP_SPACING]);
	xfdashboard_actor_install_stylable_property(actorClass, XfdashboardWindowsViewProperties[PROP_PREVENT_UPSCALING]);
}

/* Object initialization
 * Create private structure and set up default values
 */
static void xfdashboard_windows_view_init(XfdashboardWindowsView *self)
{
	XfdashboardWindowsViewPrivate	*priv;
	ClutterAction					*action;

	self->priv=priv=XFDASHBOARD_WINDOWS_VIEW_GET_PRIVATE(self);

	/* Set up default values */
	priv->windowTracker=xfdashboard_window_tracker_get_default();
	priv->workspace=NULL;
	priv->spacing=0.0f;
	priv->preventUpscaling=FALSE;
	priv->selectedItem=NULL;

	/* Set up view */
	xfdashboard_view_set_internal_name(XFDASHBOARD_VIEW(self), "windows");
	xfdashboard_view_set_name(XFDASHBOARD_VIEW(self), _("Windows"));
	xfdashboard_view_set_icon(XFDASHBOARD_VIEW(self), DEFAULT_VIEW_ICON);
	xfdashboard_view_set_fit_mode(XFDASHBOARD_VIEW(self), XFDASHBOARD_FIT_MODE_BOTH);

	/* Setup actor */
	xfdashboard_actor_set_can_focus(XFDASHBOARD_ACTOR(self), TRUE);

	priv->layout=xfdashboard_scaled_table_layout_new();
	xfdashboard_scaled_table_layout_set_relative_scale(XFDASHBOARD_SCALED_TABLE_LAYOUT(priv->layout), TRUE);
	xfdashboard_scaled_table_layout_set_prevent_upscaling(XFDASHBOARD_SCALED_TABLE_LAYOUT(priv->layout), priv->preventUpscaling);
	clutter_actor_set_layout_manager(CLUTTER_ACTOR(self), priv->layout);

	action=xfdashboard_drop_action_new();
	clutter_actor_add_action(CLUTTER_ACTOR(self), action);
	g_signal_connect_swapped(action, "begin", G_CALLBACK(_xfdashboard_windows_view_on_drop_begin), self);
	g_signal_connect_swapped(action, "drop", G_CALLBACK(_xfdashboard_windows_view_on_drop_drop), self);

	/* Connect signals */
	g_signal_connect_swapped(priv->windowTracker,
								"active-workspace-changed",
								G_CALLBACK(_xfdashboard_windows_view_on_active_workspace_changed),
								self);

	g_signal_connect_swapped(priv->windowTracker,
								"window-opened",
								G_CALLBACK(_xfdashboard_windows_view_on_window_opened),
								self);

	g_signal_connect_swapped(priv->windowTracker,
								"window-closed",
								G_CALLBACK(_xfdashboard_windows_view_on_window_closed),
								self);
}

/* Implementation: Public API */

/* Get/set spacing between elements */
gfloat xfdashboard_windows_view_get_spacing(XfdashboardWindowsView *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self), 0.0f);

	return(self->priv->spacing);
}

void xfdashboard_windows_view_set_spacing(XfdashboardWindowsView *self, const gfloat inSpacing)
{
	XfdashboardWindowsViewPrivate		*priv;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));
	g_return_if_fail(inSpacing>=0.0f);

	priv=self->priv;

	/* Set value if changed */
	if(priv->spacing!=inSpacing)
	{
		/* Set value */
		priv->spacing=inSpacing;

		/* Update layout manager */
		if(priv->layout)
		{
			xfdashboard_scaled_table_layout_set_spacing(XFDASHBOARD_SCALED_TABLE_LAYOUT(priv->layout), priv->spacing);
		}

		/* Notify about property change */
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardWindowsViewProperties[PROP_SPACING]);
	}
}

/* Get/set if layout manager should prevent to size any child larger than its real size */
gboolean xfdashboard_windows_view_get_prevent_upscaling(XfdashboardWindowsView *self)
{
	g_return_val_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self), FALSE);

	return(self->priv->preventUpscaling);
}

void xfdashboard_windows_view_set_prevent_upscaling(XfdashboardWindowsView *self, gboolean inPreventUpscaling)
{
	XfdashboardWindowsViewPrivate		*priv;

	g_return_if_fail(XFDASHBOARD_IS_WINDOWS_VIEW(self));

	priv=self->priv;

	/* Set value if changed */
	if(priv->preventUpscaling!=inPreventUpscaling)
	{
		/* Set value */
		priv->preventUpscaling=inPreventUpscaling;

		/* Update layout manager */
		if(priv->layout)
		{
			xfdashboard_scaled_table_layout_set_prevent_upscaling(XFDASHBOARD_SCALED_TABLE_LAYOUT(priv->layout), priv->preventUpscaling);
		}

		/* Notify about property change */
		g_object_notify_by_pspec(G_OBJECT(self), XfdashboardWindowsViewProperties[PROP_PREVENT_UPSCALING]);
	}
}
