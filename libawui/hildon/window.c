#include "config.h"
#include "window.h"
#include "tree-view.h"

static GtkContainerClass *container_parent_class = NULL;

G_DEFINE_TYPE (AwWindow, aw_window, HILDON_TYPE_STACKABLE_WINDOW);

static void
aw_window_init (AwWindow *view)
{
}

static void
aw_window_vadjustment_changed_cb (GtkAdjustment *adjustment,
                                  GtkAlignment  *alignment)
{
  int left_margin  = HILDON_MARGIN_DEFAULT;
  int right_margin = HILDON_MARGIN_DEFAULT;

  if (adjustment->upper > adjustment->page_size)
    right_margin = 0;

  gtk_alignment_set_padding (alignment, 0, 0, left_margin, right_margin);
}

static void
aw_window_add_tree_view (GtkContainer *container,
                         GtkWidget    *child)
{
  GtkWidget     *alignment, *pa;
  GSList        *filters, *items;
  HildonAppMenu *menu;

  gtk_widget_push_composite_child ();

  alignment = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
  gtk_container_add (container, alignment);

  pa = hildon_pannable_area_new ();
  gtk_container_add (GTK_CONTAINER (alignment), pa);

  g_object_set (pa,
                "hscrollbar-policy", GTK_POLICY_NEVER,
                "vscrollbar-policy", GTK_POLICY_AUTOMATIC,
                "mov-mode", HILDON_MOVEMENT_MODE_VERT, NULL);

  g_signal_connect
    (hildon_pannable_area_get_vadjustment (HILDON_PANNABLE_AREA (pa)),
     "changed", G_CALLBACK (aw_window_vadjustment_changed_cb),
     alignment);

  gtk_widget_pop_composite_child ();

  gtk_container_add (GTK_CONTAINER (pa), child);

  filters = aw_tree_view_get_filters (AW_TREE_VIEW (child));
  items = aw_tree_view_get_menu_items (AW_TREE_VIEW (child));

  if (filters || items)
    {
      menu = HILDON_APP_MENU (hildon_app_menu_new ());

      while (filters)
        {
          hildon_app_menu_add_filter (menu, filters->data);
          filters = filters->next;
        }

      while (items)
        {
          hildon_app_menu_append (menu, items->data);
          items = items->next;
        }

      hildon_window_set_app_menu (HILDON_WINDOW (container), menu);
    }
}

static void
aw_window_add (GtkContainer *container,
               GtkWidget    *child)
{
  if (AW_IS_TREE_VIEW (child))
    aw_window_add_tree_view (container, child);
  else
    container_parent_class->add (container, child);
}

static void
aw_window_class_init (AwWindowClass *class)
{
  GtkContainerClass *container_class;

  container_class      = GTK_CONTAINER_CLASS (class);
  container_class->add = aw_window_add;

  container_parent_class = GTK_CONTAINER_CLASS (aw_window_parent_class);
}

GtkWidget *
aw_window_new (const char *title)
{
  return gtk_widget_new (AW_TYPE_WINDOW, "title", title, NULL);
}

