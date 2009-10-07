#include "config.h"
#include "main-view.h"
#include "enums.h"

#include <libawui/model.h>
#include <hildon/hildon.h>
#include <glib/gi18n-lib.h>
#include <math.h>

typedef enum {
  PROP_LATEST_NEWS = 1,
  PROP_PROFILE,
} AwMainViewPropId;

enum {
  ACTION_STARTED,
  LAST_SIGNAL,
};

struct _AwMainViewPrivate {
  GtkAlignment  parent;
  GtkWidget    *buttons[AW_MAIN_VIEW_ACTION_LAST];
  GtkWidget    *table;

  AwNews       *latest_news;
  AwProfile    *profile;
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
  AwProfile *profile;
  GtkWidget *button;
  char      *text;

  profile = aw_profile_get_self ();

  button = view->priv->buttons[AW_MAIN_VIEW_ACTION_NEWS];

  // FIXME: do something about news markup
  hildon_button_set_value (HILDON_BUTTON (button),
                           view->priv->latest_news ?
                           aw_news_get_text (view->priv->latest_news) :
                           NULL);

  button = view->priv->buttons[AW_MAIN_VIEW_ACTION_TRADE];

  if (aw_profile_get_level (profile, AW_SCIENCE_WARFARE) < 1)
    {
      hildon_button_set_value (HILDON_BUTTON (button),
                               _("Player level 1 required"));
      gtk_widget_set_sensitive (button, FALSE);
    }
  else
    {
      text = g_strdup_printf (_("Trade Revenue: %+.f %%"),
                              aw_profile_get_bonus (profile, AW_BONUS_TRADE) * 100 - 100);
      hildon_button_set_value (HILDON_BUTTON (button), text);
      gtk_widget_set_sensitive (button, TRUE);
      g_free (text);
    }

  button = view->priv->buttons[AW_MAIN_VIEW_ACTION_ALLIANCE];

  if (aw_profile_get_level (profile, AW_SCIENCE_WARFARE) < 5)
    {
      hildon_button_set_value (HILDON_BUTTON (button),
                               _("Player level 5 required"));
      gtk_widget_set_sensitive (button, FALSE);
    }
  else
    {
      hildon_button_set_value (HILDON_BUTTON (button),
                               aw_profile_get_alliance_tag (profile) ?
                               aw_profile_get_alliance_tag (profile) :
                               _("none"));
      gtk_widget_set_sensitive (button, TRUE);
    }

  button = view->priv->buttons[AW_MAIN_VIEW_ACTION_MAP];

  text = g_strdup_printf (_("Origin: (%+d/%+d)"),
                          aw_profile_get_origin (profile)->x,
                          aw_profile_get_origin (profile)->y);
  hildon_button_set_value (HILDON_BUTTON (button), text);
  g_free (text);

  button = view->priv->buttons[AW_MAIN_VIEW_ACTION_SCIENCE];

  text = g_strdup_printf (_("Science Level: %.f, Culture Level: %.f"),
                          floor (aw_profile_get_level (profile, AW_SCIENCE_OVERALL)),
                          floor (aw_profile_get_level (profile, AW_SCIENCE_CULTURE)));
  hildon_button_set_value (HILDON_BUTTON (button), text);
  g_free (text);

  button = view->priv->buttons[AW_MAIN_VIEW_ACTION_FLEET];

  text = g_strdup_printf (_("Player Level: %.f"),
                          floor (aw_profile_get_level (profile, AW_SCIENCE_WARFARE)));
  hildon_button_set_value (HILDON_BUTTON (button), text);
  g_free (text);

  /* FIXME: retreive this information */
#if 0
  hildon_button_set_value (HILDON_BUTTON (view->map_button), "TODO: 7 stars within 5 pc");
  hildon_button_set_value (HILDON_BUTTON (view->planets_button), "TODO: 5 planets, 15 mrd people, 123 PP/h");
  hildon_button_set_value (HILDON_BUTTON (view->fleet_button), "TODO: 3 fleets, 1000 CV");
  hildon_button_set_value (HILDON_BUTTON (view->science_button), "TODO: Energy: 5:32h, Culture: 7:43h");
  hildon_button_set_value (HILDON_BUTTON (view->trade_button), "TODO: 1234 A$, Trade Revenue: 13%");
#endif

  g_object_unref (profile);
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

/* FIXME
  g_signal_connect_swapped (aw_settings_get_singleton (), "notify::alliance-tag",
                            G_CALLBACK (aw_main_view_refresh), view);
  g_signal_connect_swapped (aw_settings_get_singleton (), "notify::player-level",
                            G_CALLBACK (aw_main_view_refresh), view);
 */
  aw_main_view_refresh (view);
}

static void
aw_main_view_finalize (GObject *object)
{
  AwMainView *view = AW_MAIN_VIEW (object);

  if (view->priv->latest_news)
    g_object_ref (view->priv->latest_news);
  if (view->priv->profile)
    g_object_ref (view->priv->profile);

  G_OBJECT_CLASS (aw_main_view_parent_class)->finalize (object);
}

static void
aw_main_view_set_property (GObject      *object,
                           unsigned      prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  AwMainView *view = AW_MAIN_VIEW (object);

  switch ((AwMainViewPropId) prop_id) {
  case PROP_LATEST_NEWS:
    if (view->priv->latest_news)
      g_object_unref (view->priv->latest_news);

    view->priv->latest_news = g_value_dup_object (value);
    aw_main_view_refresh (view);
    return;
  case PROP_PROFILE:
    if (view->priv->profile)
      g_object_unref (view->priv->profile);

    view->priv->profile = g_value_dup_object (value);
    aw_main_view_refresh (view);
    return;
  }

  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
aw_main_view_get_property (GObject    *object,
                           unsigned    prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  AwMainView *view = AW_MAIN_VIEW (object);

  switch ((AwMainViewPropId) prop_id) {
  case PROP_LATEST_NEWS:
    g_value_set_object (value, view->priv->latest_news);
    return;
  case PROP_PROFILE:
    g_value_set_object (value, view->priv->profile);
    return;
  }

  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
aw_main_view_class_init (AwMainViewClass *class)
{
  GObjectClass *object_class;

  object_class               = G_OBJECT_CLASS (class);
  object_class->finalize     = aw_main_view_finalize;
  object_class->set_property = aw_main_view_set_property;
  object_class->get_property = aw_main_view_get_property;

  g_object_class_install_property
    (object_class, PROP_LATEST_NEWS,
     g_param_spec_object ("latest-news",
                          "Latest News",
                          "Latest news for the player",
                          AW_TYPE_NEWS,
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class, PROP_PROFILE,
     g_param_spec_object ("profile",
                          "Profile",
                          "The player's profile",
                          AW_TYPE_PROFILE,
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

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

void
aw_main_view_set_latest_news (AwMainView *view,
                              AwNews     *news)
{
  g_return_if_fail (AW_IS_MAIN_VIEW (view));
  g_return_if_fail (AW_IS_NEWS (news) || !news);
  g_object_set (view, "latest-news", news, NULL);
}

AwNews *
aw_main_view_get_latest_news (AwMainView *view)
{
  g_return_val_if_fail (AW_IS_MAIN_VIEW (view), NULL);
  return view->priv->latest_news;
}

void
aw_main_view_set_profile (AwMainView *view,
                          AwProfile  *profile)
{
  g_return_if_fail (AW_IS_MAIN_VIEW (view));
  g_return_if_fail (AW_IS_PROFILE (profile) || !profile);
  g_object_set (view, "profile", profile, NULL);
}

AwProfile *
aw_main_view_get_profile (AwMainView *view)
{
  g_return_val_if_fail (AW_IS_MAIN_VIEW (view), NULL);
  return view->priv->profile;
}


