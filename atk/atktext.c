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

#include "atktext.h"
#include "atkmarshal.h"

enum {
  TEXT_CHANGED,
  CARET_MOVED,
  LAST_SIGNAL
};

struct _AtkTextIfaceClass
{
  GObjectClass parent;
};

typedef struct _AtkTextIfaceClass AtkTextIfaceClass;

static void atk_text_base_init (gpointer *g_class);

static guint atk_text_signals[LAST_SIGNAL] = { 0 };

GType
atk_text_get_type ()
{
  static GType type = 0;

  if (!type) 
    {
      static const GTypeInfo tinfo =
      {
        sizeof (AtkTextIface),
        (GBaseInitFunc) atk_text_base_init,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) NULL /* atk_text_interface_init */ ,
        (GClassFinalizeFunc) NULL,

      };

      type = g_type_register_static (G_TYPE_INTERFACE, "AtkText", &tinfo, 0);
    }

  return type;
}

static void
atk_text_base_init (gpointer *g_class)
{
  static gboolean initialized = FALSE;

  if (! initialized)
    {
   /* 
    * Note that text_changed signal supports details "insert", "delete", 
    * possibly "replace". 
    */

    atk_text_signals[TEXT_CHANGED] =
      g_signal_newc ("text_changed",
                     ATK_TYPE_TEXT,
                     G_SIGNAL_RUN_LAST | G_SIGNAL_DETAILED,
                     G_STRUCT_OFFSET (AtkTextIface, text_changed), 
                     (GSignalAccumulator) NULL, NULL,
                     atk_marshal_VOID__INT_INT,
                     G_TYPE_NONE,
                     2, G_TYPE_INT, G_TYPE_INT);

    atk_text_signals[CARET_MOVED] =
      g_signal_newc ("text_caret_moved",
                     ATK_TYPE_TEXT,
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET (AtkTextIface, caret_changed),
                     (GSignalAccumulator) NULL, NULL,
                     g_cclosure_marshal_VOID__INT,
                     G_TYPE_NONE,
                     1, G_TYPE_INT);

  initialized = TRUE;
  }
}

/**
 * atk_text_get_text:
 * @text: an #AtkText
 * @start_offset: start position
 * @end_offset: end position
 *
 * Gets the specified text.
 *
 * Returns: the text from @start_offset up to, but not including @end_offset.
 **/
gchar*
atk_text_get_text (AtkText      *text,
                   gint         start_offset,
                   gint         end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text)
    return (*(iface->get_text)) (text, start_offset, end_offset);
  else
    return NULL;
}

/**
 * atk_text_get_character_at_offset
 * @text: an #AtkText
 * @offset: position
 *
 * Gets the specified text.
 *
 * Returns: the character at @offset.
 **/
gunichar
atk_text_get_character_at_offset (AtkText      *text,
                                  gint         offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, (gunichar) 0);
  g_return_val_if_fail (ATK_IS_TEXT (text), (gunichar) 0);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_at_offset)
    return (*(iface->get_character_at_offset)) (text, offset);
  else
    return (gunichar) 0;
}

/**
 * atk_text_get_text_after_offset
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 *
 * Gets the specified text.
 * If the boundary type is ATK_TEXT_BOUNDARY_WORD_START or
 * ATK_TEXT_BOUNDARY_WORD_END part of a word may be returned.
 * If the boundary type is ATK_TEXT_BOUNDARY_SENTENCE_START the start point 
 * will be the offset and will continue to the start of the next sentence. 
 * The first word may not be a complete word. Similarly for 
 * ATK_TEXT_BOUNDARY_SENTENCE_END, ATK_TEXT_BOUNDARY_LINE_START and
 * ATK_TEXT_BOUNDARY_LINE_END
 *
 * Returns: the text after @offset up to the specified @boundary_type.
 **/
gchar*
atk_text_get_text_after_offset (AtkText          *text,
                                gint             offset,
                                AtkTextBoundary  boundary_type)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_after_offset)
    return (*(iface->get_text_after_offset)) (text, offset, boundary_type);
  else
    return NULL;
}

/**
 * atk_text_get_text_at_offset
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 *
 * Gets the specified text.
 * If the boundary_type is ATK_TEXT_BOUNDARY_WORD_START or 
 * ATK_TEXT_BOUNDARY_WORD_END a complete word is returned; 
 * if the boundary type is  ATK_TEXT_BOUNDARY_SENTENCE_START or 
 * ATK_TEXT_BOUNDARY_SENTENCE_END a complete sentence
 * is returned; if the boundary type is ATK_TEXT_BOUNDARY_LINE_START or
 * ATK_TEXT_BOUNDARY_LINE_END a complete line is returned.
 *
 * Returns: the text at @offset up to the specified @boundary_type.
 **/
gchar*
atk_text_get_text_at_offset (AtkText          *text,
                             gint             offset,
                             AtkTextBoundary  boundary_type)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_at_offset)
    return (*(iface->get_text_at_offset)) (text, offset, boundary_type);
  else
    return NULL;
}

/**
 * atk_text_get_text_before_offset
 * @text: an #AtkText
 * @offset: position
 * @boundary_type: An #AtkTextBoundary
 *
 * Gets the specified text.
 * If the boundary type is ATK_TEXT_BOUNDARY_WORD_START or
 * ATK_TEXT_BOUNDARY_WORD_END part of a word may be returned.
 * If the boundary type is ATK_TEXT_BOUNDARY_SENTENCE_START the start point 
 * will be at the start of the sentence, and will continue to the offset. 
 * The last word may not be a complete word. Similarly for 
 * ATK_TEXT_BOUNDARY_SENTENCE_END, ATK_TEXT_BOUNDARY_LINE_START and
 * ATK_TEXT_BOUNDARY_LINE_END
 *
 * Returns: the text before @offset starting from the specified @boundary_type.
 **/
gchar*
atk_text_get_text_before_offset (AtkText          *text,
                                 gint             offset,
                                 AtkTextBoundary  boundary_type)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_text_before_offset)
    return (*(iface->get_text_before_offset)) (text, offset, boundary_type);
  else
    return NULL;
}

/**
 * atk_text_get_caret_offset
 * @text: an #AtkText
 *
 * Gets the offset position of the caret (cursor).
 *
 * Returns: the offset position of the caret (cursor).
 **/
gint
atk_text_get_caret_offset (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, -1);
  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_caret_offset)
    return (*(iface->get_caret_offset)) (text);
  else
    return -1;
}

/**
 * atk_text_get_row_col_at_offset
 * @text: an #AtkText
 * @offset: position
 * @row: row number
 * @col: column number
 *
 * Given an @offset, the @row and @col arguments are filled appropriately.
 **/
void
atk_text_get_row_col_at_offset (AtkText *text,
                                gint offset,
                                gint *row,
                                gint *col)
{
  AtkTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_TEXT (text));

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_row_col_at_offset)
    (*(iface->get_row_col_at_offset)) (text, offset, row, col);
  else
    {
      *row = 0;
      *col = 0;
    }
}

/**
 * atk_text_get_range_attributes
 * @text: an #AtkText
 * @start_offset: start position
 * @end_offset: end position
 *
 * Gets attributes over the specified range.
 *
 * Returns: a #PangoAttrList with the text attributes between the
 * @start_offset and the @end_offset.
 **/
PangoAttrList*
atk_text_get_range_attributes (AtkText *text,
                               gint start_offset,
                               gint end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_range_attributes)
    return (*(iface->get_range_attributes)) (text, start_offset, end_offset);
  else
    return NULL;
}

/**
 * atk_text_get_character_extents
 * @text: an #AtkText
 * @offset: position
 * @x: x-position of character
 * @y: y-position of character
 * @length: length of character
 * @width: width of character
 *
 * Given an @offset, the @x, @y, @length, and @width values are filled
 * appropriately.
 **/
void
atk_text_get_character_extents (AtkText *text,
                                gint offset,
                                gint *x,
                                gint *y,
                                gint *length,
                                gint *width)
{
  AtkTextIface *iface;

  g_return_if_fail (text != NULL);
  g_return_if_fail (ATK_IS_TEXT (text));

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_extents)
    (*(iface->get_character_extents)) (text, offset, x, y, length, width);
  else
    {
      *x = 0;
      *x = 0;
      *length = 0;
      *width = 0;
    }
}

/**
 * atk_text_get_character_count
 * @text: an #AtkText
 *
 * Gets the character count.
 *
 * Returns: the number of characters.
 **/
gint
atk_text_get_character_count (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, -1);
  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_character_count)
    return (*(iface->get_character_count)) (text);
  else
    return -1;
}

/**
 * atk_text_get_offset_at_point
 * @text: an #AtkText
 * @x: screen x-position of character
 * @y: screen y-position of character
 *
 * Gets the x,y screen coordinates of the specified character.
 *
 * Returns: the offset to the character which is located at
 * the specified @x and @y coordinates.
 **/
gint
atk_text_get_offset_at_point (AtkText *text,
                              gint x,
                              gint y)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, -1);
  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_offset_at_point)
    return (*(iface->get_offset_at_point)) (text, x, y);
  else
    return -1;
}

/**
 * atk_text_get_n_selections
 * @text: an #AtkText
 *
 * Gets the number of selected regions.
 *
 * Returns: The number of selected regions, or -1 if a failure
 *   occurred.
 **/
gint
atk_text_get_n_selections (AtkText *text)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, -1);
  g_return_val_if_fail (ATK_IS_TEXT (text), -1);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_n_selections)
    return (*(iface->get_n_selections)) (text);
  else
    return -1;
}

/**
 * atk_text_get_selection
 * @text: an #AtkText
 * @selection_num: The selection number.  The selected regions are
 * assigned numbers that corrispond to how far the region is from the
 * start of the text.  The selected region closest to the beginning
 * of the text region is assigned the number 0, etc.  Note that adding,
 * moving or deleting a selected region can change the numbering.
 * @start_offset: passes back the start position of the selected region
 * @end_offset: passes back the end position of the selected region
 *
 * Gets the text from the specified selection.
 *
 * Returns: the selected text.
 **/
gchar*
atk_text_get_selection (AtkText *text, gint selection_num,
   gint *start_offset, gint *end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, NULL);
  g_return_val_if_fail (ATK_IS_TEXT (text), NULL);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->get_selection)
  {
    return (*(iface->get_selection)) (text, selection_num,
       start_offset, end_offset);
  }
  else
    return NULL;
}

/**
 * atk_text_add_selection
 * @text: an #AtkText
 * @start_offset: the start position of the selected region
 * @end_offset: the end position of the selected region
 *
 * Adds a selection bounded by the specified offsets.
 *
 * Returns: %TRUE if success, %FALSE otherwise
 **/
gboolean
atk_text_add_selection (AtkText *text, gint start_offset,
   gint end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->add_selection)
    return (*(iface->add_selection)) (text, start_offset, end_offset);
  else
    return FALSE;
}

/**
 * atk_text_remove_selection
 * @text: an #AtkText
 * @selection_num: The selection number.  The selected regions are
 * assigned numbers that corrispond to how far the region is from the
 * start of the text.  The selected region closest to the beginning
 * of the text region is assigned the number 0, etc.  Note that adding,
 * moving or deleting a selected region can change the numbering.
 *
 * Removes the specified selection
 *
 * Returns: %TRUE if success, %FALSE otherwise
 **/
gboolean
atk_text_remove_selection (AtkText *text, gint selection_num)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->remove_selection)
    return (*(iface->remove_selection)) (text, selection_num);
  else
    return FALSE;
}

/**
 * atk_text_set_selection
 * @text: an #AtkText
 * @selection_num: The selection number.  The selected regions are
 * assigned numbers that corrispond to how far the region is from the
 * start of the text.  The selected region closest to the beginning
 * of the text region is assigned the number 0, etc.  Note that adding,
 * moving or deleting a selected region can change the numbering.
 * @start_offset: the new start position of the selection
 * @end_offset: the new end position of the selection
 *
 * Changes the start and end offset of the specified selection
 *
 * Returns: %TRUE if success, %FALSE otherwise
 **/
gboolean
atk_text_set_selection (AtkText *text, gint selection_num,
   gint start_offset, gint end_offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->set_selection)
  {
    return (*(iface->set_selection)) (text, selection_num,
       start_offset, end_offset);
  }
  else
    return FALSE;
}

/**
 * atk_text_set_caret_offset
 * @text: an #AtkText
 * @offset: position
 *
 * Sets the caret (cursor) position to the specified @offset.
 *
 * Returns: %TRUE if success, %FALSE otherwise.
 **/
gboolean
atk_text_set_caret_offset (AtkText *text,
                           gint    offset)
{
  AtkTextIface *iface;

  g_return_val_if_fail (text != NULL, FALSE);
  g_return_val_if_fail (ATK_IS_TEXT (text), FALSE);

  iface = ATK_TEXT_GET_IFACE (text);

  if (iface->set_caret_offset)
    {
      return (*(iface->set_caret_offset)) (text, offset);
    }
  else
    {
      return FALSE;
    }
}
