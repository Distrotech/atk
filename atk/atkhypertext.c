/* ATK - The Accessibility Toolkit for GTK+
 * Copyright 2001 Sun Microsystems Inc.
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

#include "atkhypertext.h"

struct _AtkHypertextIfaceClass
{
  GObjectClass parent;
};

typedef struct _AtkHypertextIfaceClass AtkHypertextIfaceClass;

static void atk_hypertext_interface_init (AtkHypertextIfaceClass *klass);

static gpointer parent_class = NULL;


GType
atk_hypertext_get_type ()
{
  static GType type = 0;

  if (!type) {
    static const GTypeInfo tinfo =
    {
      sizeof (AtkHypertextIface),
      NULL,
      NULL,
      (GInterfaceInitFunc) atk_hypertext_interface_init,
    };

    type = g_type_register_static (G_TYPE_INTERFACE, "AtkHypertext", &tinfo, 0);
  }

  return type;
}


/*
 */

static void
atk_hypertext_interface_init (AtkHypertextIfaceClass *klass)
{
  parent_class = g_type_class_ref (ATK_TYPE_HYPERTEXT);
}

/**
 *atk_hypertext_get_link:
 *@hypertext: an #AtkHypertext
 *@link_index: the index of the link to be returned
 *
 * Gets the link in this hypertext document at index 
 * @link_index
 *
 *Returns: the link in this hypertext document at
 * index @link_index
 **/
AtkHyperLink* 
atk_hypertext_get_link (AtkHypertext  *hypertext,
                        gint          link_index)
{
  AtkHypertextIface *iface;

  g_return_val_if_fail (hypertext != NULL, NULL);
  g_return_val_if_fail (ATK_IS_HYPERTEXT (hypertext), NULL);

  iface = ATK_HYPERTEXT_GET_IFACE (hypertext);

  if (iface->get_link)
    return (*(iface->get_link)) (hypertext, link_index);
  else
    return NULL;
}

/**
 *atk_hypertext_get_n_links:
 *@hypertext: an #AtkHypertext
 *
 * Gets the number of links within this hypertext document.
 *
 *Returns: the number of links within this hypertext document
 **/
gint 
atk_hypertext_get_n_links (AtkHypertext  *hypertext)
{
  AtkHypertextIface *iface;

  g_return_val_if_fail (hypertext != NULL, 0);
  g_return_val_if_fail (ATK_IS_HYPERTEXT (hypertext), 0);

  iface = ATK_HYPERTEXT_GET_IFACE (hypertext);

  if (iface->get_n_links)
    return (*(iface->get_n_links)) (hypertext);
  else
    return 0;
}

/**
 *atk_hypertext_get_link_index:
 *@hypertext: an #AtkHypertext
 *@char_index: a character index
 *
 * Gets the index into the array of hyperlinks that is associated with
 * @char_index character index, or -1 if there is no hyperlink associated with
 * @char_index.
 *
 * Returns: the index into the array of hyperlinks that is associated with
 * @char_index character index, or -1 if there is no hyperlink associated with
 * @char_index.
 **/
gint 
atk_hypertext_get_link_index (AtkHypertext  *hypertext,
                              gint          char_index)
{
  AtkHypertextIface *iface;

  g_return_val_if_fail (hypertext != NULL, -1);
  g_return_val_if_fail (ATK_IS_HYPERTEXT (hypertext), -1);

  iface = ATK_HYPERTEXT_GET_IFACE (hypertext);

  if (iface->get_link_index)
    return (*(iface->get_link_index)) (hypertext, char_index);
  else
    return -1;
}
