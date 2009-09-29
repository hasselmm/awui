#include "config.h"
#include "news-view.h"

#include <libawui/model.h>
#include <glib/gi18n-lib.h>

enum {
  NEWS_ACTIVATED,
  LAST_SIGNAL
};

static unsigned signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (AwNewsView, aw_news_view, AW_TYPE_TREE_VIEW);

static void
aw_news_view_row_tapped_cb (GtkTreeView *view,
                            GtkTreePath *path)
{
  AwNews       *news = NULL;
  GtkTreeModel *model;
  GtkTreeIter   iter;

  model = gtk_tree_view_get_model (view);

  if (gtk_tree_model_get_iter (model, &iter, path))
    gtk_tree_model_get (model, &iter, 0, &news, -1);

  g_signal_emit (view, signals[NEWS_ACTIVATED], 0, news);

  aw_news_unref (news);
}

static void
aw_news_view_init (AwNewsView *view)
{
  g_signal_connect (view, "hildon-row-tapped",
                    G_CALLBACK (aw_news_view_row_tapped_cb), NULL);
}

static GType *
aw_news_view_get_core_model (AwTreeView *view)
{
  GType types[2] = { AW_TYPE_NEWS, G_TYPE_INVALID };
  return g_memdup (types, sizeof types);
}

static const char *
aw_news_view_get_empty_text (AwTreeView *view)
{
  return _("(No News)");
}

static void
aw_news_view_text_cell_cb (GtkTreeViewColumn *column,
                           GtkCellRenderer   *cell,
                           GtkTreeModel      *model,
                           GtkTreeIter       *iter,
                           gpointer           data)
{
  PangoAttrList *attributes = NULL;
  const char    *markup     = NULL;
  AwTreeView    *view;
  AwNews        *news;

  gtk_tree_model_get (model, iter, 0, &news, -1);

  if (news)
    {
      view = AW_TREE_VIEW (gtk_tree_view_column_get_tree_view (column));

      markup = aw_news_get_text (news);

      if (aw_news_get_flags (news) & AW_NEWS_IMPORTANT)
        {
          attributes = aw_tree_view_insert_color (view, attributes,
                                                  AW_TREE_VIEW_COLOR_ATTENTION,
                                                  0, G_MAXINT);
        }
    }

  g_object_set (cell,
                "markup", markup, "attributes", attributes,
                "wrap-width", 680, NULL); // FIXME gtk_tree_view_column_get_width (column)
  pango_attr_list_unref (attributes);
  aw_news_unref (news);
}

static void
aw_news_view_time_cell_cb (GtkTreeViewColumn *column,
                           GtkCellRenderer   *cell,
                           GtkTreeModel      *model,
                           GtkTreeIter       *iter,
                           gpointer           user_data)
{
  GString        *text       = g_string_new (NULL);
  PangoAttrList  *attributes = NULL;
  AwNews         *news       = NULL;
  AwTreeView     *view;

  gtk_tree_model_get (model, iter, 0, &news, -1);

  if (news)
    {
      const AwTreeViewColorName color =
        (aw_news_get_flags (news) & AW_NEWS_IMPORTANT ?
         AW_TREE_VIEW_COLOR_ATTENTION : AW_TREE_VIEW_COLOR_SECONDARY);

      view = AW_TREE_VIEW (gtk_tree_view_column_get_tree_view (column));

      attributes = aw_tree_view_insert_color (view, attributes,
                                              color, 0, G_MAXINT);

      g_string_append (text, aw_news_get_time (news));

      attributes = aw_tree_view_insert_font (view, attributes,
                                             AW_TREE_VIEW_FONT_BOLD,
                                             0, text->len);

      g_string_append (text, "\n");
      g_string_append (text, aw_news_get_date (news));
    }

  g_object_set (cell, "text", text->str, "attributes", attributes, NULL);
  pango_attr_list_unref (attributes);
  g_string_free (text, TRUE);
  aw_news_unref (news);
}

static void
aw_news_view_create_columns (AwTreeView *view)
{
  GtkCellRenderer *cell;

  /* === text column === */

  cell = gtk_cell_renderer_text_new ();
  g_object_set (cell, "wrap-mode", PANGO_WRAP_WORD_CHAR, NULL);

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_primary_column (view, cell),
     cell, aw_news_view_text_cell_cb, NULL, NULL);

  /* === time column === */

  cell = gtk_cell_renderer_text_new ();

  gtk_tree_view_column_set_cell_data_func
    (aw_tree_view_append_secondary_column (view, cell),
     cell, aw_news_view_time_cell_cb, NULL, NULL);
}

static void
aw_news_view_class_init (AwNewsViewClass *class)
{
  AwTreeViewClass *tree_view_class;

  tree_view_class                 = AW_TREE_VIEW_CLASS (class);
  tree_view_class->get_core_model = aw_news_view_get_core_model;
  tree_view_class->get_empty_text = aw_news_view_get_empty_text;
  tree_view_class->create_columns = aw_news_view_create_columns;

  signals[NEWS_ACTIVATED] = g_signal_new ("news-activated",
                                          AW_TYPE_NEWS_VIEW,
                                          G_SIGNAL_RUN_LAST, 0, NULL, NULL,
                                          g_cclosure_marshal_VOID__BOXED,
                                          G_TYPE_NONE, 1, AW_TYPE_NEWS);
}

GtkWidget *
aw_news_view_new (void)
{
  return gtk_widget_new (AW_TYPE_NEWS_VIEW, NULL);
}

