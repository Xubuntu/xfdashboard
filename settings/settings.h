/*
 * settings: Settings of application
 * 
 * Copyright 2012-2019 Stephan Haller <nomad@froevel.de>
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

#ifndef __XFDASHBOARD_SETTINGS__
#define __XFDASHBOARD_SETTINGS__

#include <gtk/gtkx.h>

G_BEGIN_DECLS

#define XFDASHBOARD_TYPE_SETTINGS				(xfdashboard_settings_get_type())
#define XFDASHBOARD_SETTINGS(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), XFDASHBOARD_TYPE_SETTINGS, XfdashboardSettings))
#define XFDASHBOARD_IS_SETTINGS(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), XFDASHBOARD_TYPE_SETTINGS))
#define XFDASHBOARD_SETTINGS_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), XFDASHBOARD_TYPE_SETTINGS, XfdashboardSettingsClass))
#define XFDASHBOARD_IS_SETTINGS_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), XFDASHBOARD_TYPE_SETTINGS))
#define XFDASHBOARD_SETTINGS_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS((obj), XFDASHBOARD_TYPE_SETTINGS, XfdashboardSettingsClass))

typedef struct _XfdashboardSettings				XfdashboardSettings;
typedef struct _XfdashboardSettingsClass		XfdashboardSettingsClass;
typedef struct _XfdashboardSettingsPrivate		XfdashboardSettingsPrivate;

struct _XfdashboardSettings
{
	/* Parent instance */
	GObject							parent_instance;

	/* Private structure */
	XfdashboardSettingsPrivate	*priv;
};

struct _XfdashboardSettingsClass
{
	/*< private >*/
	/* Parent class */
	GObjectClass					parent_class;

	/*< public >*/
	/* Virtual functions */
};

/* Public API */
GType xfdashboard_settings_get_type(void) G_GNUC_CONST;

XfdashboardSettings* xfdashboard_settings_new(void);

GtkWidget* xfdashboard_settings_create_dialog(XfdashboardSettings *self);
GtkWidget* xfdashboard_settings_create_plug(XfdashboardSettings *self, Window inSocketID);

G_END_DECLS

#endif	/* __XFDASHBOARD_SETTINGS__ */
