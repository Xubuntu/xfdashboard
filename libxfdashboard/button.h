/*
 * button: An actor representing a label and an icon (both optional)
 *         and can react on click actions
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

#ifndef __LIBXFDASHBOARD_BUTTON__
#define __LIBXFDASHBOARD_BUTTON__

#if !defined(__LIBXFDASHBOARD_H_INSIDE__) && !defined(LIBXFDASHBOARD_COMPILATION)
#error "Only <libxfdashboard/libxfdashboard.h> can be included directly."
#endif

#include <clutter/clutter.h>

#include <libxfdashboard/background.h>
#include <libxfdashboard/types.h>

G_BEGIN_DECLS

/* Public definitions */
/**
 * XfdashboardButtonStyle:
 * @XFDASHBOARD_BUTTON_STYLE_TEXT: The actor will show only text labels.
 * @XFDASHBOARD_BUTTON_STYLE_ICON: The actor will show only icons.
 * @XFDASHBOARD_BUTTON_STYLE_BOTH: The actor will show both, text labels and icons.
 *
 * Determines the style of an actor, e.g. text labels and icons at buttons.
 */
typedef enum /*< prefix=XFDASHBOARD_BUTTON_STYLE >*/
{
	XFDASHBOARD_BUTTON_STYLE_TEXT=0,
	XFDASHBOARD_BUTTON_STYLE_ICON,
	XFDASHBOARD_BUTTON_STYLE_BOTH
} XfdashboardButtonStyle;


/* Object declaration */
#define XFDASHBOARD_TYPE_BUTTON					(xfdashboard_button_get_type())
#define XFDASHBOARD_BUTTON(obj)					(G_TYPE_CHECK_INSTANCE_CAST((obj), XFDASHBOARD_TYPE_BUTTON, XfdashboardButton))
#define XFDASHBOARD_IS_BUTTON(obj)				(G_TYPE_CHECK_INSTANCE_TYPE((obj), XFDASHBOARD_TYPE_BUTTON))
#define XFDASHBOARD_BUTTON_CLASS(klass)			(G_TYPE_CHECK_CLASS_CAST((klass), XFDASHBOARD_TYPE_BUTTON, XfdashboardButtonClass))
#define XFDASHBOARD_IS_BUTTON_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE((klass), XFDASHBOARD_TYPE_BUTTON))
#define XFDASHBOARD_BUTTON_GET_CLASS(obj)		(G_TYPE_INSTANCE_GET_CLASS((obj), XFDASHBOARD_TYPE_BUTTON, XfdashboardButtonClass))

typedef struct _XfdashboardButton				XfdashboardButton;
typedef struct _XfdashboardButtonClass			XfdashboardButtonClass;
typedef struct _XfdashboardButtonPrivate		XfdashboardButtonPrivate;

struct _XfdashboardButton
{
	/* Parent instance */
	XfdashboardBackground		parent_instance;

	/* Private structure */
	XfdashboardButtonPrivate	*priv;
};

struct _XfdashboardButtonClass
{
	/*< private >*/
	/* Parent class */
	XfdashboardBackgroundClass	parent_class;

	/*< public >*/
	/* Virtual functions */
	void (*clicked)(XfdashboardButton *self);
};

/* Public API */
GType xfdashboard_button_get_type(void) G_GNUC_CONST;

ClutterActor* xfdashboard_button_new(void);
ClutterActor* xfdashboard_button_new_with_text(const gchar *inText);
ClutterActor* xfdashboard_button_new_with_icon_name(const gchar *inIconName);
ClutterActor* xfdashboard_button_new_with_gicon(GIcon *inIcon);
ClutterActor* xfdashboard_button_new_full_with_icon_name(const gchar *inIconName, const gchar *inText);
ClutterActor* xfdashboard_button_new_full_with_gicon(GIcon *inIcon, const gchar *inText);

/* General functions */
gfloat xfdashboard_button_get_padding(XfdashboardButton *self);
void xfdashboard_button_set_padding(XfdashboardButton *self, const gfloat inPadding);

gfloat xfdashboard_button_get_spacing(XfdashboardButton *self);
void xfdashboard_button_set_spacing(XfdashboardButton *self, const gfloat inSpacing);

XfdashboardButtonStyle xfdashboard_button_get_style(XfdashboardButton *self);
void xfdashboard_button_set_style(XfdashboardButton *self, const XfdashboardButtonStyle inStyle);

/* Icon functions */
const gchar* xfdashboard_button_get_icon_name(XfdashboardButton *self);
void xfdashboard_button_set_icon_name(XfdashboardButton *self, const gchar *inIconName);

GIcon* xfdashboard_button_get_gicon(XfdashboardButton *self);
void xfdashboard_button_set_gicon(XfdashboardButton *self, GIcon *inIcon);

ClutterImage* xfdashboard_button_get_icon_image(XfdashboardButton *self);
void xfdashboard_button_set_icon_image(XfdashboardButton *self, ClutterImage *inIconImage);

gint xfdashboard_button_get_icon_size(XfdashboardButton *self);
void xfdashboard_button_set_icon_size(XfdashboardButton *self, gint inSize);

gboolean xfdashboard_button_get_sync_icon_size(XfdashboardButton *self);
void xfdashboard_button_set_sync_icon_size(XfdashboardButton *self, gboolean inSync);

XfdashboardOrientation xfdashboard_button_get_icon_orientation(XfdashboardButton *self);
void xfdashboard_button_set_icon_orientation(XfdashboardButton *self, const XfdashboardOrientation inOrientation);

/* Label functions */
const gchar* xfdashboard_button_get_text(XfdashboardButton *self);
void xfdashboard_button_set_text(XfdashboardButton *self, const gchar *inMarkupText);

const gchar* xfdashboard_button_get_font(XfdashboardButton *self);
void xfdashboard_button_set_font(XfdashboardButton *self, const gchar *inFont);

const ClutterColor* xfdashboard_button_get_color(XfdashboardButton *self);
void xfdashboard_button_set_color(XfdashboardButton *self, const ClutterColor *inColor);

PangoEllipsizeMode xfdashboard_button_get_ellipsize_mode(XfdashboardButton *self);
void xfdashboard_button_set_ellipsize_mode(XfdashboardButton *self, const PangoEllipsizeMode inMode);

gboolean xfdashboard_button_get_single_line_mode(XfdashboardButton *self);
void xfdashboard_button_set_single_line_mode(XfdashboardButton *self, const gboolean inSingleLine);

PangoAlignment xfdashboard_button_get_text_justification(XfdashboardButton *self);
void xfdashboard_button_set_text_justification(XfdashboardButton *self, const PangoAlignment inJustification);

G_END_DECLS

#endif	/* __LIBXFDASHBOARD_BUTTON__ */
