#include "config.h"
#include "map-view.h"

#include <glib/gi18n-lib.h>

enum {
  PROP_0,
  PROP_ORIGIN,
};

enum {
  STAR_ACTIVATED,
  LAST_SIGNAL
};

struct _AwMapViewPrivate {
  AwPoint *origin;
};

static unsigned signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (AwMapView, aw_map_view, AW_TYPE_TREE_VIEW);

static void
aw_map_view_row_tapped_cb (GtkTreeView *view,
                           GtkTreePath *path)
{
  AwStar       *star = NULL;
  GtkTreeModel *model;
  GtkTreeIter   iter;

  model = gtk_tree_view_get_model (view);

  if (gtk_tree_model_get_iter (model, &iter, path))
    gtk_tree_model_get (model, &iter, 0, &star, -1);

  g_signal_emit (view, signals[STAR_ACTIVATED], 0, star);

  aw_star_unref (star);
}

static void
aw_map_view_init (AwMapView *view)
{
  const AwPoint *origin = NULL;
  AwProfile     *profile;

  profile = aw_profile_get_self ();
  origin  = aw_profile_get_origin (profile);

  view->priv = G_TYPE_INSTANCE_GET_PRIVATE (view, AW_TYPE_MAP_VIEW,
                                            AwMapViewPrivate);
  view->priv->origin = aw_point_copy (origin);

  g_object_unref (profile);

  g_signal_connect (view, "hildon-row-tapped",
                    G_CALLBACK (aw_map_view_row_tapped_cb), NULL);
}

static void
aw_map_view_set_property (GObject      *object,
                          unsigned      prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  AwMapView *view = AW_MAP_VIEW (object);

  switch (prop_id) {
  case PROP_ORIGIN:
    aw_point_free (view->priv->origin);
    view->priv->origin = g_value_dup_boxed (value);
    gtk_widget_queue_draw (GTK_WIDGET (view));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
aw_map_view_get_property (GObject    *object,
                          unsigned    prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  AwMapView *view = AW_MAP_VIEW (object);

  switch (prop_id) {
  case PROP_ORIGIN:
    g_value_set_boxed (value, view->priv->origin);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static GType *
aw_map_view_get_core_model (AwTreeView *view)
{
  GType types[2] = { AW_TYPE_STAR, G_TYPE_INVALID };
  return g_memdup (types, sizeof types);
}

static const char *
aw_map_view_get_empty_text (AwTreeView *view)
{
  return _("(No Stars)");
}

static int
aw_map_view_compare (GtkTreeModel      *model,
                     GtkTreeIter       *iter_a,
                     GtkTreeIter       *iter_b,
                     AwStarCompareFunc  compare_func)
{
  AwStar *star_a = NULL;
  AwStar *star_b = NULL;
  int     result;

  gtk_tree_model_get (model, iter_a, 0, &star_a, -1);
  gtk_tree_model_get (model, iter_b, 0, &star_b, -1);

  result = compare_func (star_a, star_b);

  aw_star_unref (star_b);
  aw_star_unref (star_a);

  return result;
}

static int
aw_map_view_compare_by_name (GtkTreeModel *model,
                             GtkTreeIter  *iter_a,
                             GtkTreeIter  *iter_b,
                             gpointer      user_data)
{
  return aw_map_view_compare (model, iter_a, iter_b, aw_star_compare_by_name);
}

static void
aw_map_view_sort_by_name_cb (AwTreeView      *view,
                             GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_map_view_compare_by_name);
}

static int
aw_map_view_compare_by_id (GtkTreeModel *model,
                           GtkTreeIter  *iter_a,
                           GtkTreeIter  *iter_b,
                           gpointer      user_data)
{
  return aw_map_view_compare (model, iter_a, iter_b, aw_star_compare_by_id);
}

static void
aw_map_view_sort_by_id_cb (AwTreeView      *view,
                           GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_map_view_compare_by_id);
}

static int
aw_map_view_compare_by_level (GtkTreeModel *model,
                              GtkTreeIter  *iter_a,
                              GtkTreeIter  *iter_b,
                              gpointer      user_data)
{
  return aw_map_view_compare (model, iter_a, iter_b, aw_star_compare_by_level);
}

static void
aw_map_view_sort_by_level_cb (AwTreeView      *view,
                              GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_map_view_compare_by_level);
}

static int
aw_map_view_compare_by_distance (GtkTreeModel *model,
                                 GtkTreeIter  *iter_a,
                                 GtkTreeIter  *iter_b,
                                 gpointer      user_data)
{
  AwMapView     *view = AW_MAP_VIEW (user_data);
  const AwPoint *point_a = NULL, *point_b = NULL;
  double         distance_a, distance_b;
  AwStar        *star_a, *star_b;

  gtk_tree_model_get (model, iter_a, 0, &star_a, -1);
  gtk_tree_model_get (model, iter_b, 0, &star_b, -1);

  if (star_a)
    point_a = aw_star_get_location (star_a);
  if (star_b)
    point_b = aw_star_get_location (star_b);

  distance_a = aw_point_get_distance (point_a, view->priv->origin);
  distance_b = aw_point_get_distance (point_b, view->priv->origin);

  aw_star_unref (star_a);
  aw_star_unref (star_b);

  if (distance_a != distance_b)
    return (distance_a < distance_b ? -1 : +1);

  return aw_map_view_compare_by_name (model, iter_a, iter_b, user_data);
}

static void
aw_map_view_sort_by_distance_cb (AwTreeView      *view,
                                 GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_map_view_compare_by_distance);
}

static GSList *
aw_map_view_get_filters (AwTreeView *view)
{
  struct {
    const char *label;
    void      (*callback) ();
  } filters[] = {
      { _("Distance"), aw_map_view_sort_by_distance_cb },
      { _("Name"),     aw_map_view_sort_by_name_cb     },
      { _("ID"),       aw_map_view_sort_by_id_cb       },
      { _("Level"),    aw_map_view_sort_by_level_cb    },
  };

  gpointer button = NULL;
  int      i;

  for (i = G_N_ELEMENTS (filters) - 1; i >= 0; --i)
    {
      button = gtk_radio_button_new_from_widget (button);
      gtk_button_set_label (button, filters[i].label);
      gtk_toggle_button_set_mode (button, FALSE);

      g_signal_connect_swapped (button, "toggled",
                                filters[i].callback, view);

      gtk_widget_show (button);
    }

  return gtk_radio_button_get_group (button);
}

static void
aw_map_view_title_cell_cb (GtkTreeViewColumn *column,
                           GtkCellRenderer   *cell,
                           GtkTreeModel      *model,
                           GtkTreeIter       *iter,
                           gpointer           user_data)
{
  GString        *text       = g_string_new (NULL);
  PangoAttrList  *attributes = NULL;
  AwStar         *star       = NULL;
  double          distance   = 0;
  AwTreeView     *view;

  gtk_tree_model_get (model, iter, 0, &star, -1);

  if (star)
    {
      view = AW_TREE_VIEW (gtk_tree_view_column_get_tree_view (column));

      distance = aw_point_get_distance (AW_MAP_VIEW (view)->priv->origin,
                                        aw_star_get_location (star));

      g_string_append (text, aw_star_get_name (star));
      g_string_append (text, "\n");

      attributes = aw_tree_view_insert_font (view, attributes,
                                             AW_TREE_VIEW_FONT_SECONDARY,
                                             text->len, G_MAXINT);
      attributes = aw_tree_view_insert_color (view, attributes,
                                              AW_TREE_VIEW_COLOR_SECONDARY,
                                              text->len, G_MAXINT);


      g_string_append        (text, _("Location"));
      g_string_append_printf (text, ": %d/%d, ",
                              aw_star_get_x (star),
                              aw_star_get_y (star));
      g_string_append        (text, _("Distance"));
      g_string_append_printf (text, ": %.1f pc", distance);
    }

  g_object_set (cell, "text", text->str, "attributes", attributes, NULL);
  pango_attr_list_unref (attributes);
  g_string_free (text, TRUE);
  aw_star_unref (star);
}

static void
aw_map_view_detail_cell_cb (GtkTreeViewColumn *column,
                            GtkCellRenderer   *cell,
                            GtkTreeModel      *model,
                            GtkTreeIter       *iter,
                            gpointer           user_data)
{
  char       *text  = NULL;
  AwStar     *star  = NULL;
  AwTreeView *view;

  gtk_tree_model_get (model, iter, 0, &star, -1);

  if (star)
    {
      view = AW_TREE_VIEW (gtk_tree_view_column_get_tree_view (column));

      text = g_strdup_printf ("%s: %d\n%s: %d",
                              _("ID"), aw_star_get_id (star),
                              _("Level"), aw_star_get_level (star));
    }

  g_object_set (cell, "text", text, NULL);

  aw_star_unref (star);
  g_free (text);
}

static void
aw_map_view_create_columns (AwTreeView *view)
{
  GtkCellRenderer *cell;

  /* === title column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_primary_column (view, cell),
     cell, aw_map_view_title_cell_cb, NULL, NULL);

  /* === detail column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell),
     cell, aw_map_view_detail_cell_cb, NULL, NULL);
}

static void
aw_map_view_finalize (GObject *object)
{
  AwMapView *view = AW_MAP_VIEW (object);

  aw_point_free (view->priv->origin);

  G_OBJECT_CLASS (aw_map_view_parent_class)->finalize (object);
}

static void
aw_map_view_class_init (AwMapViewClass *class)
{
  GObjectClass    *object_class;
  AwTreeViewClass *tree_view_class;

  object_class               = G_OBJECT_CLASS (class);
  object_class->set_property = aw_map_view_set_property;
  object_class->get_property = aw_map_view_get_property;
  object_class->finalize     = aw_map_view_finalize;

  tree_view_class                 = AW_TREE_VIEW_CLASS (class);
  tree_view_class->get_core_model = aw_map_view_get_core_model;
  tree_view_class->get_empty_text = aw_map_view_get_empty_text;
  tree_view_class->get_filters    = aw_map_view_get_filters;
  tree_view_class->create_columns = aw_map_view_create_columns;

  g_object_class_install_property
    (object_class, PROP_ORIGIN,
     g_param_spec_boxed ("origin",
                         "Origin",
                         "Origin of the map view",
                         AW_TYPE_POINT,
                         G_PARAM_READWRITE |
                         G_PARAM_STATIC_STRINGS));

  signals[STAR_ACTIVATED] = g_signal_new ("star-activated",
                                          AW_TYPE_MAP_VIEW,
                                          G_SIGNAL_RUN_LAST, 0, NULL, NULL,
                                          g_cclosure_marshal_VOID__BOXED,
                                          G_TYPE_NONE, 1, AW_TYPE_STAR);

  g_type_class_add_private (class, sizeof (AwMapViewPrivate));
}

GtkWidget *
aw_map_view_new (void)
{
  return gtk_widget_new (AW_TYPE_MAP_VIEW, NULL);
}

void
aw_map_view_set_origin (AwMapView *view,
                        AwPoint   *point)
{
  g_return_if_fail (AW_IS_MAP_VIEW (view));
  g_object_set (view, "origin", point, NULL);
}

G_CONST_RETURN AwPoint *
aw_map_view_get_origin (AwMapView *view)
{
  g_return_val_if_fail (AW_IS_MAP_VIEW (view), NULL);
  return view->priv->origin;
}

