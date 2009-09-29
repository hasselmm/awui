#include "config.h"
#include "fleet-view.h"

#include <libawui/model.h>
#include <glib/gi18n-lib.h>

#define AW_VESSEL_MASK(vessel1, vessel2) \
  GUINT_TO_POINTER(((vessel1) + 1) + (((vessel2) + 1) << 8))

typedef int (* AwFleetDetailFunc) (const AwFleet *fleet);

enum {
  FLEET_ACTIVATED,
  LAST_SIGNAL
};

static unsigned        signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE(AwFleetView, aw_fleet_view, AW_TYPE_TREE_VIEW);

static void
aw_fleet_view_row_tapped_cb (GtkTreeView *view,
                             GtkTreePath *path)
{
  AwFleet      *fleet = NULL;
  GtkTreeModel *model;
  GtkTreeIter   iter;

  model = gtk_tree_view_get_model (view);

  if (gtk_tree_model_get_iter (model, &iter, path))
    gtk_tree_model_get (model, &iter, 0, &fleet, -1);

  g_signal_emit (view, signals[FLEET_ACTIVATED], 0, fleet);

  aw_fleet_unref (fleet);
}

static void
aw_fleet_view_init (AwFleetView *view)
{
  g_signal_connect (view, "hildon-row-tapped",
                    G_CALLBACK (aw_fleet_view_row_tapped_cb), NULL);
}

static GType *
aw_fleet_view_get_core_model (AwTreeView *view)
{
  GType types[2] = { AW_TYPE_FLEET, G_TYPE_INVALID };
  return g_memdup (types, sizeof types);
}

static const char *
aw_fleet_view_get_empty_text (AwTreeView *view)
{
  return _("(No Fleets)");
}

#if 0
static int
aw_fleet_view_compare (GtkTreeModel        *model,
                        GtkTreeIter         *iter_a,
                        GtkTreeIter         *iter_b,
                        AwFleetCompareFunc  compare_func)
{
  AwFleet *fleet_a = NULL;
  AwFleet *fleet_b = NULL;
  int       result;

  gtk_tree_model_get (model, iter_a, 0, &fleet_a, -1);
  gtk_tree_model_get (model, iter_b, 0, &fleet_b, -1);

  result = compare_func (fleet_a, fleet_b);

  aw_fleet_unref (fleet_b);
  aw_fleet_unref (fleet_a);

  return result;
}

static int
aw_fleet_view_compare_by_name (GtkTreeModel *model,
                                GtkTreeIter  *iter_a,
                                GtkTreeIter  *iter_b,
                                gpointer      user_data)
{
  return aw_fleet_view_compare (model, iter_a, iter_b,
                                 aw_fleet_compare_by_name);
}

static void
aw_fleet_view_sort_by_name_cb (AwTreeView      *view,
                                GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_fleet_view_compare_by_name);
}

static int
aw_fleet_view_compare_by_population (GtkTreeModel *model,
                                      GtkTreeIter  *iter_a,
                                      GtkTreeIter  *iter_b,
                                      gpointer      user_data)
{
  return aw_fleet_view_compare (model, iter_a, iter_b,
                                 aw_fleet_compare_by_population);
}

static void
aw_fleet_view_sort_by_population_cb (AwTreeView      *view,
                                      GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_fleet_view_compare_by_population);
}

static int
aw_fleet_view_compare_by_starbase (GtkTreeModel *model,
                                    GtkTreeIter  *iter_a,
                                    GtkTreeIter  *iter_b,
                                    gpointer      user_data)
{
  return aw_fleet_view_compare (model, iter_a, iter_b,
                                 aw_fleet_compare_by_starbase);
}

static void
aw_fleet_view_sort_by_starbase_cb (AwTreeView      *view,
                                    GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_fleet_view_compare_by_starbase);
}

static int
aw_fleet_view_compare_by_owner (GtkTreeModel *model,
                                 GtkTreeIter  *iter_a,
                                 GtkTreeIter  *iter_b,
                                 gpointer      user_data)
{
  return aw_fleet_view_compare (model, iter_a, iter_b,
                                 aw_fleet_compare_by_owner);
}

static void
aw_fleet_view_sort_by_owner_cb (AwTreeView      *view,
                                 GtkToggleButton *button)
{
  if (gtk_toggle_button_get_active (button))
    aw_tree_view_set_sort_func (view, aw_fleet_view_compare_by_owner);
}

static GSList *
aw_fleet_view_get_filters (AwTreeView *view)
{
  struct {
    const char *label;
    void      (*callback) ();
  } filters[] = {
      { _("Name"),       aw_fleet_view_sort_by_name_cb       },
      { _("Population"), aw_fleet_view_sort_by_population_cb },
      { _("Starbase"),   aw_fleet_view_sort_by_starbase_cb   },
      { _("Owner"),      aw_fleet_view_sort_by_owner_cb      },
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
#endif

static void
aw_fleet_view_title_cell_cb (GtkTreeViewColumn *column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *model,
                              GtkTreeIter       *iter,
                              gpointer           user_data)
{
  GString        *text       = g_string_new (NULL);
  PangoAttrList  *attributes = NULL;
  AwFleet        *fleet      = NULL;
  AwTreeView     *view;

  gtk_tree_model_get (model, iter, 0, &fleet, -1);

  if (fleet)
    {
      view = AW_TREE_VIEW (gtk_tree_view_column_get_tree_view (column));

      g_string_append (text, aw_fleet_get_planet_name (fleet));

      if (aw_fleet_get_arrival_time (fleet))
        {
          attributes = aw_tree_view_insert_font  (view, attributes,
                                                  AW_TREE_VIEW_FONT_SECONDARY,
                                                  text->len, G_MAXINT);
          attributes = aw_tree_view_insert_color (view, attributes,
                                                  AW_TREE_VIEW_COLOR_SECONDARY,
                                                  text->len, G_MAXINT);

          g_string_append_printf (text, "\n%s: %s - %s", _("Arrival"),
                                  aw_fleet_get_arrival_time (fleet),
                                  aw_fleet_get_arrival_date (fleet));
        }
      else if (aw_fleet_get_flags (fleet) & AW_FLEET_PENDING)
        {
          attributes = aw_tree_view_insert_font  (view, attributes,
                                                  AW_TREE_VIEW_FONT_SECONDARY,
                                                  text->len, G_MAXINT);
          attributes = aw_tree_view_insert_color (view, attributes,
                                                  AW_TREE_VIEW_COLOR_SECONDARY,
                                                  text->len, G_MAXINT);

          g_string_append_printf (text, "\n%s: %s", _("Arrival"), _("pending"));
        }
      else if (aw_fleet_get_flags (fleet) & AW_FLEET_SIEGING)
        {
          attributes = aw_tree_view_insert_color (view, attributes,
                                                  AW_TREE_VIEW_COLOR_ATTENTION,
                                                  0, G_MAXINT);
        }
    }

  g_object_set (cell, "text", text->str, "attributes", attributes, NULL);
  pango_attr_list_unref (attributes);
  g_string_free (text, TRUE);
  aw_fleet_unref (fleet);
}

static void
aw_fleet_view_vessel_cell_cb (GtkTreeViewColumn *column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *model,
                              GtkTreeIter       *iter,
                              gpointer           user_data)
{
  AwVesselType vessel1 = ((GPOINTER_TO_UINT (user_data) >> 0) & 255) - 1;
  AwVesselType vessel2 = ((GPOINTER_TO_UINT (user_data) >> 8) & 255) - 1;
  GString     *text    = g_string_new (NULL);
  AwFleet     *fleet   = NULL;
  int          amount;

  gtk_tree_model_get (model, iter, 0, &fleet, -1);

  if (fleet)
    {
      if (AW_VESSEL_INVALID != vessel1)
        {
          g_string_append (text, aw_vessel_get_display_name (vessel1, 0));
          amount = aw_fleet_get_vessels (fleet, vessel1);

          if (amount > 0)
            g_string_append_printf (text, ": %d\n", amount);
          else
            g_string_append_printf (text, ": -\n");
        }

      if (AW_VESSEL_INVALID != vessel2)
        {
          g_string_append (text, aw_vessel_get_display_name (vessel2, 0));
          amount = aw_fleet_get_vessels (fleet, vessel2);

          if (amount > 0)
            g_string_append_printf (text, ": %d", amount);
          else
            g_string_append_printf (text, ": -");
        }
    }

  g_object_set (cell, "text", text->str, NULL);
  g_string_free (text, TRUE);
  aw_fleet_unref (fleet);
}

static void
aw_fleet_view_combat_value_cell_cb (GtkTreeViewColumn *column,
                                    GtkCellRenderer   *cell,
                                    GtkTreeModel      *model,
                                    GtkTreeIter       *iter,
                                    gpointer           user_data)
{
  GString *text  = g_string_new (NULL);
  AwFleet *fleet = NULL;

  gtk_tree_model_get (model, iter, 0, &fleet, -1);

  if (fleet)
    {
      const int combat_value = aw_fleet_get_combat_value (fleet);
      const int build_cost   = 0;

      g_string_append_printf (text, "%s: ", _("Combat Value"));

      if (combat_value > 0)
        g_string_append_printf (text, "%d", combat_value);
      else
        g_string_append_c (text, '-');

      g_string_append_printf (text, "\n%s: %d", _("Build Cost"), build_cost);
    }

  g_object_set (cell, "text", text->str, NULL);
  g_string_free (text, TRUE);
  aw_fleet_unref (fleet);
}

static void
aw_fleet_view_create_columns (AwTreeView *view)
{
  GtkCellRenderer *cell;

  /* === title column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_primary_column (view, cell), cell,
     aw_fleet_view_title_cell_cb, NULL, NULL);

  /* === transports/colony ships column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell), cell,
     aw_fleet_view_vessel_cell_cb, AW_VESSEL_MASK (AW_VESSEL_TRANSPORT,
                                                   AW_VESSEL_COLONY_SHIP), NULL);

  /* === destroyer/cruiser column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell), cell,
     aw_fleet_view_vessel_cell_cb, AW_VESSEL_MASK (AW_VESSEL_DESTROYER,
                                                   AW_VESSEL_CRUISER), NULL);

  /* === battleship column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell), cell,
     aw_fleet_view_vessel_cell_cb, AW_VESSEL_MASK (AW_VESSEL_BATTLESHIP,
                                                   AW_VESSEL_INVALID), NULL);

  /* === combat value column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell), cell,
     aw_fleet_view_combat_value_cell_cb, NULL, NULL);
}

static void
aw_fleet_view_class_init (AwFleetViewClass *class)
{
  AwTreeViewClass *tree_view_class;

  tree_view_class                 = AW_TREE_VIEW_CLASS (class);
  tree_view_class->get_core_model = aw_fleet_view_get_core_model;
  tree_view_class->get_empty_text = aw_fleet_view_get_empty_text;
#if 0
  tree_view_class->get_filters    = aw_fleet_view_get_filters;
#endif
  tree_view_class->create_columns = aw_fleet_view_create_columns;

  signals[FLEET_ACTIVATED] = g_signal_new ("fleet-activated",
                                           AW_TYPE_FLEET_VIEW,
                                           G_SIGNAL_RUN_LAST, 0, NULL, NULL,
                                           g_cclosure_marshal_VOID__BOXED,
                                           G_TYPE_NONE, 1, AW_TYPE_FLEET);
}

GtkWidget *
aw_fleet_view_new (void)
{
  return gtk_widget_new (AW_TYPE_FLEET_VIEW, NULL);
}
