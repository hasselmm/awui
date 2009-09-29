#include "config.h"
#include "system-view.h"
#include "tree-view.h"

#include <libawui/model.h>
#include <glib/gi18n-lib.h>

G_DEFINE_TYPE (AwSystemView, aw_system_view, AW_TYPE_PLANET_VIEW);

static void
aw_system_view_init (AwSystemView *view)
{
}

static void
aw_system_view_title_cell_cb (GtkTreeViewColumn *column,
                              GtkCellRenderer   *cell,
                              GtkTreeModel      *model,
                              GtkTreeIter       *iter,
                              gpointer           user_data)
{
  GString        *text       = g_string_new (NULL);
  PangoAttrList  *attributes = NULL;
  AwPlanet       *planet     = NULL;
  AwTreeView     *view;

  gtk_tree_model_get (model, iter, 0, &planet, -1);

  if (planet)
    {
      view = AW_TREE_VIEW (gtk_tree_view_column_get_tree_view (column));

      g_string_append (text, aw_planet_get_name (planet));
      g_string_append (text, "\n");

      attributes = aw_tree_view_insert_font (view, attributes,
                                             AW_TREE_VIEW_FONT_SECONDARY,
                                             text->len, G_MAXINT);

      if (aw_planet_get_flags (planet) & AW_PLANET_SIEGED)
        {
          attributes = aw_tree_view_insert_color (view, attributes,
                                                  AW_TREE_VIEW_COLOR_ATTENTION,
                                                  0, G_MAXINT);
        }
      else if (aw_planet_get_owner (planet))
        {
          attributes = aw_tree_view_insert_color (view, attributes,
                                                  AW_TREE_VIEW_COLOR_SECONDARY,
                                                  text->len, G_MAXINT);
        }
      else
        {
          attributes = aw_tree_view_insert_color (view, attributes,
                                                  AW_TREE_VIEW_COLOR_SECONDARY,
                                                  0, G_MAXINT);
        }

      if (!aw_planet_get_owner (planet))
        {
          attributes = aw_tree_view_insert_font (view, attributes,
                                                 AW_TREE_VIEW_FONT_ITALIC,
                                                 0, text->len);
          attributes = aw_tree_view_insert_font (view, attributes,
                                                 AW_TREE_VIEW_FONT_ITALIC,
                                                 text->len, G_MAXINT);

          g_string_append_printf (text,  _("empty planet"));
        }
      else
        g_string_append (text, aw_planet_get_owner (planet));
    }

  g_object_set (cell, "text", text->str, "attributes", attributes, NULL);
  pango_attr_list_unref (attributes);
  g_string_free (text, TRUE);
  aw_planet_unref (planet);
}

static void
aw_system_view_detail_cell_cb (GtkTreeViewColumn *column,
                               GtkCellRenderer   *cell,
                               GtkTreeModel      *model,
                               GtkTreeIter       *iter,
                               gpointer           user_data)
{
  AwPlanet *planet = NULL;
  char     *text = NULL;

  gtk_tree_model_get (model, iter, 0, &planet, -1);

  if (planet)
    {
      text = g_strdup_printf ("%s: %d\n%s: %d",
                              _("Population"), aw_planet_get_population (planet),
                              _("Starbase"), aw_planet_get_starbase (planet));
    }

  g_object_set (cell, "text", text, NULL);

  aw_planet_unref (planet);
  g_free (text);
}

static void
aw_system_view_create_columns (AwTreeView *view)
{
  GtkCellRenderer *cell;

  /* === title column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_primary_column (view, cell),
     cell, aw_system_view_title_cell_cb, NULL, NULL);

  /* === detail column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell),
     cell, aw_system_view_detail_cell_cb, NULL, NULL);
}

static void
aw_system_view_class_init (AwSystemViewClass *class)
{
  AwTreeViewClass *tree_view_class;

  tree_view_class                 = AW_TREE_VIEW_CLASS (class);
  tree_view_class->create_columns = aw_system_view_create_columns;
}

GtkWidget *
aw_system_view_new (void)
{
  return gtk_widget_new (AW_TYPE_SYSTEM_VIEW, NULL);
}

static gboolean
aw_system_view_row_matches_planet_name (GtkTreeModel *model,
                                        GtkTreeIter  *iter,
                                        const char   *name)
{
  AwPlanet *planet = NULL;
  gboolean  matches;

  gtk_tree_model_get (model, iter, 0, &planet, -1);
  matches = (planet && !g_strcmp0 (name, aw_planet_get_name (planet)));
  aw_planet_unref (planet);

  return matches;
}

typedef struct {
  AwSystemView *view;
  char         *name;
} AwSystemViewClosure;

static void
aw_system_view_closure_free (AwSystemViewClosure *closure)
{
  g_free (closure->name);
  g_free (closure);
}

static void
aw_system_view_row_inserted_cb (GtkTreeModel        *model,
                                GtkTreePath         *path,
                                GtkTreeIter         *iter,
                                AwSystemViewClosure *closure)
{
  if (aw_system_view_row_matches_planet_name (model, iter, closure->name))
    {
      g_print ("%p %s\n", closure->view, closure->name);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (closure->view), path, NULL,
                                  FALSE, 0.0, 0.0);
    }
}

static void
aw_system_view_notify_model_cb (AwSystemView *view,
                                GParamSpec   *pspec,
                                const char   *name)
{
  aw_system_view_select_planet_by_name (view, name);
}

gboolean
aw_system_view_select_planet_by_name (AwSystemView *view,
                                      const char   *name)
{
  GtkTreeModel        *model;
  GtkTreeIter          iter;
  GtkTreePath         *path;
  AwSystemViewClosure *closure;

  g_return_val_if_fail (AW_IS_SYSTEM_VIEW (view), FALSE);
  g_return_val_if_fail (NULL != name,             FALSE);

  g_signal_handlers_disconnect_matched (view, G_SIGNAL_MATCH_FUNC, 0, 0, NULL,
                                        aw_system_view_notify_model_cb, NULL);

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));

  if (!model)
    {
      g_signal_connect_data (view, "notify::model",
                             G_CALLBACK (aw_system_view_notify_model_cb),
                             g_strdup (name), (GClosureNotify) g_free, 0);

      return FALSE;
    }

  g_signal_handlers_disconnect_matched (model, G_SIGNAL_MATCH_FUNC, 0, 0, NULL,
                                        aw_system_view_row_inserted_cb, NULL);

  if (gtk_tree_model_get_iter_first (model, &iter))
    do
      {
        if (aw_system_view_row_matches_planet_name (model, &iter, name))
          {
            path = gtk_tree_model_get_path (model, &iter);
            gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (view), path, NULL,
                                          FALSE, 0.0, 0.0);
            gtk_tree_path_free (path);

            return TRUE;
          }
      }
    while (gtk_tree_model_iter_next (model, &iter));

  closure = g_new0 (AwSystemViewClosure, 1);
  closure->name = g_strdup (name);
  closure->view = view;

  g_signal_connect_data (model, "row-inserted",
                         G_CALLBACK (aw_system_view_row_inserted_cb),
                         closure, (GClosureNotify) aw_system_view_closure_free,
                         G_CONNECT_AFTER);

  return FALSE;
}

