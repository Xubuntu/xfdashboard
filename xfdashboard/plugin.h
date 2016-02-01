/*
 * plugin: A plugin class managing loading the shared object as well as
 *         initializing and setting up extensions to this application
 * 
 * Copyright 2012-2016 Stephan Haller <nomad@froevel.de>
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

#ifndef __XFDASHBOARD_PLUGIN__
#define __XFDASHBOARD_PLUGIN__

#include <glib-object.h>
#include <gmodule.h>

G_BEGIN_DECLS

/* Helper macros to declare, define and register GObject types in plugins */
#define XFDASHBOARD_DECLARE_PLUGIN_TYPE(inFunctionNamePrefix) \
	void inFunctionNamePrefix##_register_plugin_type(XfdashboardPlugin *inPlugin);

#define XFDASHBOARD_DEFINE_PLUGIN_TYPE(inFunctionNamePrefix) \
	void inFunctionNamePrefix##_register_plugin_type(XfdashboardPlugin *inPlugin) \
	{ \
		inFunctionNamePrefix##_register_type(G_TYPE_MODULE(inPlugin)); \
	}

#define XFDASHBOARD_REGISTER_PLUGIN_TYPE(self, inFunctionNamePrefix) \
	inFunctionNamePrefix##_register_plugin_type(XFDASHBOARD_PLUGIN(self));


/* Definitions */
#define XFDASHBOARD_PLUGIN_ACTION_HANDLED	(TRUE)


/* Object declaration */
#define XFDASHBOARD_TYPE_PLUGIN				(xfdashboard_plugin_get_type())
#define XFDASHBOARD_PLUGIN(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), XFDASHBOARD_TYPE_PLUGIN, XfdashboardPlugin))
#define XFDASHBOARD_IS_PLUGIN(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), XFDASHBOARD_TYPE_PLUGIN))
#define XFDASHBOARD_PLUGIN_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), XFDASHBOARD_TYPE_PLUGIN, XfdashboardPluginClass))
#define XFDASHBOARD_IS_PLUGIN_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), XFDASHBOARD_TYPE_PLUGIN))
#define XFDASHBOARD_PLUGIN_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), XFDASHBOARD_TYPE_PLUGIN, XfdashboardPluginClass))

typedef struct _XfdashboardPlugin			XfdashboardPlugin;
typedef struct _XfdashboardPluginClass		XfdashboardPluginClass;
typedef struct _XfdashboardPluginPrivate	XfdashboardPluginPrivate;

struct _XfdashboardPlugin
{
	/* Parent instance */
	GTypeModule						parent_instance;

	/* Private structure */
	XfdashboardPluginPrivate		*priv;
};

struct _XfdashboardPluginClass
{
	/*< private >*/
	/* Parent class */
	GTypeModuleClass				parent_class;

	/*< public >*/
	/* Virtual functions */
	gboolean (*enable)(XfdashboardPlugin *self);
	gboolean (*disable)(XfdashboardPlugin *self);

	gboolean (*configure)(XfdashboardPlugin *self);
};

/* Error */
#define XFDASHBOARD_PLUGIN_ERROR					(xfdashboard_plugin_error_quark())

GQuark xfdashboard_plugin_error_quark(void);

typedef enum /*< prefix=XFDASHBOARD_PLUGIN_ERROR >*/
{
	XFDASHBOARD_PLUGIN_ERROR_NONE,
	XFDASHBOARD_PLUGIN_ERROR_ERROR,
} XfdashboardPluginErrorEnum;

/* Public API */
GType xfdashboard_plugin_get_type(void) G_GNUC_CONST;

XfdashboardPlugin* xfdashboard_plugin_new(const gchar *inPluginFilename, GError **outError);

void xfdashboard_plugin_set_info(XfdashboardPlugin *self,
									const gchar *inFirstPropertyName, ...)
									G_GNUC_NULL_TERMINATED;

void xfdashboard_plugin_enable(XfdashboardPlugin *self);
void xfdashboard_plugin_disable(XfdashboardPlugin *self);

const gchar* xfdashboard_plugin_get_config_path(XfdashboardPlugin *self);
const gchar* xfdashboard_plugin_get_cache_path(XfdashboardPlugin *self);
const gchar* xfdashboard_plugin_get_data_path(XfdashboardPlugin *self);

G_END_DECLS

#endif	/* __XFDASHBOARD_PLUGIN__ */
