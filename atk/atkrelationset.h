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

#ifndef __ATK_RELATION_SET_H__
#define __ATK_RELATION_SET_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <glib-object.h>
#include <atk/atkobject.h>
#include <atk/atkrelation.h>

#define ATK_TYPE_RELATION_SET                     (atk_relation_set_get_type ())
#define ATK_RELATION_SET(obj)                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), ATK_TYPE_RELATION_SET, AtkRelationSet))
#define ATK_RELATION_SET_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), ATK_TYPE_RELATION_SET, AtkRelationSetClass))
#define ATK_IS_RELATION_SET(obj)                  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ATK_TYPE_RELATION_SET))
#define ATK_IS_RELATION_SET_CLASS(klass)          (G_TYPE_CHECK_CLASS_TYPE ((klass), ATK_TYPE_RELATION_SET))
#define ATK_RELATION_SET_GET_CLASS(obj)           (G_TYPE_INSTANCE_GET_CLASS ((obj), ATK_TYPE_RELATION_SET, AtkRelationSetClass))

typedef struct _AtkRelationSetClass       AtkRelationSetClass;


struct _AtkRelationSet
{
  GObject parent;

  GArray *relations;
};

struct _AtkRelationSetClass
{
  GObjectClass parent;
};

GType atk_relation_set_get_type (void);

AtkRelationType atk_relation_type_register            (const gchar *name);

/*
 * Create a new relation set.
 */
AtkRelationSet*    atk_relation_set_new               (void);
/*
 * Returns whether the relation set contains a relation that matches the
 * specified type.
 */
gboolean        atk_relation_set_contains             (AtkRelationSet  *set,
                                                       AtkRelationType relationship);
/*
 * Remove a relation from the from the relation set.
 */
void            atk_relation_set_remove               (AtkRelationSet  *set,
                                                       AtkRelation     *relation);
/*
 * Add a new relation to the current relation set if it is not already
 * present.
 */
void            atk_relation_set_add                  (AtkRelationSet  *set,
                                                       AtkRelation     *relation);
/*
 * Returns the number of relations in a relation set.
 */
gint            atk_relation_set_get_n_relations      (AtkRelationSet  *set);
/*
 * Returns the relation at the specified position in the relation set.
 */
AtkRelation*    atk_relation_set_get_relation         (AtkRelationSet  *set,
                                                       gint            i);
/*
 * Returns a relation that matches the specified type.
 */
AtkRelation*    atk_relation_set_get_relation_by_type (AtkRelationSet  *set,
                                                       AtkRelationType relationship);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ATK_RELATION_SET_H__ */