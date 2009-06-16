/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#include "config.h"
#include "gtkvbbox.h"
#include "gtkorientable.h"
#include "gtkintl.h"
#include "gtkalias.h"


static gint default_spacing = 10;
static GtkButtonBoxStyle default_layout_style = GTK_BUTTONBOX_EDGE;

G_DEFINE_TYPE (GtkVButtonBox, gtk_vbutton_box, GTK_TYPE_BUTTON_BOX)

static void
gtk_vbutton_box_class_init (GtkVButtonBoxClass *class)
{
}

static void
gtk_vbutton_box_init (GtkVButtonBox *vbutton_box)
{
  gtk_orientable_set_orientation (GTK_ORIENTABLE (vbutton_box),
		  GTK_ORIENTATION_VERTICAL);
}

GtkWidget*
gtk_vbutton_box_new (void)
{
  GtkVButtonBox *vbutton_box;

  vbutton_box = g_object_new (GTK_TYPE_VBUTTON_BOX, NULL);

  return GTK_WIDGET (vbutton_box);
}



/* set default value for spacing */

void
gtk_vbutton_box_set_spacing_default (gint spacing)
{
  default_spacing = spacing;
}


/* set default value for layout style */

void
gtk_vbutton_box_set_layout_default (GtkButtonBoxStyle layout)
{
  g_return_if_fail (layout >= GTK_BUTTONBOX_DEFAULT_STYLE &&
		    layout <= GTK_BUTTONBOX_CENTER);

  default_layout_style = layout;
}

/* get default value for spacing */

gint
gtk_vbutton_box_get_spacing_default (void)
{
  return default_spacing;
}



/* get default value for layout style */

GtkButtonBoxStyle
gtk_vbutton_box_get_layout_default (void)
{
  return default_layout_style;
}

#define __GTK_VBBOX_C__
#include "gtkaliasdef.c"
