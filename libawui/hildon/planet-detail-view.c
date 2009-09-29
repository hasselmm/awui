#include "config.h"
#include "planet-detail-view.h"

#include <glib/gi18n-lib.h>

struct _AwPlanetDetailViewPrivate {
  AwPlanet *planet;
};

G_DEFINE_TYPE (AwPlanetDetailView, aw_planet_detail_view,
               HILDON_TYPE_STACKABLE_WINDOW);

static GtkToolbar *
aw_planet_detail_view_create_toolbar (AwPlanetDetailView *view)
{
  GtkWidget   *toolbar, *label;
  GtkToolItem *item;

  toolbar = gtk_toolbar_new ();

  label = gtk_label_new ("Population: a (+b)");
  gtk_misc_set_padding (GTK_MISC (label), HILDON_MARGIN_DOUBLE, 0);
  item = gtk_tool_item_new ();
  gtk_container_add (GTK_CONTAINER (item), label);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

  label = gtk_label_new ("Production: c (+d)");
  gtk_misc_set_padding (GTK_MISC (label), HILDON_MARGIN_DOUBLE, 0);
  item = gtk_tool_item_new ();
  gtk_container_add (GTK_CONTAINER (item), label);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

  item = gtk_separator_tool_item_new ();
  gtk_tool_item_set_expand (item, TRUE);
  gtk_separator_tool_item_set_draw (GTK_SEPARATOR_TOOL_ITEM (item), FALSE);
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

  item = gtk_tool_button_new (NULL, _("Previous"));
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

  item = gtk_tool_button_new (NULL, _("Next Planet"));
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

  gtk_widget_show_all (toolbar);

  return GTK_TOOLBAR (toolbar);
}

static GtkWidget *
aw_planet_detail_view_create_button (GtkSizeGroup *group,
                                     const char   *title)
                                     
{
#if 0
  return hildon_button_new_with_text (HILDON_SIZE_AUTO_WIDTH |
                                      HILDON_SIZE_FINGER_HEIGHT,
                                      HILDON_BUTTON_ARRANGEMENT_VERTICAL,
                                      title, NULL);
#else
  GtkWidget *button, *box, *align, *widget;

  button = hildon_gtk_button_new (HILDON_SIZE_AUTO_WIDTH |
                                  HILDON_SIZE_FINGER_HEIGHT);

  box = gtk_hbox_new (FALSE, HILDON_MARGIN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (button), box);

  widget = gtk_label_new (title);
  gtk_size_group_add_widget (group, widget);
  gtk_box_pack_start (GTK_BOX (box), widget, FALSE, FALSE, 0);

  align = gtk_alignment_new (0.5, 0.5, 1.0, 0.0);
  gtk_box_pack_start (GTK_BOX (box), align, TRUE, TRUE, 0);

  widget = gtk_progress_bar_new ();
  gtk_widget_set_name (widget, "small-progress-bar");
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (widget), 0.3);
  gtk_container_add (GTK_CONTAINER (align), widget);

  return button;
#endif
}

static void
aw_planet_detail_view_init (AwPlanetDetailView *view)
{
  GtkWidget      *table, *button;
  GtkSizeGroup   *title_group;
  AwBuildingType  building;
  AwVesselType    vessel;
  const char     *title;

  view->priv = G_TYPE_INSTANCE_GET_PRIVATE (view, AW_TYPE_PLANET_DETAIL_VIEW,
                                            AwPlanetDetailViewPrivate);

  table = gtk_table_new (5, 2, TRUE);
  title_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

  for (building = 0; building < AW_BUILDING_LAST; ++building)
    {
      title  = aw_building_get_display_name (building, 0);
      button = aw_planet_detail_view_create_button (title_group, title);
      gtk_table_attach_defaults (GTK_TABLE (table), button,
                                 0, 1, building, building + 1);
    }

  for (vessel = 0; vessel < AW_BUILDING_LAST; ++vessel)
    {
      title  = aw_vessel_get_display_name (vessel, 0);
      button = aw_planet_detail_view_create_button (title_group, title);
      gtk_table_attach_defaults (GTK_TABLE (table), button,
                                 1, 2, vessel, vessel + 1);
    }

  gtk_container_add (GTK_CONTAINER (view), table);
  gtk_widget_show_all (table);

  hildon_window_add_toolbar (HILDON_WINDOW (view),
                             aw_planet_detail_view_create_toolbar (view));
}

static void
aw_planet_detail_view_class_init (AwPlanetDetailViewClass *class)
{
  g_type_class_add_private (class, sizeof (AwPlanetDetailViewPrivate));
}

GtkWidget *
aw_planet_detail_view_new (AwPlanet *planet)
{
  return gtk_widget_new (AW_TYPE_PLANET_DETAIL_VIEW, "planet", planet);
}

void
aw_planet_detail_view_set_planet (AwPlanetDetailView *view,
                                  AwPlanet           *planet)
{
  g_return_if_fail (AW_IS_PLANET_DETAIL_VIEW (view));
  g_object_set (view, "planet", planet, NULL);
}

AwPlanet *
aw_planet_detail_view_get_planet (AwPlanetDetailView *view)
{
  g_return_val_if_fail (AW_IS_PLANET_DETAIL_VIEW (view),  NULL);
  return view->priv->planet;
}

