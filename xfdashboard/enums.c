
/* Generated data (by glib-mkenums) */

/*
 * enums: Definitions of enumerations and flags used in GObject objects
 * 
 * Copyrigt 2012-2015 Stephan Haller <nomad@froevel.de>
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

#if HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */

#include "enums.h"


/* enumerations from "binding.h" */
#include "binding.h"

GType xfdashboard_binding_flags_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GFlagsValue values[]=
		{
			{ XFDASHBOARD_BINDING_FLAGS_NONE, "XFDASHBOARD_BINDING_FLAGS_NONE", "none" },
			{ XFDASHBOARD_BINDING_FLAGS_ALLOW_UNFOCUSABLE_TARGET, "XFDASHBOARD_BINDING_FLAGS_ALLOW_UNFOCUSABLE_TARGET", "allow-unfocusable-target" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_flags_register_static(g_intern_static_string("XfdashboardBindingFlags"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}


/* enumerations from "bindings-pool.h" */
#include "bindings-pool.h"

GType xfdashboard_bindings_pool_error_enum_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_BINDINGS_POOL_ERROR_FILE_NOT_FOUND, "XFDASHBOARD_BINDINGS_POOL_ERROR_FILE_NOT_FOUND", "file-not-found" },
			{ XFDASHBOARD_BINDINGS_POOL_ERROR_PARSER_INTERNAL_ERROR, "XFDASHBOARD_BINDINGS_POOL_ERROR_PARSER_INTERNAL_ERROR", "parser-internal-error" },
			{ XFDASHBOARD_BINDINGS_POOL_ERROR_MALFORMED, "XFDASHBOARD_BINDINGS_POOL_ERROR_MALFORMED", "malformed" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardBindingsPoolErrorEnum"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}


/* enumerations from "theme.h" */
#include "theme.h"

GType xfdashboard_theme_error_enum_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_THEME_ERROR_GENERAL_ERROR, "XFDASHBOARD_THEME_ERROR_GENERAL_ERROR", "general-error" },
			{ XFDASHBOARD_THEME_ERROR_ALREADY_LOADED, "XFDASHBOARD_THEME_ERROR_ALREADY_LOADED", "already-loaded" },
			{ XFDASHBOARD_THEME_ERROR_THEME_NOT_FOUND, "XFDASHBOARD_THEME_ERROR_THEME_NOT_FOUND", "theme-not-found" },
			{ XFDASHBOARD_THEME_ERROR_INVALID_THEME_FILE, "XFDASHBOARD_THEME_ERROR_INVALID_THEME_FILE", "invalid-theme-file" },
			{ XFDASHBOARD_THEME_ERROR_RESOURCE_NOT_FOUND, "XFDASHBOARD_THEME_ERROR_RESOURCE_NOT_FOUND", "resource-not-found" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardThemeErrorEnum"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}


/* enumerations from "theme-css.h" */
#include "theme-css.h"

GType xfdashboard_theme_css_error_enum_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_THEME_CSS_ERROR_INVALID_ARGUMENT, "XFDASHBOARD_THEME_CSS_ERROR_INVALID_ARGUMENT", "invalid-argument" },
			{ XFDASHBOARD_THEME_CSS_ERROR_UNSUPPORTED_STREAM, "XFDASHBOARD_THEME_CSS_ERROR_UNSUPPORTED_STREAM", "unsupported-stream" },
			{ XFDASHBOARD_THEME_CSS_ERROR_PARSER_ERROR, "XFDASHBOARD_THEME_CSS_ERROR_PARSER_ERROR", "parser-error" },
			{ XFDASHBOARD_THEME_CSS_ERROR_FUNCTION_ERROR, "XFDASHBOARD_THEME_CSS_ERROR_FUNCTION_ERROR", "function-error" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardThemeCSSErrorEnum"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}


/* enumerations from "theme-effects.h" */
#include "theme-effects.h"

GType xfdashboard_theme_effects_error_enum_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_THEME_EFFECTS_ERROR_ERROR, "XFDASHBOARD_THEME_EFFECTS_ERROR_ERROR", "error" },
			{ XFDASHBOARD_THEME_EFFECTS_ERROR_MALFORMED, "XFDASHBOARD_THEME_EFFECTS_ERROR_MALFORMED", "malformed" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardThemeEffectsErrorEnum"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}


/* enumerations from "theme-layout.h" */
#include "theme-layout.h"

GType xfdashboard_theme_layout_error_enum_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_THEME_LAYOUT_ERROR_ERROR, "XFDASHBOARD_THEME_LAYOUT_ERROR_ERROR", "error" },
			{ XFDASHBOARD_THEME_LAYOUT_ERROR_MALFORMED, "XFDASHBOARD_THEME_LAYOUT_ERROR_MALFORMED", "malformed" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardThemeLayoutErrorEnum"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}


/* enumerations from "types.h" */
#include "types.h"

GType xfdashboard_view_mode_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_VIEW_MODE_LIST, "XFDASHBOARD_VIEW_MODE_LIST", "list" },
			{ XFDASHBOARD_VIEW_MODE_ICON, "XFDASHBOARD_VIEW_MODE_ICON", "icon" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardViewMode"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_policy_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_POLICY_NEVER, "XFDASHBOARD_POLICY_NEVER", "never" },
			{ XFDASHBOARD_POLICY_AUTOMATIC, "XFDASHBOARD_POLICY_AUTOMATIC", "automatic" },
			{ XFDASHBOARD_POLICY_ALWAYS, "XFDASHBOARD_POLICY_ALWAYS", "always" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardPolicy"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_style_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_STYLE_TEXT, "XFDASHBOARD_STYLE_TEXT", "text" },
			{ XFDASHBOARD_STYLE_ICON, "XFDASHBOARD_STYLE_ICON", "icon" },
			{ XFDASHBOARD_STYLE_BOTH, "XFDASHBOARD_STYLE_BOTH", "both" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardStyle"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_orientation_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_ORIENTATION_LEFT, "XFDASHBOARD_ORIENTATION_LEFT", "left" },
			{ XFDASHBOARD_ORIENTATION_RIGHT, "XFDASHBOARD_ORIENTATION_RIGHT", "right" },
			{ XFDASHBOARD_ORIENTATION_TOP, "XFDASHBOARD_ORIENTATION_TOP", "top" },
			{ XFDASHBOARD_ORIENTATION_BOTTOM, "XFDASHBOARD_ORIENTATION_BOTTOM", "bottom" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardOrientation"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_background_type_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GFlagsValue values[]=
		{
			{ XFDASHBOARD_BACKGROUND_TYPE_NONE, "XFDASHBOARD_BACKGROUND_TYPE_NONE", "none" },
			{ XFDASHBOARD_BACKGROUND_TYPE_FILL, "XFDASHBOARD_BACKGROUND_TYPE_FILL", "fill" },
			{ XFDASHBOARD_BACKGROUND_TYPE_OUTLINE, "XFDASHBOARD_BACKGROUND_TYPE_OUTLINE", "outline" },
			{ XFDASHBOARD_BACKGROUND_TYPE_ROUNDED_CORNERS, "XFDASHBOARD_BACKGROUND_TYPE_ROUNDED_CORNERS", "rounded-corners" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_flags_register_static(g_intern_static_string("XfdashboardBackgroundType"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_corners_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GFlagsValue values[]=
		{
			{ XFDASHBOARD_CORNERS_NONE, "XFDASHBOARD_CORNERS_NONE", "none" },
			{ XFDASHBOARD_CORNERS_TOP_LEFT, "XFDASHBOARD_CORNERS_TOP_LEFT", "top-left" },
			{ XFDASHBOARD_CORNERS_TOP_RIGHT, "XFDASHBOARD_CORNERS_TOP_RIGHT", "top-right" },
			{ XFDASHBOARD_CORNERS_BOTTOM_LEFT, "XFDASHBOARD_CORNERS_BOTTOM_LEFT", "bottom-left" },
			{ XFDASHBOARD_CORNERS_BOTTOM_RIGHT, "XFDASHBOARD_CORNERS_BOTTOM_RIGHT", "bottom-right" },
			{ XFDASHBOARD_CORNERS_TOP, "XFDASHBOARD_CORNERS_TOP", "top" },
			{ XFDASHBOARD_CORNERS_BOTTOM, "XFDASHBOARD_CORNERS_BOTTOM", "bottom" },
			{ XFDASHBOARD_CORNERS_LEFT, "XFDASHBOARD_CORNERS_LEFT", "left" },
			{ XFDASHBOARD_CORNERS_RIGHT, "XFDASHBOARD_CORNERS_RIGHT", "right" },
			{ XFDASHBOARD_CORNERS_ALL, "XFDASHBOARD_CORNERS_ALL", "all" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_flags_register_static(g_intern_static_string("XfdashboardCorners"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_borders_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GFlagsValue values[]=
		{
			{ XFDASHBOARD_BORDERS_NONE, "XFDASHBOARD_BORDERS_NONE", "none" },
			{ XFDASHBOARD_BORDERS_LEFT, "XFDASHBOARD_BORDERS_LEFT", "left" },
			{ XFDASHBOARD_BORDERS_TOP, "XFDASHBOARD_BORDERS_TOP", "top" },
			{ XFDASHBOARD_BORDERS_RIGHT, "XFDASHBOARD_BORDERS_RIGHT", "right" },
			{ XFDASHBOARD_BORDERS_BOTTOM, "XFDASHBOARD_BORDERS_BOTTOM", "bottom" },
			{ XFDASHBOARD_BORDERS_ALL, "XFDASHBOARD_BORDERS_ALL", "all" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_flags_register_static(g_intern_static_string("XfdashboardBorders"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_fit_mode_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_FIT_MODE_NONE, "XFDASHBOARD_FIT_MODE_NONE", "none" },
			{ XFDASHBOARD_FIT_MODE_HORIZONTAL, "XFDASHBOARD_FIT_MODE_HORIZONTAL", "horizontal" },
			{ XFDASHBOARD_FIT_MODE_VERTICAL, "XFDASHBOARD_FIT_MODE_VERTICAL", "vertical" },
			{ XFDASHBOARD_FIT_MODE_BOTH, "XFDASHBOARD_FIT_MODE_BOTH", "both" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardFitMode"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_stage_background_image_type_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_STAGE_BACKGROUND_IMAGE_TYPE_NONE, "XFDASHBOARD_STAGE_BACKGROUND_IMAGE_TYPE_NONE", "none" },
			{ XFDASHBOARD_STAGE_BACKGROUND_IMAGE_TYPE_DESKTOP, "XFDASHBOARD_STAGE_BACKGROUND_IMAGE_TYPE_DESKTOP", "desktop" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardStageBackgroundImageType"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_selection_target_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_SELECTION_TARGET_NONE, "XFDASHBOARD_SELECTION_TARGET_NONE", "none" },
			{ XFDASHBOARD_SELECTION_TARGET_LEFT, "XFDASHBOARD_SELECTION_TARGET_LEFT", "left" },
			{ XFDASHBOARD_SELECTION_TARGET_RIGHT, "XFDASHBOARD_SELECTION_TARGET_RIGHT", "right" },
			{ XFDASHBOARD_SELECTION_TARGET_UP, "XFDASHBOARD_SELECTION_TARGET_UP", "up" },
			{ XFDASHBOARD_SELECTION_TARGET_DOWN, "XFDASHBOARD_SELECTION_TARGET_DOWN", "down" },
			{ XFDASHBOARD_SELECTION_TARGET_FIRST, "XFDASHBOARD_SELECTION_TARGET_FIRST", "first" },
			{ XFDASHBOARD_SELECTION_TARGET_LAST, "XFDASHBOARD_SELECTION_TARGET_LAST", "last" },
			{ XFDASHBOARD_SELECTION_TARGET_PAGE_LEFT, "XFDASHBOARD_SELECTION_TARGET_PAGE_LEFT", "page-left" },
			{ XFDASHBOARD_SELECTION_TARGET_PAGE_RIGHT, "XFDASHBOARD_SELECTION_TARGET_PAGE_RIGHT", "page-right" },
			{ XFDASHBOARD_SELECTION_TARGET_PAGE_UP, "XFDASHBOARD_SELECTION_TARGET_PAGE_UP", "page-up" },
			{ XFDASHBOARD_SELECTION_TARGET_PAGE_DOWN, "XFDASHBOARD_SELECTION_TARGET_PAGE_DOWN", "page-down" },
			{ XFDASHBOARD_SELECTION_TARGET_NEXT, "XFDASHBOARD_SELECTION_TARGET_NEXT", "next" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardSelectionTarget"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}

GType xfdashboard_anchor_point_get_type(void)
{
	static volatile gsize	g_define_type_id__volatile=0;

	if(g_once_init_enter(&g_define_type_id__volatile))
	{
		static const GEnumValue values[]=
		{
			{ XFDASHBOARD_ANCHOR_POINT_NONE, "XFDASHBOARD_ANCHOR_POINT_NONE", "none" },
			{ XFDASHBOARD_ANCHOR_POINT_NORTH_WEST, "XFDASHBOARD_ANCHOR_POINT_NORTH_WEST", "north-west" },
			{ XFDASHBOARD_ANCHOR_POINT_NORTH, "XFDASHBOARD_ANCHOR_POINT_NORTH", "north" },
			{ XFDASHBOARD_ANCHOR_POINT_NORTH_EAST, "XFDASHBOARD_ANCHOR_POINT_NORTH_EAST", "north-east" },
			{ XFDASHBOARD_ANCHOR_POINT_EAST, "XFDASHBOARD_ANCHOR_POINT_EAST", "east" },
			{ XFDASHBOARD_ANCHOR_POINT_SOUTH_EAST, "XFDASHBOARD_ANCHOR_POINT_SOUTH_EAST", "south-east" },
			{ XFDASHBOARD_ANCHOR_POINT_SOUTH, "XFDASHBOARD_ANCHOR_POINT_SOUTH", "south" },
			{ XFDASHBOARD_ANCHOR_POINT_SOUTH_WEST, "XFDASHBOARD_ANCHOR_POINT_SOUTH_WEST", "south-west" },
			{ XFDASHBOARD_ANCHOR_POINT_WEST, "XFDASHBOARD_ANCHOR_POINT_WEST", "west" },
			{ XFDASHBOARD_ANCHOR_POINT_CENTER, "XFDASHBOARD_ANCHOR_POINT_CENTER", "center" },
			{ 0, NULL, NULL }
		};

		GType	g_define_type_id=g_enum_register_static(g_intern_static_string("XfdashboardAnchorPoint"), values);
		g_once_init_leave(&g_define_type_id__volatile, g_define_type_id);
	}

	return(g_define_type_id__volatile);
}


/* Generated data ends here */

