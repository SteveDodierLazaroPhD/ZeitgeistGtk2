/* gtkbuildable.c
 * Copyright (C) 2006-2007 Async Open Source,
 *                         Johan Dahlin <jdahlin@async.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include <config.h>
#include "gtkbuildable.h"
#include "gtktypeutils.h"
#include "gtkintl.h"
#include "gtkalias.h"

GType
gtk_buildable_get_type (void)
{
  static GType buildable_type = 0;

  if (!buildable_type)
    buildable_type =
      g_type_register_static_simple (G_TYPE_INTERFACE, I_("GtkBuildable"),
				     sizeof (GtkBuildableIface),
				     NULL, 0, NULL, 0);

  return buildable_type;
}

/**
 * gtk_buildable_set_name:
 * @buildable: a #GtkBuildable
 * @name: name to set
 *
 * Sets the name of the buildable object, it's used to synchronize the name
 * if the object already has it's own concept of name.
 *
 * #GtkWidget implements this to map the buildable name to the widget name
 *
 * Since: 2.12
 **/
void
gtk_buildable_set_name (GtkBuildable *buildable,
                        const gchar  *name)
{
  GtkBuildableIface *iface;

  g_return_if_fail (GTK_IS_BUILDABLE (buildable));
  g_return_if_fail (name != NULL);

  iface = GTK_BUILDABLE_GET_IFACE (buildable);

  if (iface->set_name)
    (* iface->set_name) (buildable, name);
  else
    g_object_set_data_full (G_OBJECT (buildable),
			    "gtk-builder-name",
			    g_strdup (name),
			    g_free);
}

/**
 * gtk_buildable_get_name:
 * @buildable: a #GtkBuildable
 *
 *   
 * Returns the buildable name. #GtkBuilder sets the name based on the 
 * the <link linkend="BUILDER-UI">GtkBuilder UI definition</link> used 
 * to construct the @buildable.
 *
 * #GtkWidget implements this to map the buildable name to the widget name
 *
 * Returns: the name set with gtk_buildable_set_name()
 *
 * Since: 2.12
 **/
const gchar *
gtk_buildable_get_name (GtkBuildable *buildable)
{
  GtkBuildableIface *iface;

  g_return_val_if_fail (GTK_IS_BUILDABLE (buildable), NULL);

  iface = GTK_BUILDABLE_GET_IFACE (buildable);

  if (iface->get_name)
    return (* iface->get_name) (buildable);
  else
    return (const gchar*)g_object_get_data (G_OBJECT (buildable),
					    "gtk-builder-name");
}

/**
 * gtk_buildable_add:
 * @buildable: a #GtkBuildable
 * @builder: a #GtkBuilder
 * @child: child to add
 * @type: kind of child or %NULL
 *
 * Add a child to a buildable. type is an optional string
 * describing how the child should be added.
 *
 * #GtkContainer implements this to be able to add a child widget
 * to the container. #GtkNotebook uses the @type to distinguish between
 * page labels (@type = "page-label") and normal children.
 *
 * Since: 2.12
 **/
void
gtk_buildable_add (GtkBuildable *buildable,
                   GtkBuilder   *builder,
                   GObject      *child,
                   const gchar  *type)
{
  GtkBuildableIface *iface;

  g_return_if_fail (GTK_IS_BUILDABLE (buildable));
  g_return_if_fail (GTK_IS_BUILDER (builder));

  iface = GTK_BUILDABLE_GET_IFACE (buildable);
  g_return_if_fail (iface->add != NULL);

  (* iface->add) (buildable, builder, child, type);
}

/**
 * gtk_buildable_set_property:
 * @buildable: a #GtkBuildable
 * @builder: a #GtkBuilder
 * @name: name of property
 * @value: value of property
 *
 * Sets the property name @name to @value on the buildable object @buildable
 * which is created by the @builder.
 *
 * This is optional to implement and is normally not needed.
 * g_object_set_property() is used as a fallback.
 *
 * #GtkWindow implements this to delay showing (::visible) itself until
 * the whole interface is fully created.
 *
 * Since: 2.12
 **/
void
gtk_buildable_set_property (GtkBuildable *buildable,
                            GtkBuilder   *builder,
                            const gchar  *name,
                            const GValue *value)
{
  GtkBuildableIface *iface;

  g_return_if_fail (GTK_IS_BUILDABLE (buildable));
  g_return_if_fail (GTK_IS_BUILDER (builder));
  g_return_if_fail (name != NULL);
  g_return_if_fail (value != NULL);

  iface = GTK_BUILDABLE_GET_IFACE (buildable);
  if (iface->set_property)
    (* iface->set_property) (buildable, builder, name, value);
  else
    g_object_set_property (G_OBJECT (buildable), name, value);
}

/**
 * gtk_buildable_parser_finished:
 * @buildable: a #GtkBuildable
 * @builder: a #GtkBuilder
 *
 * Finish the parsing of a <link linkend="BUILDER-UI">GtkBuilder UI definition</link>
 * snippet. Note that this will be called once for each time gtk_builder_add_from_file or
 * gtk_builder_add_from_string is called on a builder.
 *
 * #GtkWindow implements this to delay showing (::visible) itself until
 * the whole interface is fully created.
 *
 * Since: 2.12
 **/
void
gtk_buildable_parser_finished (GtkBuildable *buildable,
			       GtkBuilder   *builder)
{
  GtkBuildableIface *iface;

  g_return_if_fail (GTK_IS_BUILDABLE (buildable));
  g_return_if_fail (GTK_IS_BUILDER (builder));

  iface = GTK_BUILDABLE_GET_IFACE (buildable);
  if (iface->parser_finished)
    (* iface->parser_finished) (buildable, builder);
}

/**
 * gtk_buildable_construct_child
 * @buildable: A #GtkBuildable
 * @builder: #GtkBuilder used to construct this object
 * @name: name of child to construct
 *
 * Construct a child of @buildable with the name @name.
 *
 * #GtkUIManager implements this to reference to a widget created in a 
 * &lt;ui&gt; tag which is outside of the normal 
 * <link linkend="BUILDER-UI">GtkBuilder UI definition</link>
 * hierarchy.
 *
 * Returns: the child with name @name
 *
 * Since: 2.12
 **/
GObject *
gtk_buildable_construct_child (GtkBuildable *buildable,
                               GtkBuilder   *builder,
                               const gchar  *name)
{
  GtkBuildableIface *iface;

  g_return_val_if_fail (GTK_IS_BUILDABLE (buildable), NULL);
  g_return_val_if_fail (GTK_IS_BUILDER (builder), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  iface = GTK_BUILDABLE_GET_IFACE (buildable);
  g_return_val_if_fail (iface->construct_child != NULL, NULL);

  return (* iface->construct_child) (buildable, builder, name);
}

/**
 * gtk_buildable_custom_tag_start
 * @buildable: a #GtkBuildable
 * @builder: a #GtkBuilder used to construct this object
 * @child: child object or %NULL for non-child tags
 * @tagname: name of tag
 * @parser: a #GMarkupParser structure
 * @data: user data that will be passed in to parser functions
 *
 * This is called when an unknown tag under &lt;child&gt; tag is found.
 * 
 * Called when an unknown tag is present under a &lt;child&gt; tag.
 * If the buildable implementation wishes to handle the tag it should
 * return %TRUE and fill in the @parser structure. Remember to either
 * implement custom_tag_end or custom_tag_finish to free
 * the user data allocated here.
 *
 * #GtkWidget implements this and parsers all &lt;accelerator&gt; tags to
 * keyboard accelerators.
 * #GtkContainer implements this to map properties defined under
 * &lt;packing&gt; tag to child properties.
 *
 * Returns: %TRUE if a object has a custom implementation, %FALSE
 *          if it doesn't.
 *
 * Since: 2.12
 **/
gboolean
gtk_buildable_custom_tag_start (GtkBuildable  *buildable,
                                GtkBuilder    *builder,
                                GObject       *child,
                                const gchar   *tagname,
                                GMarkupParser *parser,
                                gpointer      *data)
{
  GtkBuildableIface *iface;

  g_return_val_if_fail (GTK_IS_BUILDABLE (buildable), FALSE);
  g_return_val_if_fail (GTK_IS_BUILDER (builder), FALSE);
  g_return_val_if_fail (tagname != NULL, FALSE);

  iface = GTK_BUILDABLE_GET_IFACE (buildable);
  g_return_val_if_fail (iface->custom_tag_start != NULL, FALSE);

  return (* iface->custom_tag_start) (buildable, builder, child,
                                      tagname, parser, data);
}

/**
 * gtk_buildable_custom_tag_end
 * @buildable: A #GtkBuildable
 * @builder: #GtkBuilder used to construct this object
 * @child: child object or %NULL for non-child tags
 * @tagname: name of tag
 * @data: user data that will be passed in to parser functions
 *
 * This is called for each custom tag handled by the buildable.
 * It will be called when the end of the tag is reached.
 *
 * Since: 2.12
 **/
void
gtk_buildable_custom_tag_end (GtkBuildable  *buildable,
                              GtkBuilder    *builder,
                              GObject       *child,
                              const gchar   *tagname,
                              gpointer      *data)
{
  GtkBuildableIface *iface;

  g_return_if_fail (GTK_IS_BUILDABLE (buildable));
  g_return_if_fail (GTK_IS_BUILDER (builder));
  g_return_if_fail (tagname != NULL);

  iface = GTK_BUILDABLE_GET_IFACE (buildable);
  if (iface->custom_tag_end)
    (* iface->custom_tag_end) (buildable, builder, child, tagname, data);
}

/**
 * gtk_buildable_custom_finished:
 * @buildable: a #GtkBuildable
 * @builder: a #GtkBuilder
 * @child: child object or %NULL for non-child tags
 * @tagname: the name of the tag
 * @data: user data created in custom_tag_start
 *
 * This is similar to gtk_buildable_parser_finished() but is
 * called once for each custom tag handled by the @buildable.
 * 
 * Since: 2.12
 **/
void
gtk_buildable_custom_finished (GtkBuildable  *buildable,
			       GtkBuilder    *builder,
			       GObject       *child,
			       const gchar   *tagname,
			       gpointer       data)
{
  GtkBuildableIface *iface;

  g_return_if_fail (GTK_IS_BUILDABLE (buildable));
  g_return_if_fail (GTK_IS_BUILDER (builder));

  iface = GTK_BUILDABLE_GET_IFACE (buildable);
  if (iface->custom_finished)
    (* iface->custom_finished) (buildable, builder, child, tagname, data);
}

/**
 * gtk_buildable_get_internal_child
 * @buildable: a #GtkBuildable
 * @builder: a #GtkBuilder
 * @childname: name of child
 *
 * Get the internal child called @childname of the @buildable object.
 *
 * Returns: the internal child of the buildable object 
 *
 * Since: 2.12
 **/
GObject *
gtk_buildable_get_internal_child (GtkBuildable *buildable,
                                  GtkBuilder   *builder,
                                  const gchar  *childname)
{
  GtkBuildableIface *iface;

  g_return_val_if_fail (GTK_IS_BUILDABLE (buildable), NULL);
  g_return_val_if_fail (GTK_IS_BUILDER (builder), NULL);
  g_return_val_if_fail (childname != NULL, NULL);

  iface = GTK_BUILDABLE_GET_IFACE (buildable);
  if (!iface->get_internal_child)
    return NULL;

  return (* iface->get_internal_child) (buildable, builder, childname);
}

#define __GTK_BUILDABLE_C__
#include "gtkaliasdef.c"
