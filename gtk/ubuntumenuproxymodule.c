/*
 * Copyright (C) 2010 Canonical, Ltd.
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
 *
 * Authors: Cody Russell <bratsche@gnome.org>
 */

#include "config.h"
#include "gtkintl.h"
#include "ubuntumenuproxy.h"
#include "ubuntumenuproxymodule.h"
#include "gtkmodules.h"
#include "gtkalias.h"

enum {
  PROP_0,
  PROP_MODULENAME
};

static GObject  *ubuntu_menu_proxy_module_constructor   (GType                  type,
                                                         guint                  n_params,
                                                         GObjectConstructParam *params);
static void      ubuntu_menu_proxy_module_finalize      (GObject               *object);
static gboolean  ubuntu_menu_proxy_module_real_load     (GTypeModule           *gmodule);
static void      ubuntu_menu_proxy_module_real_unload   (GTypeModule           *gmodule);


G_DEFINE_TYPE (UbuntuMenuProxyModule, ubuntu_menu_proxy_module, G_TYPE_TYPE_MODULE);

static UbuntuMenuProxyModule *proxy_module_singleton = NULL;

static void
ubuntu_menu_proxy_module_class_init (UbuntuMenuProxyModuleClass *class)
{
  GObjectClass     *object_class      = G_OBJECT_CLASS (class);
  GTypeModuleClass *type_module_class = G_TYPE_MODULE_CLASS (class);

  object_class->finalize     = ubuntu_menu_proxy_module_finalize;

  type_module_class->load    = ubuntu_menu_proxy_module_real_load;
  type_module_class->unload  = ubuntu_menu_proxy_module_real_unload;
}

static void
ubuntu_menu_proxy_module_init (UbuntuMenuProxyModule *module)
{
  module->name     = g_strdup (g_getenv ("UBUNTU_MENUPROXY"));
  module->library  = NULL;
  module->load     = NULL;
  module->unload   = NULL;
}

static void
ubuntu_menu_proxy_module_finalize (GObject *object)
{
  UbuntuMenuProxyModule *module = UBUNTU_MENU_PROXY_MODULE (object);

  if (module->name != NULL)
    {
      g_free (module->name);
    }

  G_OBJECT_CLASS (ubuntu_menu_proxy_module_parent_class)->finalize (object);
}

static gboolean
ubuntu_menu_proxy_module_real_load (GTypeModule *gmodule)
{
  UbuntuMenuProxyModule *module = UBUNTU_MENU_PROXY_MODULE (gmodule);
  gchar *path;

  if (proxy_module_singleton != NULL)
    return TRUE;

  if (!module->name)
    {
      g_warning ("Module path not set");
      return FALSE;
    }

  path = _gtk_find_module (module->name, "menuproxies");

  module->library = g_module_open (path, 0);

  if (!module->library)
    {
      g_printerr ("%s\n", g_module_error ());
      return FALSE;
    }

  /* Make sure that the loaded library contains the required methods */
  if (!g_module_symbol (module->library,
                        "menu_proxy_module_load",
                        (gpointer *) &module->load) ||
      !g_module_symbol (module->library,
                        "menu_proxy_module_unload",
                        (gpointer *) &module->unload))
    {
      g_printerr ("%s\n", g_module_error ());
      g_module_close (module->library);

      return FALSE;
    }

  /* Initialize the loaded module */
  module->load (module);

  return TRUE;
}

static void
ubuntu_menu_proxy_module_real_unload (GTypeModule *gmodule)
{
  UbuntuMenuProxyModule *module = UBUNTU_MENU_PROXY_MODULE (gmodule);

  module->unload (module);

  g_module_close (module->library);
  module->library = NULL;

  module->load   = NULL;
  module->unload = NULL;
}

static gboolean
is_valid_module_name (const gchar *name)
{
#if !defined(G_OS_WIN32) && !defined(G_WITH_CYGWIN)
  return g_str_has_prefix (name, "lib") && g_str_has_suffix (name, ".so");
#else
  return g_str_has_suffix (name, ".dll");
#endif
}

static void
setup_instance (UbuntuMenuProxyModule *module)
{
  GType *proxy_types;
  guint  n_proxies;

  proxy_types = g_type_children (UBUNTU_TYPE_MENU_PROXY,
                                 &n_proxies);

  if (n_proxies > 1)
    {
      g_warning ("There are %d child types of UbuntuMenuProxy, should be 0 or 1.\n",
                 n_proxies);
    }
  else if (n_proxies == 1)
    {
      g_object_new (proxy_types[0], NULL);
    }
}

UbuntuMenuProxyModule *
ubuntu_menu_proxy_module_get (void)
{
  if (!proxy_module_singleton)
    {
      UbuntuMenuProxyModule *module = NULL;
      const gchar *module_name;

      module_name = g_getenv ("UBUNTU_MENUPROXY");

      if (module_name != NULL)
        {
          if (is_valid_module_name (module_name))
            {
              gchar *path = _gtk_find_module (module_name, "menuproxies");

              module = g_object_new (UBUNTU_TYPE_MENU_PROXY_MODULE,
                                     NULL);

              if (!g_type_module_use (G_TYPE_MODULE (module)))
                {
                  g_warning ("Failed to load type module: %s\n", path);

                  g_object_unref (module);
                  g_free (path);

                  return NULL;
                }

              setup_instance (module);

              g_free (path);
              g_type_module_unuse (G_TYPE_MODULE (module));
            }

          proxy_module_singleton = module;
        }
    }

  return proxy_module_singleton;
}

#define __UBUNTU_MENU_PROXY_MODULE_C__
#include "gtkaliasdef.c"
