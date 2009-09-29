#include "config.h"
#include "planet-view.h"

#include <libawui/model.h>
#include <glib/gi18n-lib.h>

typedef int (* AwPlanetDetailFunc) (const AwPlanet *planet);

struct _AwPlanetViewPrivate {
  GtkTreeRowReference *summary_row;
  gulong               summary_changed_id;
};

enum {
  PROP_0,
  PROP_SUMMARY_VISIBLE,
};

enum {
  PLANET_ACTIVATED,
  LAST_SIGNAL
};

static unsigned        signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (AwPlanetView, aw_planet_view, AW_TYPE_TREE_VIEW);

static void
aw_planet_view_create_summary (AwPlanetView *view);

static void
aw_planet_view_row_tapped_cb (GtkTreeView *view,
                              GtkTreePath *path)
{
  AwPlanet     *planet = NULL;
  GtkTreeModel *model;
  GtkTreeIter   iter;

  model = gtk_tree_view_get_model (view);

  if (gtk_tree_model_get_iter (model, &iter, path))
    gtk_tree_model_get (model, &iter, 0, &planet, -1);

  if (planet)
    g_signal_emit (view, signals[PLANET_ACTIVATED], 0, planet);

  aw_planet_unref (planet);
}

static void
aw_planet_view_init (AwPlanetView *view)
{
  view->priv = G_TYPE_INSTANCE_GET_PRIVATE (view, AW_TYPE_PLANET_VIEW,
                                            AwPlanetViewPrivate);

  g_signal_connect (view, "hildon-row-tapped",
                    G_CALLBACK (aw_planet_view_row_tapped_cb), NULL);
}

static GType *
aw_planet_view_get_core_model (AwTreeView *view)
{
  GType types[2] = { AW_TYPE_PLANET, G_TYPE_INVALID };
  return g_memdup (types, sizeof types);
}

static const char *
aw_planet_view_get_empty_text (AwTreeView *view)
{
  return _("(No Planets)");
}

static int
aw_planet_view_compare (GtkTreeModel        *model,
                        GtkTreeIter         *iter_a,
                        GtkTreeIter         *iter_b,
                        AwPlanetCompareFunc  compare_func)
{
  AwPlanet *planet_a = NULL;
  AwPlanet *planet_b = NULL;
  int       result;

  gtk_tree_model_get (model, iter_a, 0, &planet_a, -1);
  gtk_tree_model_get (model, iter_b, 0, &planet_b, -1);

  result = compare_func (planet_a, planet_b);

  aw_planet_unref (planet_b);
  aw_planet_unref (planet_a);

  return result;
}

static int
aw_planet_view_compare_by_name (GtkTreeModel *model,
                                GtkTreeIter  *iter_a,
                                GtkTreeIter  *iter_b,
                                gpointer      user_data)
{
  return aw_planet_view_compare (model, iter_a, iter_b,
                                 aw_planet_compare_by_name);
}

static void
aw_planet_view_sort_by_name_cb (AwTreeView      *view,
                                GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_planet_view_compare_by_name);
}

static int
aw_planet_view_compare_by_population (GtkTreeModel *model,
                                      GtkTreeIter  *iter_a,
                                      GtkTreeIter  *iter_b,
                                      gpointer      user_data)
{
  return aw_planet_view_compare (model, iter_a, iter_b,
                                 aw_planet_compare_by_population);
}

static void
aw_planet_view_sort_by_population_cb (AwTreeView      *view,
                                      GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_planet_view_compare_by_population);
}

static int
aw_planet_view_compare_by_starbase (GtkTreeModel *model,
                                    GtkTreeIter  *iter_a,
                                    GtkTreeIter  *iter_b,
                                    gpointer      user_data)
{
  return aw_planet_view_compare (model, iter_a, iter_b,
                                 aw_planet_compare_by_starbase);
}

static void
aw_planet_view_sort_by_starbase_cb (AwTreeView      *view,
                                    GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_planet_view_compare_by_starbase);
}

static int
aw_planet_view_compare_by_owner (GtkTreeModel *model,
                                 GtkTreeIter  *iter_a,
                                 GtkTreeIter  *iter_b,
                                 gpointer      user_data)
{
  return aw_planet_view_compare (model, iter_a, iter_b,
                                 aw_planet_compare_by_owner);
}

static void
aw_planet_view_sort_by_owner_cb (AwTreeView      *view,
                                 GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_planet_view_compare_by_owner);
}

static GSList *
aw_planet_view_get_filters (AwTreeView *view)
{
  struct {
    const char *label;
    void      (*callback) ();
  } filters[] = {
      { _("Name"),       aw_planet_view_sort_by_name_cb       },
      { _("Population"), aw_planet_view_sort_by_population_cb },
      { _("Starbase"),   aw_planet_view_sort_by_starbase_cb   },
      { _("Owner"),      aw_planet_view_sort_by_owner_cb      },
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
aw_planet_view_title_cell_cb (GtkTreeViewColumn *column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *model,
                              GtkTreeIter       *iter,
                              gpointer           user_data)
{
  const char    *text       = NULL;
  PangoAttrList *attributes = NULL;
  AwPlanet      *planet = NULL;
  AwTreeView    *view;

  gtk_tree_model_get (model, iter, 0, &planet, -1);

  if (planet)
    {
      view = AW_TREE_VIEW (gtk_tree_view_column_get_tree_view (column));

      if (aw_planet_get_flags (planet) & AW_PLANET_SIEGED)
        {
          attributes = aw_tree_view_insert_color (view, attributes,
                                                  AW_TREE_VIEW_COLOR_ATTENTION,
                                                  0, G_MAXINT);
        }

      text = aw_planet_get_name (planet);
    }

  g_object_set (cell, "text", text, "attributes", attributes, NULL);
  pango_attr_list_unref (attributes);
  aw_planet_unref (planet);
}

static void
aw_planet_view_summarize (GtkTreeModel       *model,
                          AwPlanetDetailFunc  value1_func,
                          int                *value1,
                          AwPlanetDetailFunc  value2_func,
                          int                *value2)
{
  AwPlanet    *planet;
  GtkTreeIter  iter;

  if (value1)
    *value1 = 0;
  if (value2)
    *value2 = 0;

  if (gtk_tree_model_get_iter_first (model, &iter))
    {
      do
        {
          gtk_tree_model_get (model, &iter, 0, &planet, -1);

          if (!planet)
            continue;

          if (value1 && value1_func)
            *value1 += value1_func (planet);
          if (value2 && value2_func)
            *value2 += value2_func (planet);

          aw_planet_unref (planet);
        }
      while (gtk_tree_model_iter_next (model, &iter));
    }
}

static void
aw_planet_view_update_details (GtkCellRenderer    *cell,
                               GtkTreeModel       *model,
                               GtkTreeIter        *iter,
                               const char         *label1,
                               AwPlanetDetailFunc  value1_func,
                               const char         *label2,
                               AwPlanetDetailFunc  value2_func)
{
  int       value1, value2;
  GString  *text = g_string_new (NULL);
  AwPlanet *planet = NULL;

  gtk_tree_model_get (model, iter, 0, &planet, -1);

  if (planet)
    {
      value1 = value1_func (planet);
      value2 = value2_func (planet);
    }
  else
    aw_planet_view_summarize (model, value1_func, &value1, value2_func, &value2);

  if (value1 >= 0)
    g_string_append_printf (text, "%s: %d", label1, value1);
  else
    g_string_append_printf (text, "%s: -", label1);

  g_string_append_c (text, '\n');

  if (value2 >= 0)
    g_string_append_printf (text, "%s: %d", label2, value2);
  else
    g_string_append_printf (text, "%s: -", label2);

  g_object_set (cell, "text", text->str, NULL);
  g_string_free (text, TRUE);
  aw_planet_unref (planet);
}

static void
aw_planet_view_industries_cell_cb (GtkTreeViewColumn *column,
                                   GtkCellRenderer   *cell,
                                   GtkTreeModel      *model,
                                   GtkTreeIter       *iter,
                                   gpointer           user_data)
{
  aw_planet_view_update_details (cell, model, iter,
                                 _("Farm"), aw_planet_get_farm,
                                 _("Factory"), aw_planet_get_factory);
}

static void
aw_planet_view_science_cell_cb (GtkTreeViewColumn *column,
                                GtkCellRenderer   *cell,
                                GtkTreeModel      *model,
                                GtkTreeIter       *iter,
                                gpointer           user_data)
{
  aw_planet_view_update_details (cell, model, iter,
                                 _("Cybernet"), aw_planet_get_cybernet,
                                 _("Laboratory"), aw_planet_get_laboratory);
}

static void
aw_planet_view_population_cell_cb (GtkTreeViewColumn *column,
                                   GtkCellRenderer   *cell,
                                   GtkTreeModel      *model,
                                   GtkTreeIter       *iter,
                                   gpointer           user_data)
{
  aw_planet_view_update_details (cell, model, iter,
                                 _("Growth"), aw_planet_get_growth_per_hour,
                                 _("Population"), aw_planet_get_population);
}

static void
aw_planet_view_production_cell_cb (GtkTreeViewColumn *column,
                                   GtkCellRenderer   *cell,
                                   GtkTreeModel      *model,
                                   GtkTreeIter       *iter,
                                   gpointer           user_data)
{
  int            points = 0, growth = 0;
  GString       *text = g_string_new (NULL);
  PangoAttrList *attributes = NULL;
  AwPlanet      *planet = NULL;
  AwTreeView    *view;

  gtk_tree_model_get (model, iter, 0, &planet, -1);

  if (planet)
    {
      growth = aw_planet_get_production_per_hour (planet);
      points = aw_planet_get_production_points (planet);
    }
  else
    {
      aw_planet_view_summarize (model,
                                aw_planet_get_production_per_hour, &growth,
                                aw_planet_get_production_points, &points);
    }

  view = AW_TREE_VIEW (gtk_tree_view_column_get_tree_view (column));

  g_string_append_printf (text, "%s: %d\n",
                          _("Production"), growth);

  attributes = aw_tree_view_insert_font (view, attributes,
                                         AW_TREE_VIEW_FONT_BOLD,
                                         text->len, G_MAXINT);
  attributes = aw_tree_view_insert_color (view, attributes,
                                          AW_TREE_VIEW_COLOR_PRIMARY,
                                          text->len, G_MAXINT);

  if (points >= 0)
    g_string_append_printf (text, "%d", points);
  else
    g_string_append_c (text, '-');

  g_object_set (cell, "text", text->str, "attributes", attributes, NULL);
  pango_attr_list_unref (attributes);
  g_string_free (text, TRUE);
  aw_planet_unref (planet);
}

static void
aw_planet_view_create_columns (AwTreeView *view)
{
  GtkCellRenderer *cell;

  /* === title column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_primary_column (view, cell),
     cell, aw_planet_view_title_cell_cb, NULL, NULL);

  /* === industries column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell),
     cell, aw_planet_view_industries_cell_cb, NULL, NULL);

  /* === science column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell),
     cell, aw_planet_view_science_cell_cb, NULL, NULL);

  /* === population column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell),
     cell, aw_planet_view_population_cell_cb, NULL, NULL);

  /* === production column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell),
     cell, aw_planet_view_production_cell_cb, NULL, NULL);
}

static void
aw_planet_view_model_cb (AwPlanetView *view)
{
  g_signal_handlers_disconnect_by_func (view, aw_planet_view_model_cb, NULL);
  aw_planet_view_create_summary (view);
}

static gboolean
aw_planet_view_summary_changed_cb (gpointer user_data)
{
  AwPlanetView *view  = user_data;
  GtkTreeModel *model = NULL;
  GtkTreePath  *path  = NULL;
  GtkTreeIter   iter;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));

  if (model && view->priv->summary_row)
    path = gtk_tree_row_reference_get_path (view->priv->summary_row);

  if (path && gtk_tree_model_get_iter (model, &iter, path))
    {
      gtk_list_store_move_before (GTK_LIST_STORE (model), &iter, NULL);
      gtk_tree_model_row_changed (model, path, &iter);
    }

  view->priv->summary_changed_id = 0;

  return FALSE;
}

static void
aw_planet_view_row_changed_cb (GtkTreeModel *model,
                               GtkTreePath  *path,
                               GtkTreeIter  *iter,
                               AwPlanetView *view)
{
  GtkTreePath *summary_path = NULL;

  if (view->priv->summary_changed_id)
    return;

  if (view->priv->summary_row)
    summary_path = gtk_tree_row_reference_get_path (view->priv->summary_row);

  if (!summary_path || !gtk_tree_path_compare (path, summary_path))
    return;

  view->priv->summary_changed_id =
    g_idle_add (aw_planet_view_summary_changed_cb, view);
}

static void
aw_planet_view_row_deleted_cb (GtkTreeModel *model,
                               GtkTreePath  *path,
                               AwPlanetView *view)
{
  aw_planet_view_row_changed_cb (model, path, NULL, view);
}

static void
aw_planet_view_create_summary (AwPlanetView *view)
{
  GtkTreeModel *model;
  GtkTreePath  *path;
  GtkTreeIter   iter;

  if (view->priv->summary_row)
    return;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));

  if (model)
    {
      gtk_list_store_append (GTK_LIST_STORE (model), &iter);

      path = gtk_tree_model_get_path (model, &iter);
      view->priv->summary_row = gtk_tree_row_reference_new (model, path);
      gtk_tree_path_free (path);

      g_signal_connect (model, "row-inserted",
                        G_CALLBACK (aw_planet_view_row_changed_cb), view);
      g_signal_connect (model, "row-changed",
                        G_CALLBACK (aw_planet_view_row_changed_cb), view);
      g_signal_connect (model, "row-deleted",
                        G_CALLBACK (aw_planet_view_row_deleted_cb), view);
    }
  else
    {
      g_signal_connect (view, "notify::model",
                        G_CALLBACK (aw_planet_view_model_cb), NULL);

    }
}

static void
aw_planet_view_remove_summary (AwPlanetView *view)
{
  if (view->priv->summary_row)
    {
      gtk_tree_row_reference_free (view->priv->summary_row);
      view->priv->summary_row = NULL;
    }

  if (view->priv->summary_changed_id)
    {
      g_source_remove (view->priv->summary_changed_id);
      view->priv->summary_changed_id = 0;
    }
}

static void
aw_planet_view_real_set_summary_visible (AwPlanetView *view,
                                         gboolean      visible)
{
  if (visible)
    aw_planet_view_create_summary (view);
  else
    aw_planet_view_remove_summary (view);
}

static void
aw_planet_view_set_property (GObject      *object,
                             unsigned      prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  switch (prop_id) {
  case PROP_SUMMARY_VISIBLE:
    aw_planet_view_real_set_summary_visible (AW_PLANET_VIEW (object),
                                             g_value_get_boolean (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
aw_planet_view_get_property (GObject    *object,
                             unsigned    prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  AwPlanetView *view = AW_PLANET_VIEW (object);

  switch (prop_id) {
    g_value_set_boolean (value, NULL != view->priv->summary_row);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
aw_planet_view_finalize (GObject *object)
{
  aw_planet_view_remove_summary (AW_PLANET_VIEW (object));
  G_OBJECT_CLASS (aw_planet_view_parent_class)->finalize (object);
}

static void
aw_planet_view_class_init (AwPlanetViewClass *class)
{
  GObjectClass    *object_class;
  AwTreeViewClass *tree_view_class;

  object_class               = G_OBJECT_CLASS (class);
  object_class->set_property = aw_planet_view_set_property;
  object_class->get_property = aw_planet_view_get_property;
  object_class->finalize     = aw_planet_view_finalize;

  tree_view_class                 = AW_TREE_VIEW_CLASS (class);
  tree_view_class->get_core_model = aw_planet_view_get_core_model;
  tree_view_class->get_empty_text = aw_planet_view_get_empty_text;
  tree_view_class->get_filters    = aw_planet_view_get_filters;
  tree_view_class->create_columns = aw_planet_view_create_columns;

  g_object_class_install_property
    (object_class, PROP_SUMMARY_VISIBLE,
     g_param_spec_boolean ("summary-visible",
                           "Summary Visible",
                           "Weither to show a summary",
                           FALSE,
                           G_PARAM_READWRITE |
                           G_PARAM_STATIC_STRINGS));


  signals[PLANET_ACTIVATED] = g_signal_new ("planet-activated",
                                            AW_TYPE_PLANET_VIEW,
                                            G_SIGNAL_RUN_LAST, 0, NULL, NULL,
                                            g_cclosure_marshal_VOID__BOXED,
                                            G_TYPE_NONE, 1, AW_TYPE_PLANET);

  g_type_class_add_private (class, sizeof (AwPlanetViewPrivate));
}

GtkWidget *
aw_planet_view_new (void)
{
  return gtk_widget_new (AW_TYPE_PLANET_VIEW, NULL);
}

void
aw_planet_view_set_summary_visible (AwPlanetView *view,
                                    gboolean      visible)
{
  g_return_if_fail (AW_IS_PLANET_VIEW (view));
  g_object_set (view, "summary-visible", visible, NULL);
}
  
gboolean
aw_planet_view_get_summary_visible (AwPlanetView *view)
{
  g_return_val_if_fail (AW_IS_PLANET_VIEW (view), FALSE);
  return (NULL != view->priv->summary_row);
}
