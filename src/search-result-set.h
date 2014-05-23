/*
 * search-result-set: Contains and manages set of identifiers of a search
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

#ifndef __XFDASHBOARD_SEARCH_RESULT_SET__
#define __XFDASHBOARD_SEARCH_RESULT_SET__

#include <glib-object.h>

G_BEGIN_DECLS

#define XFDASHBOARD_TYPE_SEARCH_RESULT_SET				(xfdashboard_search_result_set_get_type())
#define XFDASHBOARD_SEARCH_RESULT_SET(obj)				(G_TYPE_CHECK_INSTANCE_CAST((obj), XFDASHBOARD_TYPE_SEARCH_RESULT_SET, XfdashboardSearchResultSet))
#define XFDASHBOARD_IS_SEARCH_RESULT_SET(obj)			(G_TYPE_CHECK_INSTANCE_TYPE((obj), XFDASHBOARD_TYPE_SEARCH_RESULT_SET))
#define XFDASHBOARD_SEARCH_RESULT_SET_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST((klass), XFDASHBOARD_TYPE_SEARCH_RESULT_SET, XfdashboardSearchResultSetClass))
#define XFDASHBOARD_IS_SEARCH_RESULT_SET_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), XFDASHBOARD_TYPE_SEARCH_RESULT_SET))
#define XFDASHBOARD_SEARCH_RESULT_SET_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), XFDASHBOARD_TYPE_SEARCH_RESULT_SET, XfdashboardSearchResultSetClass))

typedef struct _XfdashboardSearchResultSet				XfdashboardSearchResultSet;
typedef struct _XfdashboardSearchResultSetClass			XfdashboardSearchResultSetClass;
typedef struct _XfdashboardSearchResultSetPrivate		XfdashboardSearchResultSetPrivate;

struct _XfdashboardSearchResultSet
{
	/* Parent instance */
	GObject								parent_instance;

	/* Private structure */
	XfdashboardSearchResultSetPrivate	*priv;
};

struct _XfdashboardSearchResultSetClass
{
	/*< private >*/
	/* Parent class */
	GObjectClass						parent_class;

	/*< public >*/
	/* Virtual functions */
};

/* Public API */
GType xfdashboard_search_result_set_get_type(void) G_GNUC_CONST;

XfdashboardSearchResultSet* xfdashboard_search_result_set_new(void);

guint xfdashboard_search_result_set_get_size(XfdashboardSearchResultSet *self);

void xfdashboard_search_result_set_add_item(XfdashboardSearchResultSet *self, GVariant *inItem);
GVariant* xfdashboard_search_result_set_get_item(XfdashboardSearchResultSet *self, gint inIndex);

gint xfdashboard_search_result_set_get_index(XfdashboardSearchResultSet *self, GVariant *inItem);

void xfdashboard_search_result_set_foreach(XfdashboardSearchResultSet *self,
											GFunc inCallbackFunc,
											gpointer inUserData);

typedef gint (*XfdashboardSearchResultSetCompareFunc)(GVariant *inLeft, GVariant *inRight, gpointer inUserData);
void xfdashboard_search_result_set_sort(XfdashboardSearchResultSet *self,
											XfdashboardSearchResultSetCompareFunc inCallbackFunc,
											gpointer inUserData);
G_END_DECLS

#endif	/* __XFDASHBOARD_SEARCH_RESULT_SET__ */
