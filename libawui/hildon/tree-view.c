#include "config.h"
#include "tree-view.h"

#include <glib/gi18n-lib.h>

enum {
  PROP_0,
  PROP_EMPTY_TEXT,
};

struct _AwTreeViewPrivate {
  char                 *empty_text;

  GdkColor              attention_color;
  GdkColor              secondary_color;

  PangoFontDescription *small_system_font;
  PangoFontDescription *large_system_font;

  GList                *secondary_cells;
};

static GObjectClass   *object_parent_class;
static GtkWidgetClass *widget_parent_class;

G_DEFINE_TYPE (AwTreeView, aw_tree_view, GTK_TYPE_TREE_VIEW);

static void
aw_tree_view_init (AwTreeView *view)
{
  view->priv = G_TYPE_INSTANCE_GET_PRIVATE (view, AW_TYPE_TREE_VIEW,
                                            AwTreeViewPrivate);
}

static void
aw_tree_view_constructed (GObject *object)
{
  AwTreeViewClass *class = AW_TREE_VIEW_GET_CLASS (object);

  if (object_parent_class->constructed)
    object_parent_class->constructed (object);
  if (class->create_columns)
    class->create_columns (AW_TREE_VIEW (object));
}

static void
aw_tree_view_finalize (GObject *object)
{
  AwTreeView *view = AW_TREE_VIEW (object);

  g_list_free (view->priv->secondary_cells);
  g_free      (view->priv->empty_text);

  object_parent_class->finalize (object);
}

static void
aw_tree_view_check_model (AwTreeView *view)
{
  GType           *core_model = NULL;
  GType            column_type;
  AwTreeViewClass *class;
  GtkTreeModel    *model;
  int              i;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));

  if (!model)
    return;

  class = AW_TREE_VIEW_GET_CLASS (view);

  if (class->get_core_model)
    core_model = class->get_core_model (view);

  if (!core_model)
    return;

  for (i = 0; G_TYPE_INVALID != core_model[i]; ++i)
    {
      column_type = gtk_tree_model_get_column_type (model, i);

      if (g_type_is_a (column_type, core_model[i]))
        continue;

      g_warning ("%s: Invalid column type %s for column %d of %s. "
                 "Expected column type for %s is %s.", G_STRFUNC,
                 g_type_name (column_type), i, G_OBJECT_TYPE_NAME (model),
                 G_OBJECT_TYPE_NAME (view), g_type_name (core_model[i]));

      /* reset the model to avoid crashes */
      gtk_tree_view_set_model (GTK_TREE_VIEW (view), NULL);
      break;
    }

  g_free (core_model);
}

static void
aw_tree_view_notify (GObject    *object,
                     GParamSpec *pspec)
{
  /* FIXME: Proper method would be to override the "model" property and check
   * the model in set_property(). Unfortunatly GtkTreeModel violates GObject's
   * property design pattern and calls the gtk_tree_view_set_model() from
   * its set_property() method, instead of doing it the other way arround.
   */
  if (!strcmp (pspec->name, "model"))
    aw_tree_view_check_model (AW_TREE_VIEW (object));

  if (object_parent_class->notify)
    object_parent_class->notify (object, pspec);
}

static void
aw_tree_view_set_property (GObject      *object,
                           unsigned      prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  AwTreeView *view = AW_TREE_VIEW (object);

  switch (prop_id) {
  case PROP_EMPTY_TEXT:
    g_free (view->priv->empty_text);
    view->priv->empty_text = g_value_dup_string (value);
    gtk_widget_queue_draw (GTK_WIDGET (view));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
aw_tree_view_get_property (GObject    *object,
                           unsigned    prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  AwTreeView *view = AW_TREE_VIEW (object);

  switch (prop_id) {
  case PROP_EMPTY_TEXT:
    g_value_set_string (value, view->priv->empty_text);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
aw_tree_view_draw_empty_text (GtkWidget      *widget,
                              GdkEventExpose *event)
{
  AwTreeView      *view;
  AwTreeViewClass *class;
  int              width, height;
  const char      *empty_text;
  PangoLayout     *layout;
  cairo_t         *cr;

  view       = AW_TREE_VIEW (widget);
  class      = AW_TREE_VIEW_GET_CLASS (view);

  empty_text = view->priv->empty_text;

  if (!empty_text && class->get_empty_text)
    empty_text = class->get_empty_text (view);
  if (!empty_text)
    empty_text = _("(Empty)");

  if (empty_text[0])
    {
      cr = gdk_cairo_create (event->window);
      gdk_cairo_region (cr, event->region);
      cairo_clip (cr);

      layout = gtk_widget_create_pango_layout (widget, empty_text);

      if (view->priv->large_system_font)
        pango_layout_set_font_description (layout, view->priv->large_system_font);

      pango_layout_get_pixel_size (layout, &width, &height);

      cairo_move_to (cr, (widget->allocation.width - width) / 2,
                     (widget->allocation.height - height) / 2);

      gdk_cairo_set_source_color (cr, &view->priv->secondary_color);
      pango_cairo_show_layout (cr, layout);

      g_object_unref (layout);
      cairo_destroy (cr);
    }
}

static gboolean
aw_tree_view_expose_event (GtkWidget      *widget,
                           GdkEventExpose *event)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;

  widget_parent_class->expose_event (widget, event);

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (widget));

  if (!model || !gtk_tree_model_get_iter_first (model, &iter))
    aw_tree_view_draw_empty_text (widget, event);

  return FALSE;
}

static void
aw_tree_view_update_secondary_cell_style (AwTreeView *view)
{
  GList *l, *n;

  for (l = view->priv->secondary_cells; l && (n = l->next, l); l = n)
    {
      if (l->data)
        {
          g_object_set (l->data,
                        "foreground-gdk", &view->priv->secondary_color,
                        "font-desc", view->priv->small_system_font, NULL);
        }
      else
        view->priv->secondary_cells = g_list_delete_link (view->priv->secondary_cells, l);
    }
}

static void
aw_tree_view_style_set (GtkWidget *widget,
                        GtkStyle  *old_style)
{
  AwTreeView *view = AW_TREE_VIEW (widget);
  GtkStyle   *style;

  if (widget_parent_class->style_set)
    widget_parent_class->style_set (widget, old_style);

  /* lookup named colors */

  if (!gtk_style_lookup_color (widget->style, "SecondaryTextColor",
                               &view->priv->secondary_color))
    view->priv->secondary_color = widget->style->text[GTK_STATE_INSENSITIVE];

  if (!gtk_style_lookup_color (widget->style, "AttentionColor",
                               &view->priv->attention_color))
    gdk_color_parse ("#f00", &view->priv->attention_color);

  style = gtk_rc_get_style_by_paths (gtk_widget_get_settings (widget),
                                     "LargeSystemFont", NULL,
                                     G_OBJECT_TYPE (view));

  /* lookup named fonts */

  view->priv->large_system_font = (style ? style->font_desc : NULL);

  style = gtk_rc_get_style_by_paths (gtk_widget_get_settings (widget),
                                     "SmallSystemFont", NULL,
                                     G_OBJECT_TYPE (view));

  view->priv->small_system_font = (style ? style->font_desc : NULL);

  /* apply style */

  aw_tree_view_update_secondary_cell_style (view);
}

static void
aw_tree_view_class_init (AwTreeViewClass *class)
{
  GObjectClass   *object_class;
  GtkWidgetClass *widget_class;

  object_class               = G_OBJECT_CLASS (class);
  object_class->constructed  = aw_tree_view_constructed;
  object_class->finalize     = aw_tree_view_finalize;
  object_class->set_property = aw_tree_view_set_property;
  object_class->get_property = aw_tree_view_get_property;
  object_class->notify       = aw_tree_view_notify;

  widget_class               = GTK_WIDGET_CLASS (class);
  widget_class->expose_event = aw_tree_view_expose_event;
  widget_class->style_set    = aw_tree_view_style_set;

  g_object_class_install_property
    (object_class, PROP_EMPTY_TEXT,
     g_param_spec_string ("empty-text",
                          "Empty Text",
                          "Text to show when the model is empty",
                          NULL,
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  g_type_class_add_private (class, sizeof (AwTreeViewPrivate));

  object_parent_class = G_OBJECT_CLASS   (aw_tree_view_parent_class);
  widget_parent_class = GTK_WIDGET_CLASS (aw_tree_view_parent_class);
}

GtkWidget *
aw_tree_view_new (HildonUIMode ui_mode)
{
  return gtk_widget_new (AW_TYPE_TREE_VIEW,
                         "hildon-ui-mode", ui_mode, NULL);
}

GtkWidget *
aw_tree_view_new_with_model (HildonUIMode  ui_mode,
                             GtkTreeModel *model)
{
  return gtk_widget_new (AW_TYPE_TREE_VIEW,
                         "hildon-ui-mode", ui_mode,
                         "model", model, NULL);
}

void
aw_tree_view_set_empty_text (AwTreeView   *view,
                             const char   *text)
{
  g_return_if_fail (AW_IS_TREE_VIEW (view));
  g_object_set (view, "empty-text", text, NULL);
}

G_CONST_RETURN char *
aw_tree_view_get_empty_text (AwTreeView *view)
{
  g_return_val_if_fail (AW_IS_TREE_VIEW (view), NULL);
  return view->priv->empty_text;
}

GSList *
aw_tree_view_get_filters (AwTreeView *view)
{
  AwTreeViewClass *class;

  g_return_val_if_fail (AW_IS_TREE_VIEW (view), NULL);

  class = AW_TREE_VIEW_GET_CLASS (view);

  if (class->get_filters)
    return class->get_filters (view);

  return NULL;
}

GSList *
aw_tree_view_get_menu_items (AwTreeView *view)
{
  AwTreeViewClass *class;

  g_return_val_if_fail (AW_IS_TREE_VIEW (view), NULL);

  class = AW_TREE_VIEW_GET_CLASS (view);

  if (class->get_menu_items)
    return class->get_menu_items (view);

  return NULL;
}

void
aw_tree_view_set_sort_func (AwTreeView             *view,
                            GtkTreeIterCompareFunc  sort_func)
{
  GtkTreeModel *model;

  g_return_if_fail (AW_IS_TREE_VIEW (view));

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));

  gtk_tree_sortable_set_default_sort_func (GTK_TREE_SORTABLE (model),
                                           sort_func, view, NULL);

  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (model),
                                        GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID,
                                        GTK_SORT_ASCENDING);
}

static GtkTreeModel *
aw_tree_view_create_core_model (AwTreeView *view)
{
  AwTreeViewClass *class;
  GtkTreeModel    *model = NULL;
  GType           *types = NULL;
  int              n_types = 0;

  class = AW_TREE_VIEW_GET_CLASS (view);

  if (class->get_core_model)
    types = class->get_core_model (view);

  if (types)
    {
      for (n_types = 0; G_TYPE_INVALID != types[n_types]; ++n_types);
      model = GTK_TREE_MODEL (gtk_list_store_newv (n_types, types));
      g_free (types);
    }

  return model;
}

void
aw_tree_view_append_items (AwTreeView *view,
                           GList      *items)
{
  gpointer    model;

  g_return_if_fail (AW_IS_TREE_VIEW (view));

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));

  if (!model)
    {
      model = aw_tree_view_create_core_model (view);
      gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);
    }

  g_return_if_fail (GTK_IS_LIST_STORE (model));

  while (items)
    {
      gtk_list_store_insert_with_values (model, NULL, -1, 0, items->data, -1);
      items = items->next;
    }
}

GtkTreeViewColumn *
aw_tree_view_append_primary_column (AwTreeView      *view,
                                    GtkCellRenderer *cell)
{
  GtkTreeViewColumn *column;

  g_return_val_if_fail (AW_IS_TREE_VIEW (view), NULL);
  g_return_val_if_fail (GTK_IS_CELL_RENDERER_TEXT (cell), NULL);

  g_object_set (cell,
                "xalign", 0.0,
                "xpad",   HILDON_MARGIN_DEFAULT, NULL);

  column = gtk_tree_view_column_new ();
  gtk_tree_view_column_set_expand (column, TRUE);
  gtk_tree_view_column_pack_start (column, cell, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);

  return column;
}

GtkTreeViewColumn *
aw_tree_view_append_secondary_column (AwTreeView      *view,
                                      GtkCellRenderer *cell)
{
  GtkTreeViewColumn *column;

  g_return_val_if_fail (AW_IS_TREE_VIEW (view), NULL);
  g_return_val_if_fail (GTK_IS_CELL_RENDERER_TEXT (cell), NULL);

  g_object_set (cell, "foreground-gdk", &view->priv->secondary_color,
                      "font-desc",      view->priv->small_system_font,
                      "xpad",           HILDON_MARGIN_DEFAULT,
                      "alignment",      PANGO_ALIGN_RIGHT,
                      "xalign",         1.0, NULL);

  column = gtk_tree_view_column_new ();
  gtk_tree_view_column_set_expand (column, FALSE);
  gtk_tree_view_column_pack_start (column, cell, TRUE);
  gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);

  view->priv->secondary_cells = g_list_prepend (view->priv->secondary_cells, cell);
  g_object_add_weak_pointer (G_OBJECT (cell), &view->priv->secondary_cells->data);

  return column;
}

PangoAttrList *
aw_tree_view_insert_color (AwTreeView          *view,
                           PangoAttrList       *list,
                           AwTreeViewColorName  name,
                           int                  start,
                           int                  end)
{
  PangoAttribute *attr  = NULL;
  GdkColor       *color = NULL;

  g_return_val_if_fail (AW_IS_TREE_VIEW (view),             list);
  g_return_val_if_fail (name < AW_TREE_VIEW_COLOR_LAST,     list);
  g_return_val_if_fail (name >= 0,                          list);
  g_return_val_if_fail (start < end,                        list);

  if (!list)
    list = pango_attr_list_new ();

  switch (name)
    {
    case AW_TREE_VIEW_COLOR_PRIMARY:
      color = &GTK_WIDGET (view)->style->text[GTK_STATE_NORMAL];
      break;
    case AW_TREE_VIEW_COLOR_SECONDARY:
      color = &view->priv->secondary_color;
      break;
    case AW_TREE_VIEW_COLOR_ATTENTION:
      color = &view->priv->attention_color;
      break;
    case AW_TREE_VIEW_COLOR_LAST:
      color = NULL;
      break;
    }

  if (color)
    {
      attr = pango_attr_foreground_new (color->red, color->green, color->blue);
      attr->start_index = start;
      attr->end_index   = end;

      pango_attr_list_insert (list, attr);
    }

  return list;
}

PangoAttrList *
aw_tree_view_insert_font (AwTreeView         *view,
                          PangoAttrList      *list,
                          AwTreeViewFontName  name,
                          int                 start,
                          int                 end)
{
  PangoAttribute *attr = NULL;

  g_return_val_if_fail (AW_IS_TREE_VIEW (view),             list);
  g_return_val_if_fail (name < AW_TREE_VIEW_FONT_LAST,      list);
  g_return_val_if_fail (name >= 0,                          list);
  g_return_val_if_fail (start < end,                        list);

  if (!list)
    list = pango_attr_list_new ();

  switch (name)
    {
    case AW_TREE_VIEW_FONT_PRIMARY:
      attr = pango_attr_font_desc_new (GTK_WIDGET (view)->style->font_desc);
      break;
    case AW_TREE_VIEW_FONT_SECONDARY:
      if (view->priv->small_system_font)
        attr = pango_attr_font_desc_new (view->priv->small_system_font);
      else
        attr = pango_attr_scale_new (PANGO_SCALE_SMALL);

      break;
    case AW_TREE_VIEW_FONT_ITALIC:
      attr = pango_attr_style_new (PANGO_STYLE_ITALIC);
      break;
    case AW_TREE_VIEW_FONT_BOLD:
      attr = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
      break;
    case AW_TREE_VIEW_FONT_LAST:
      break;
    }

  if (attr)
    {
      attr->start_index = start;
      attr->end_index   = end;

      pango_attr_list_insert (list, attr);
    }

  return list;
}

int
aw_tree_view_accumulate (AwTreeView  *view,
                         int        (*callback)(),
                         gpointer     user_data)
{
  int           result = 0;
  GtkTreeModel *model;
  GtkTreeIter   iter;
  gpointer      item;
  GType         type;

  g_return_val_if_fail (AW_IS_TREE_VIEW (view), -1);
  g_return_val_if_fail (NULL != callback,       -1);

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));
  type  = gtk_tree_model_get_column_type (model, 0);

  g_return_val_if_fail (g_type_is_a (type, G_TYPE_OBJECT) ||
                        g_type_is_a (type, G_TYPE_BOXED), -1);

  if (model && gtk_tree_model_get_iter_first (model, &iter))
    do
      {
        gtk_tree_model_get (model, &iter, 0, &item, -1);

        if (item)
          {
            result += callback (item, user_data);

            if (g_type_is_a (type, G_TYPE_OBJECT))
              g_object_unref (item);
            else
              g_boxed_free (type, item);
          }
      }
    while (gtk_tree_model_iter_next (model, &iter));

  return result;
}


