/* ATK -  Accessibility Toolkit
 * Copyright 2001 Sun Microsystems Inc.
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

#include <glib-object.h>

#include "atk.h"

#define NUM_POSSIBLE_STATES                     (sizeof(AtkStateMask)*8)

/* New GObject properties registered by AtkObject */
enum
{
  PROP_0,  /* gobject convention */

  PROP_NAME,
  PROP_DESCRIPTION,
  PROP_PARENT,      /* ancestry has changed */
  PROP_CHILD,       /* children have changed in number or identity */
  PROP_ROLE,        /* AtkRole has changed  */
  PROP_STATE,       /* AtkState has changed */
  PROP_TEXT,        /* Used only by AtkText implementors */
  PROP_CARET,       /* Used only by AtkText implementors */
  PROP_LAST         /* gobject convention */
};

enum {
  CHILDREN_CHANGED,
  LAST_SIGNAL
};

struct _AtkRelationSet
{
  GArray     *relations;
};

struct _AtkRelation
{
  GArray              *target;
  AtkRelationType     relationship;
};


typedef struct
{
  GObject  object;
} _AtkIfaceImplementor;

static void            atk_object_class_init       (AtkObjectClass  *klass);
static void            atk_object_init             (AtkObject       *accessible,
                                                    AtkObjectClass  *klass);
static AtkRelationSet* atk_object_real_get_relation_set (AtkObject *accessible);

static void            atk_object_real_set_property(GObject         *object,
                                                    guint            prop_id,
                                                    const GValue    *value,
                                                    GParamSpec      *pspec);
static void            atk_object_real_get_property(GObject         *object,
                                                    guint            prop_id,
                                                    GValue          *value,
                                                    GParamSpec      *pspec);
static void            atk_object_finalize         (GObject         *object);

static gchar* state_names[NUM_POSSIBLE_STATES];

#if 0
static guint atk_object_signals[LAST_SIGNAL] = { 0, };
#endif

static gpointer parent_class = NULL;

static const gchar* atk_object_name_property_name = "accessible-name";
static const gchar* atk_object_name_property_state = "accessible-state";
static const gchar* atk_object_name_property_description = "accessible-description";

GType
atk_object_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (AtkObjectClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) atk_object_class_init,
        (GClassFinalizeFunc) NULL,
        NULL,
        sizeof (AtkObject),
        0,
        (GInstanceInitFunc) atk_object_init,
      } ;
      type = g_type_register_static (G_TYPE_OBJECT, "AtkObject", &typeInfo, 0) ;
    }
  return type;
}

static void
atk_object_class_init (AtkObjectClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  gobject_class->set_property = atk_object_real_set_property;
  gobject_class->get_property = atk_object_real_get_property;
  gobject_class->finalize = atk_object_finalize;

  klass->get_relation_set = atk_object_real_get_relation_set;

  state_names[ATK_STATE_ARMED]           = "armed";
  state_names[ATK_STATE_BUSY]            = "busy";
  state_names[ATK_STATE_CHECKED]         = "checked";
  state_names[ATK_STATE_COLLAPSED]       = "collapsed";
  state_names[ATK_STATE_EDITABLE]        = "editable";
  state_names[ATK_STATE_EXPANDABLE]      = "expandable";
  state_names[ATK_STATE_EXPANDED]        = "expanded";
  state_names[ATK_STATE_FOCUSABLE]       = "focusable";
  state_names[ATK_STATE_FOCUSED]         = "focused";
  state_names[ATK_STATE_HORIZONTAL]      = "horizontal";
  state_names[ATK_STATE_ICONIFIED]       = "iconified";
  state_names[ATK_STATE_MODAL]           = "modal";
  state_names[ATK_STATE_MULTI_LINE]      = "multi-line";
  state_names[ATK_STATE_MULTISELECTABLE] = "multiselectable";
  state_names[ATK_STATE_OPAQUE]          = "opaque";
  state_names[ATK_STATE_PRESSED]         = "pressed";
  state_names[ATK_STATE_RESIZABLE]       = "resizeable";
  state_names[ATK_STATE_SELECTABLE]      = "selectable";
  state_names[ATK_STATE_SELECTED]        = "selected";
  state_names[ATK_STATE_SENSITIVE]       = "sensitive";
  state_names[ATK_STATE_SHOWING]         = "showing";
  state_names[ATK_STATE_SINGLE_LINE]     = "single-line";
  state_names[ATK_STATE_TRANSIENT]       = "transient";
  state_names[ATK_STATE_VERTICAL]        = "vertical";
  state_names[ATK_STATE_VISIBLE]         = "visible";

  klass->children_changed = NULL;

  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string (atk_object_name_property_name,
                                                        "Accessible Name",
                                                        "Object instance\'s name formatted for "
                                                           "assistive technology access",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_DESCRIPTION,
                                   g_param_spec_string (atk_object_name_property_description,
                                                        "Accessible Description",
                                                        "Description of an object, formatted for "
                                                        "assistive technology access",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_STATE,
                                   g_param_spec_int    (atk_object_name_property_state,
                                                        "Accessible State",
                                                        "The current state of this object "
                                                        "or its UI component",
                                                        0,
                                                        G_MAXINT,
                                                        0,
                                                        G_PARAM_WRITABLE));
#if 0
  /* register some properties - these could be change signals instead */
  g_object_class_install_property (gobject_class,
                                   PROP_PARENT,
                                   g_param_spec_ccallback ("accessible_parent", "Accessible Parent",
                                                           "First accessible ancestor (container or object) "
                                                           "which this object is a descendant of",
                                                           G_PARAM_READWRITE));


  g_object_class_install_property (gobject_class,
                                   PROP_ROLE,
                                   g_param_spec_ccallback ("accessible_role", "Accessible Role",
                                                           "The user-interface role of this object",
                                                           G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_STATE,
                                   g_param_spec_ccallback ("accessible_state", "Accessible State",
                                                           "The current state of this object "
                                                           "or its UI component",
                                                           G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_TEXT,
                                   g_param_spec_ccallback ("accessible_text", "Accessible Text",
                                                           "This object\'s accessible text contents",
                                                           G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class,
                                   PROP_CARET,
                                   g_param_spec_ccallback ("accessible_caret", "Accessible Text Caret",
                                                           "The current text caret state and position "
                                                           "for this component",
                                                           G_PARAM_READWRITE));
  gaccessible_signals[CHILDREN_CHANGED] =
    g_signal_newc ("accessible_children_changed",
                   G_TYPE_FROM_CLASS (klass),
                   G_SIGNAL_RUN_LAST,
                   G_STRUCT_OFFSET (AtkObjectClass, children_changed), /* still need to declare and define this func */
                   NULL,
                   g_cclosure_marshal_VOID__UINT_POINTER,
                   G_TYPE_NONE,
                   2, G_TYPE_UINT, ATK_TYPE_OBJECT);
#endif
}

static void
atk_object_init  (AtkObject        *accessible,
                  AtkObjectClass   *klass)
{
    accessible->relation_set = g_malloc (sizeof (AtkRelationSet));
    g_return_if_fail (accessible->relation_set != NULL);
    accessible->relation_set->relations = NULL;
}

GType
atk_object_iface_get_type (void)
{
  static GType type = 0;

  if (!type)
    {
      static const GTypeInfo typeInfo =
      {
        sizeof (AtkObjectIface),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
      } ;

      type = g_type_register_static (G_TYPE_INTERFACE, "AtkObjectIface", &typeInfo, 0) ;
    }

  return type;
}

G_CONST_RETURN gchar*
atk_object_get_name (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_name)
    return (klass->get_name) (accessible);
  else
    return NULL;
}

G_CONST_RETURN gchar*
atk_object_get_description (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_description)
    return (klass->get_description) (accessible);
  else
    return NULL;
}

AtkObject*
atk_object_get_parent (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_parent)
    return (klass->get_parent) (accessible);
  else
    return NULL;
}

gint
atk_object_get_n_accessible_children (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), 0);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), 0);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_n_children)
    return (klass->get_n_children) (accessible);
  else
    return 0;
}

AtkObject*
atk_object_ref_accessible_child (AtkObject   *accessible,
                                 gint        i)
{
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->ref_child)
    return (klass->ref_child) (accessible, i);
  else
    return NULL;
}

AtkRelationSet*
atk_object_get_relation_set (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), NULL);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), NULL);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_relation_set)
    return (klass->get_relation_set) (accessible);
  else
    return NULL;
}

AtkRole
atk_role_register (gchar *name)
{
  /* TODO: associate name with new type */
  static guint type = ATK_ROLE_LAST_DEFINED;
  return (++type);
}

AtkRole
atk_object_get_role (AtkObject *accessible) {
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), ATK_ROLE_UNKNOWN);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), ATK_ROLE_UNKNOWN);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_role)
    return (klass->get_role) (accessible);
  else
    return ATK_ROLE_UNKNOWN;
}

AtkStateType
atk_state_type_register (gchar *name)
{
  /* TODO: associate name with new type */
  static guint type = ATK_STATE_LAST_DEFINED;
  if (type < NUM_POSSIBLE_STATES) {
    return (++type);
  }
  return ATK_STATE_INVALID; /* caller needs to check */
}

AtkState
atk_object_get_state (AtkObject *accessible) {
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), 0);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), 0);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_state)
    return (klass->get_state) (accessible);
  else
    return 0;
}

gint
atk_object_get_index_in_parent (AtkObject *accessible)
{
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), -1);
  g_return_val_if_fail (ATK_OBJECT (accessible), -1);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->get_index_in_parent)
    return (klass->get_index_in_parent) (accessible);
  else
    return -1;
}

void
atk_object_set_name (AtkObject    *accessible,
                     const gchar  *name)
{
  AtkObjectClass *klass;

  g_return_if_fail ((accessible != NULL));
  g_return_if_fail (ATK_IS_OBJECT (accessible));
  g_return_if_fail ((name != NULL));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_name)
  {
    (klass->set_name) (accessible, name);
    g_object_notify (G_OBJECT (accessible), atk_object_name_property_name);
  }
}

void
atk_object_set_description (AtkObject   *accessible,
                            const gchar *description)
{
  AtkObjectClass *klass;

  g_return_if_fail ((accessible != NULL));
  g_return_if_fail (ATK_IS_OBJECT (accessible));
  g_return_if_fail ((description != NULL));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_description)
  {
    (klass->set_description) (accessible, description);
    g_object_notify (G_OBJECT (accessible), atk_object_name_property_description);
  }
}

void
atk_object_set_parent (AtkObject *accessible,
                       AtkObject *parent)
{
  AtkObjectClass *klass;

  g_return_if_fail ((accessible != NULL));
  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_parent)
    (klass->set_parent) (accessible, parent);
}

void
atk_object_set_role (AtkObject *accessible, 
                     AtkRole   role)
{
  AtkObjectClass *klass;

  g_return_if_fail ((accessible != NULL));
  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->set_role)
    (klass->set_role) (accessible, role);
}

guint
atk_object_connect_property_change_handler (AtkObject *accessible,
                                            AtkPropertyChangeHandler *handler)
{
  AtkObjectClass *klass;

  g_return_val_if_fail ((accessible != NULL), 0);
  g_return_val_if_fail (ATK_IS_OBJECT (accessible), 0);
  g_return_val_if_fail ((handler != NULL), 0);

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->connect_property_change_handler)
    return (klass->connect_property_change_handler) (accessible, handler);
  else
    return 0;
}

void
atk_object_remove_property_change_handler  (AtkObject *accessible,
                                            guint      handler_id)
{
  AtkObjectClass *klass;

  g_return_if_fail ((accessible != NULL));
  g_return_if_fail (ATK_IS_OBJECT (accessible));

  klass = ATK_OBJECT_GET_CLASS (accessible);
  if (klass->remove_property_change_handler)
    (klass->remove_property_change_handler) (accessible, handler_id);
}

AtkRelationType
atk_relation_type_register (gchar *name)
{
  /* TODO: associate name with new type */
  static guint type = ATK_RELATION_LAST_DEFINED;
  return (++type);
}

AtkRelation*
atk_relation_new (GArray          *target,
                  AtkRelationType relationship)
{
  AtkRelation* relation;
  g_return_val_if_fail ((target != NULL), NULL);

  relation = (AtkRelation *) g_malloc (sizeof (AtkRelation));

  g_return_val_if_fail ((relation != NULL), NULL);

  relation->target = target;
  relation->relationship = relationship;

  return relation;
}

gboolean
atk_relation_set_contains (AtkRelationSet   *set,
                           AtkRelationType  relationship)
{
  GArray *array_item;
  AtkRelation *item;
  gint  i;

  g_return_val_if_fail ((set != NULL), FALSE);

  array_item = set->relations;
  if (array_item == NULL)
    return FALSE;
  for (i = 0; i < array_item->len; i++)
  {
    item = g_array_index (array_item, AtkRelation*, i);
    if (item->relationship == relationship)
      return TRUE;
  }
  return FALSE;
}

void
atk_relation_set_remove (AtkRelationSet *set,
                         AtkRelation    *relation)
{
  GArray *array_item;
  AtkRelation *item;
  gint  i;

  g_return_if_fail (set != NULL);
  g_return_if_fail (relation != NULL);

  array_item = set->relations;
  if (array_item == NULL)
    return;
  for (i = 0; i < array_item->len; i++)
  {
    item = g_array_index (array_item, AtkRelation*, i);
    if (item == relation)
    {
      g_array_remove_index (array_item, i);
      return;
    }
  }
}

void
atk_relation_set_add (AtkRelationSet *set,
                      AtkRelation    *relation)
{
  g_return_if_fail (set != NULL);
  g_return_if_fail (relation != NULL);

  if (set->relations == NULL)
  {
    set->relations = g_array_new (FALSE, TRUE, sizeof (AtkRelation));
  }
  set->relations = g_array_append_val (set->relations, relation);
}

gint
atk_relation_set_get_n_relations (AtkRelationSet *set)
{
  g_return_val_if_fail (set != NULL, 0);

  if (set->relations == NULL)
    return 0;

  return set->relations->len;
}

AtkRelation*
atk_relation_set_get_relation (AtkRelationSet *set,
                               gint           i)
{
  GArray *array_item;
  AtkRelation* item;

  g_return_val_if_fail (set != NULL, NULL);
  g_return_val_if_fail (i >= 0, NULL);

  array_item = set->relations;
  if (array_item == NULL)
    return NULL;
  item = g_array_index (array_item, AtkRelation*, i);
  if (item == NULL)
    return NULL;

  return item;
}

AtkRelation*
atk_relation_set_get_relation_by_type (AtkRelationSet  *set,
                                       AtkRelationType relationship)
{
  GArray *array_item;
  AtkRelation *item;
  gint i;

  g_return_val_if_fail (set != NULL, NULL);

  array_item = set->relations;
  if (array_item == NULL)
    return NULL;
  for (i = 0; i < array_item->len; i++)
  {
    item = g_array_index (array_item, AtkRelation*, i);
    if (item->relationship == relationship)
      return item;
  }
  return NULL;
}

AtkRelationType
atk_relation_get_type (AtkRelation *relation)
{
  g_return_val_if_fail (relation != NULL, 0);
  return relation->relationship;
}

GArray*
atk_relation_get_target (AtkRelation *relation)
{
  g_return_val_if_fail (relation != NULL, 0);
  return relation->target;
}

gchar*
atk_state_mask_get_name (AtkStateMask state)
{
  gint n;

  if (state == 0)
    return NULL;

  for (n=0; n<NUM_POSSIBLE_STATES; ++n)
    {
      /* fall through and return null if multiple bits are set */
      if (state == (1 << n)) return state_names[n];
    }

  return NULL;
}

AtkStateMask
atk_state_mask_for_name (gchar *name)
{
  gint i;

  g_return_val_if_fail ((strlen(name)>0), 0);
  for (i=0; i<NUM_POSSIBLE_STATES; ++i)
    {
      if (!strcmp(name, state_names[i])) return ATK_STATE(i);
    }
  return 0;
}

/**
 * Return a reference to an object's AtkObject implementation, if
 * the object implements AtkObjectIface.
 * @object: The GObject instance which should implement #AtkObjectIface
 * if a non-null return value is required.
 */
AtkObject *
atk_object_ref_accessible (AtkIfaceImplementor *object)
{
  AtkObjectIface     *iface;
  AtkObject          *accessible = NULL;

  g_return_val_if_fail ((object != NULL), NULL);
  g_return_val_if_fail ((iface = ATK_OBJECT_GET_IFACE (object)), NULL );

  if (iface != NULL) accessible =  (*(iface->ref_accessible)) (object) ;

  g_return_val_if_fail ((accessible != NULL), NULL);

  return ATK_OBJECT (accessible) ;
}

AtkRelationSet*
atk_object_real_get_relation_set (AtkObject *accessible)
{
    return accessible->relation_set;
}

static void
atk_object_real_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  AtkObject *accessible;

  accessible = ATK_OBJECT (object);

  switch (prop_id)
  {
    case PROP_NAME:
      atk_object_set_name (accessible, g_value_get_string (value));
      break;
    case PROP_DESCRIPTION:
      atk_object_set_description (accessible, g_value_get_string (value));
      break;
    case PROP_STATE:
      g_print ("This interface does not support setting the state of an accessible object\n");
      break;
    default:
      break;
  }
}

static void
atk_object_real_get_property (GObject      *object,
                         guint         prop_id,
                         GValue       *value,
                         GParamSpec   *pspec)
{
  AtkObject *accessible;

  accessible = ATK_OBJECT (object);

  switch (prop_id)
  {
    case PROP_NAME:
      g_value_set_string (value, atk_object_get_name (accessible));
      break;
    case PROP_DESCRIPTION:
      g_value_set_string (value, atk_object_get_description (accessible));
      break;
    case PROP_STATE:
      g_value_set_int (value, atk_object_get_state (accessible));
      break;
    default:
      break;
  }
}

static void
atk_object_finalize (GObject *object)
{
  AtkObject        *accessible;
  GArray           *relations;

  g_return_if_fail (ATK_IS_OBJECT (object));

  accessible = ATK_OBJECT (object);

  g_free (accessible->name);
  g_free (accessible->description);

  /*
   * Free memory allocated for relations and relation sets;
   */
  relations = accessible->relation_set->relations;
  if (relations)
  {
    gint        len = relations->len;
    gint        i;
    AtkRelation *relation;

    for (i = 0; i < len; i++)
    {
      relation = g_array_index (relations, AtkRelation*, i);
      g_array_free (relation->target, TRUE);
    }
    g_array_free (relations, TRUE);
  }
  g_free (accessible->relation_set);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}
