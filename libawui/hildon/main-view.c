#include "config.h"
#include "main-view.h"
#include "enums.h"

#include <libawui/model.h>
#include <hildon/hildon.h>
#include <glib/gi18n-lib.h>

enum {
  ACTION_STARTED,
  LAST_SIGNAL,
};

struct _AwMainViewPrivate {
  GtkAlignment  parent;
  GtkWidget    *buttons[AW_MAIN_VIEW_ACTION_LAST];
  GtkWidget    *table;
};

static unsigned signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (AwMainView, aw_main_view, GTK_TYPE_ALIGNMENT);

static void
aw_main_view_button_clicked_cb (GtkWidget *button,
                                gpointer   user_data)
{
  AwMainViewAction  action;
  GtkWidget        *view;

  action = GPOINTER_TO_UINT (user_data);
  view = gtk_widget_get_ancestor (button, AW_TYPE_MAIN_VIEW);
  g_signal_emit (view, signals[ACTION_STARTED], 0, action);
}

static void
aw_main_view_create_button (AwMainView       *view,
                            AwMainViewAction  action,
                            int               column,
                            int               width,
                            int               row,
                            const char       *title)
{
  GtkWidget *button;

  button = hildon_button_new_with_text
    (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_THUMB_HEIGHT,
     HILDON_BUTTON_ARRANGEMENT_VERTICAL, title, NULL),

  gtk_table_attach_defaults (GTK_TABLE (view->priv->table), button,
                             column, column + width, row, row + 1);

  hildon_button_set_title_alignment (HILDON_BUTTON (button), 0.5, 1.0);
  hildon_button_set_value_alignment (HILDON_BUTTON (button), 0.5, 0.5);

  g_signal_connect (button, "clicked",
                    G_CALLBACK (aw_main_view_button_clicked_cb),
                    GUINT_TO_POINTER (action));

  view->priv->buttons[action] = button;
}

static void
aw_main_view_refresh (AwMainView *view)
{
  GtkWidget *button;
  char      *alliance_tag = NULL;
  int        player_level = 0;

  g_object_get (aw_settings_get_singleton (),
                "alliance-tag", &alliance_tag,
                "player-level", &player_level, NULL);

  button = view->priv->buttons[AW_MAIN_VIEW_ACTION_TRADE];

  if (player_level < 1)
    {
      hildon_button_set_value (HILDON_BUTTON (button),
                               _("Player level 1 required"));
      gtk_widget_set_sensitive (button, FALSE);
    }

  button = view->priv->buttons[AW_MAIN_VIEW_ACTION_ALLIANCE];

  if (player_level < 5)
    {
      hildon_button_set_value (HILDON_BUTTON (button),
                               _("Player level 5 required"));
      gtk_widget_set_sensitive (button, FALSE);
    }
  else
    {
      hildon_button_set_value (HILDON_BUTTON (button),
                               alliance_tag ? alliance_tag : _("none"));
      gtk_widget_set_sensitive (button, TRUE);
    }

  /* FIXME: retreive this information */
#if 0
  hildon_button_set_value (HILDON_BUTTON (view->news_button), "TODO: warning: 2 failed logins");
  hildon_button_set_value (HILDON_BUTTON (view->map_button), "TODO: 7 stars within 5 pc");
  hildon_button_set_value (HILDON_BUTTON (view->planets_button), "TODO: 5 planets, 15 mrd people, 123 PP/h");
  hildon_button_set_value (HILDON_BUTTON (view->fleet_button), "TODO: 3 fleets, 1000 CV");
  hildon_button_set_value (HILDON_BUTTON (view->science_button), "TODO: Energy: 5:32h, Culture: 7:43h");
#endif

  g_free (alliance_tag);
}

static void
aw_main_view_init (AwMainView *view)
{
  view->priv = G_TYPE_INSTANCE_GET_PRIVATE (view, AW_TYPE_MAIN_VIEW,
                                            AwMainViewPrivate);

  gtk_alignment_set_padding (GTK_ALIGNMENT (view),
                             0, 0, HILDON_MARGIN_DOUBLE,
                             HILDON_MARGIN_DOUBLE);
  gtk_alignment_set (GTK_ALIGNMENT (view),
                     0.5, 0.5, 1.0, 0.0);

  view->priv->table = gtk_table_new (3, 6, TRUE);
  gtk_container_add (GTK_CONTAINER (view), view->priv->table);

  aw_main_view_create_button (view, AW_MAIN_VIEW_ACTION_NEWS,     0, 4, 0, _("News"));
  aw_main_view_create_button (view, AW_MAIN_VIEW_ACTION_MAP,      4, 2, 0, _("Map"));

  aw_main_view_create_button (view, AW_MAIN_VIEW_ACTION_PLANETS,  0, 4, 1, _("Planets"));
  aw_main_view_create_button (view, AW_MAIN_VIEW_ACTION_FLEET,    4, 2, 1, _("Fleet"));

  aw_main_view_create_button (view, AW_MAIN_VIEW_ACTION_TRADE,    0, 2, 2, _("Trade"));
  aw_main_view_create_button (view, AW_MAIN_VIEW_ACTION_SCIENCE,  2, 2, 2, _("Science"));
  aw_main_view_create_button (view, AW_MAIN_VIEW_ACTION_ALLIANCE, 4, 2, 2, _("Alliance"));

  gtk_widget_show_all (view->priv->table);

  g_signal_connect_swapped (aw_settings_get_singleton (), "notify::alliance-tag",
                            G_CALLBACK (aw_main_view_refresh), view);
  g_signal_connect_swapped (aw_settings_get_singleton (), "notify::player-level",
                            G_CALLBACK (aw_main_view_refresh), view);

  aw_main_view_refresh (view);
}

static void
aw_main_view_class_init (AwMainViewClass *class)
{
  signals[ACTION_STARTED] = g_signal_new ("action-started",
                                          AW_TYPE_MAIN_VIEW,
                                          G_SIGNAL_RUN_LAST, 0, NULL, NULL,
                                          g_cclosure_marshal_VOID__ENUM,
                                          G_TYPE_NONE, 1, AW_TYPE_MAIN_VIEW_ACTION);

  g_type_class_add_private (class, sizeof (AwMainViewPrivate));
}

GtkWidget *
aw_main_view_new (void)
{
  return gtk_widget_new (AW_TYPE_MAIN_VIEW, NULL);
}
