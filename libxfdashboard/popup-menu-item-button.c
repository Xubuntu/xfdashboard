/*
 * popup-menu-item-button: A button pop-up menu item
 * 
 * Copyright 2012-2017 Stephan Haller <nomad@froevel.de>
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

#include <libxfdashboard/popup-menu-item-button.h>

#include <glib/gi18n-lib.h>

#include <libxfdashboard/popup-menu-item.h>
#include <libxfdashboard/click-action.h>
#include <libxfdashboard/compat.h>


/* Define this class in GObject system */
static void _xfdashboard_popup_menu_item_button_popup_menu_item_iface_init(XfdashboardPopupMenuItemInterface *iface);

G_DEFINE_TYPE_WITH_CODE(XfdashboardPopupMenuItemButton,
						xfdashboard_popup_menu_item_button,
						XFDASHBOARD_TYPE_LABEL,
						G_IMPLEMENT_INTERFACE(XFDASHBOARD_TYPE_POPUP_MENU_ITEM, _xfdashboard_popup_menu_item_button_popup_menu_item_iface_init))

/* Private structure - access only by public API if needed */
#define XFDASHBOARD_BUTTON_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), XFDASHBOARD_TYPE_POPUP_MENU_ITEM_BUTTON, XfdashboardPopupMenuItemButtonPrivate))

struct _XfdashboardPopupMenuItemButtonPrivate
{
	/* Instance related */
	ClutterAction				*clickAction;
	gboolean					enabled;
};

/* IMPLEMENTATION: Private variables and methods */

/* Pop-up menu item button was clicked so activate this item */
static void _xfdashboard_popup_menu_item_button_clicked(XfdashboardClickAction *inAction,
														ClutterActor *self,
														gpointer inUserData)
{
	g_return_if_fail(XFDASHBOARD_IS_CLICK_ACTION(inAction));
	g_return_if_fail(XFDASHBOARD_IS_POPUP_MENU_ITEM_BUTTON(self));

	/* Only activate item if click was perform with left button */
	if(xfdashboard_click_action_get_button(inAction)==XFDASHBOARD_CLICK_ACTION_LEFT_BUTTON)
	{
		xfdashboard_popup_menu_item_activate(XFDASHBOARD_POPUP_MENU_ITEM(self));
	}
}

/* IMPLEMENTATION: Interface XfdashboardPopupMenuItem */

/* Get enable state of this pop-up menu item */
static gboolean _xfdashboard_popup_menu_item_button_popup_menu_item_get_enabled(XfdashboardPopupMenuItem *inMenuItem)
{
	XfdashboardPopupMenuItemButton			*self;
	XfdashboardPopupMenuItemButtonPrivate	*priv;

	g_return_val_if_fail(XFDASHBOARD_IS_POPUP_MENU_ITEM_BUTTON(inMenuItem), FALSE);

	self=XFDASHBOARD_POPUP_MENU_ITEM_BUTTON(inMenuItem);
	priv=self->priv;

	/* Return enabled state */
	return(priv->enabled);
}

/* Set enable state of this pop-up menu item */
static void _xfdashboard_popup_menu_item_button_popup_menu_item_set_enabled(XfdashboardPopupMenuItem *inMenuItem, gboolean inEnabled)
{
	XfdashboardPopupMenuItemButton			*self;
	XfdashboardPopupMenuItemButtonPrivate	*priv;

	g_return_if_fail(XFDASHBOARD_IS_POPUP_MENU_ITEM_BUTTON(inMenuItem));

	self=XFDASHBOARD_POPUP_MENU_ITEM_BUTTON(inMenuItem);
	priv=self->priv;

	/* Set enabled state */
	priv->enabled=inEnabled;
}

/* Interface initialization
 * Set up default functions
 */
void _xfdashboard_popup_menu_item_button_popup_menu_item_iface_init(XfdashboardPopupMenuItemInterface *iface)
{
	iface->get_enabled=_xfdashboard_popup_menu_item_button_popup_menu_item_get_enabled;
	iface->set_enabled=_xfdashboard_popup_menu_item_button_popup_menu_item_set_enabled;
}

/* IMPLEMENTATION: GObject */

/* Class initialization
 * Override functions in parent classes and define properties
 * and signals
 */
static void xfdashboard_popup_menu_item_button_class_init(XfdashboardPopupMenuItemButtonClass *klass)
{
	/* Set up private structure */
	g_type_class_add_private(klass, sizeof(XfdashboardPopupMenuItemButtonPrivate));
}

/* Object initialization
 * Create private structure and set up default values
 */
static void xfdashboard_popup_menu_item_button_init(XfdashboardPopupMenuItemButton *self)
{
	XfdashboardPopupMenuItemButtonPrivate	*priv;

	priv=self->priv=XFDASHBOARD_BUTTON_GET_PRIVATE(self);

	/* Set up default values */
	priv->enabled=TRUE;

	/* This actor reacts on events */
	clutter_actor_set_reactive(CLUTTER_ACTOR(self), TRUE);

	/* Connect signals */
	priv->clickAction=xfdashboard_click_action_new();
	clutter_actor_add_action(CLUTTER_ACTOR(self), priv->clickAction);
	g_signal_connect(priv->clickAction, "clicked", G_CALLBACK(_xfdashboard_popup_menu_item_button_clicked), NULL);
}

/* IMPLEMENTATION: Public API */

/* Create new actor */
ClutterActor* xfdashboard_popup_menu_item_button_new(void)
{
	return(g_object_new(XFDASHBOARD_TYPE_POPUP_MENU_ITEM_BUTTON,
						"text", N_(""),
						"label-style", XFDASHBOARD_LABEL_STYLE_TEXT,
						NULL));
}

ClutterActor* xfdashboard_popup_menu_item_button_new_with_text(const gchar *inText)
{
	return(g_object_new(XFDASHBOARD_TYPE_POPUP_MENU_ITEM_BUTTON,
						"text", inText,
						"label-style", XFDASHBOARD_LABEL_STYLE_TEXT,
						NULL));
}
