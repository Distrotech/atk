/* ATK -  Accessibility Toolkit
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

#ifndef __ATK_HYPERTEXT_H__
#define __ATK_HYPERTEXT_H__

#include <atk/atkobject.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The AtkHypertext class is the base class for all classes that present 
 * hypertext information on the display. This class provides the standard 
 * mechanism for an assistive technology to access that text via its 
 * content, attributes, and spatial location. It also provides standard 
 * mechanisms for manipulating hyperlinks.
 *
 * I do not think that there is a GTK+ widget for this but I assume that 
 * there will be a custom widget so we define the interface.
 */

#define ATK_TYPE_HYPERTEXT                    (atk_hypertext_get_type ())
#define ATK_IS_HYPERTEXT(obj)                 G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATK_TYPE_HYPERTEXT)
#define ATK_HYPERTEXT(obj)                    G_TYPE_CHECK_INSTANCE_CAST ((obj), ATK_TYPE_HYPERTEXT, AtkHypertext)
#define ATK_HYPERTEXT_GET_IFACE(obj)          (G_TYPE_INSTANCE_GET_INTERFACE ((obj), ATK_TYPE_HYPERTEXT, AtkHypertextIface))

#ifndef _TYPEDEF_ATK_HYPERTEXT_
#define _TYPEDEF_ATK_HYPERTEXT_
typedef struct _AtkHypertext AtkHypertext;
#endif
typedef struct _AtkHypertextIface AtkHypertextIface;

typedef struct _AtkHyperLink  AtkHyperLink;

struct _AtkHypertextIface
{
  GTypeInterface parent;

  /*
   * Returns the nth link of this hypertext document
   */
  AtkHyperLink*(* get_link)                 (AtkHypertext       *hypertext,
                                             gint               link_index);
  /*
   * Returns the number of links within this hypertext document.
   */
  gint         (* get_n_links)              (AtkHypertext       *hypertext);
  /*
   * Returns the index into the array of hyperlinks that is associated 
   * this character index, or -1 if there is no hyperlink associated with
   * this index.
   */
  gint         (* get_link_index)           (AtkHypertext       *hypertext,
                                             gint               char_index);

};
GType atk_hypertext_get_type (void);

AtkHyperLink* atk_hypertext_get_link       (AtkHypertext *hypertext,
                                            gint          link_index);
gint          atk_hypertext_get_n_links    (AtkHypertext *hypertext);
gint          atk_hypertext_get_link_index (AtkHypertext *hypertext,
                                            gint          char_index);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ATK_HYPERTEXT_H__ */
