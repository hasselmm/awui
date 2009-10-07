#include "main.h"

#include <glib/gi18n.h>
#include <libawui/model.h>

typedef GList * (* AwFetchItemsFinishFunc) (AwSession     *session,
                                            GAsyncResult  *result,
                                            GError       **error);

typedef void    (* AwProcessItemsFunc)     (AwTreeView *view,
                                            GList      *list);

static GtkWidget *main_view = NULL;

static GtkWidget *
show_view (const char *title,
           GtkWidget  *view,
           const char *first_action,
           ...)
{
  GtkWidget     *window, *button;
  HildonAppMenu *app_menu;
  GCallback      callback;
  const char    *label;
  va_list        args;

  window = aw_window_new (title);
  gtk_container_add (GTK_CONTAINER (window), view);
  gtk_widget_show_all (window);

  app_menu = hildon_window_get_app_menu (HILDON_WINDOW (window));

  va_start (args, first_action);

  for (label = first_action; label; )
    {
      button = gtk_button_new_with_label (label);
      hildon_app_menu_append (app_menu, GTK_BUTTON (button));
      gtk_widget_show (button);

      callback = va_arg (args, GCallback);
      g_signal_connect (button, "clicked", callback, view);

      label = va_arg (args, const char *);
    }

  va_end (args);

  return window;
}

static GList *
fetch_items (GObject                *source,
             GAsyncResult           *result,
             gpointer                view,
             AwFetchItemsFinishFunc  fetch_items_finish,
             AwProcessItemsFunc      process_items)
{
  GError *error = NULL;
  GList  *items = NULL;

  items = fetch_items_finish (AW_SESSION (source), result, &error);

  if (error)
    {
      hildon_banner_show_information (view, NULL, error->message);
      g_error_free (error);
    }
  else
    process_items (view, items);

  return items;
}

static void
fetch_news_cb (GObject      *source,
               GAsyncResult *result,
               gpointer      view)
{
  GtkTreeModel *model;
  GList        *news;
  int           n;

  news = fetch_items (source, result, view,
                      aw_session_fetch_news_finish,
                      aw_tree_view_append_items);

  if (news)
    {
      model = gtk_tree_view_get_model (view);

      n = gtk_tree_model_iter_n_children (model, NULL);
      n = (n + 4) / 5;

      if (0 == n)
        aw_main_view_set_latest_news (AW_MAIN_VIEW (main_view), news->data);

      aw_session_fetch_news_async (AW_SESSION (source),
                                   n, fetch_news_cb, view);
    }

}

static void
fetch_map_cb (GObject      *source,
              GAsyncResult *result,
              gpointer      view)
{
  fetch_items (source, result, view,
               aw_session_fetch_map_finish,
               aw_tree_view_append_items);
}

static void
news_activated_cb (GtkWidget *view,
                   AwNews    *news,
                   AwSession *session)
{
  char **links, **p;

  links = aw_news_get_links (news);

  if (links)
    for (p = links; *p; ++p)
      {
        g_print ("%s\n", *p);
        hildon_banner_show_information (view, NULL, *p);
      }
}

static void
planet_activated_cb (GtkWidget *view,
                     AwPlanet  *planet,
                     AwSession *session)
{
  gtk_widget_show (aw_planet_detail_view_new (planet));
}

static void
fetch_system_cb (GObject      *source,
                 GAsyncResult *result,
                 gpointer      view)
{
  fetch_items (source, result, view,
               aw_session_fetch_system_finish,
               aw_tree_view_append_items);
}

static void
show_star_system (AwSession  *session,
                  int         system_id,
                  const char *system_name,
                  const char *planet_name)
{
  GtkWidget *view = aw_system_view_new ();
  g_signal_connect (view, "planet-activated",
                    G_CALLBACK (planet_activated_cb), session);
  aw_session_fetch_system_async (session, system_id, fetch_system_cb, view);
  show_view (system_name, view, NULL);

  if (planet_name)
    aw_system_view_select_planet_by_name (AW_SYSTEM_VIEW (view), planet_name);
}

static void
star_activated_cb (GtkWidget *view,
                   AwStar    *star,
                   AwSession *session)
{
  show_star_system (session, aw_star_get_id (star),
                    aw_star_get_name (star), NULL);
}

#if 0
static void
session_cb (GObject             *source,
            GAsyncResult        *result,
            gpointer             view,
            AwSessionFinishFunc  finish)
{
  GError *error = NULL;

  items = fetch_items_finish (AW_SESSION (source), result, &error);

  if (error)
    {
      hildon_banner_show_information (view, NULL, error->message);
      g_error_free (error);
    }
}
#endif

static void
science_id_cb (AwScienceView *view,
               GParamSpec    *pspec,
               AwSession     *session)
{
  const AwScienceId science_id = aw_science_view_get_science_id (view);
  aw_session_select_science_async (session, science_id, NULL, NULL);
}

static void
merge_buildings (AwTreeView *view,
                 GList      *list)
{
  GHashTable   *table;
  GtkTreeModel *model;
  GtkTreeIter   iter;

  table = g_hash_table_new (g_str_hash, g_str_equal);

  while (list)
    {
      const char *name = aw_planet_get_name (list->data);
      g_hash_table_insert (table, (gpointer) name, list->data);
      list = list->next;
    }

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (view));

  if (gtk_tree_model_get_iter_first (model, &iter))
    {
      do
        {
          AwPlanet   *p0, *p1;
          const char *name;

          gtk_tree_model_get (model, &iter, 0, &p0, -1);

          if (!p0)
            continue;

          name = aw_planet_get_name (p0);
          p1 = g_hash_table_lookup (table, name);

          if (p1)
            {
              aw_planet_set_building_levels (p0,
                                             AW_ITEM_FARM, aw_planet_get_farm_level (p1),
                                             AW_ITEM_FACTORY, aw_planet_get_factory_level (p1),
                                             AW_ITEM_CYBERNET, aw_planet_get_cybernet_level (p1),
                                             AW_ITEM_LABORATORY, aw_planet_get_laboratory_level (p1),
                                             AW_ITEM_INVALID);

              aw_planet_set_population (p0, aw_planet_get_population (p1));
              aw_planet_set_production_points (p0, aw_planet_get_production_points (p1));
            }

          aw_planet_unref (p0);
        }
      while (gtk_tree_model_iter_next (model, &iter));
    }

  g_hash_table_unref (table);
  gtk_widget_queue_draw (GTK_WIDGET (view));
}

static void
fetch_buildings_cb (GObject      *source,
                    GAsyncResult *result,
                    gpointer      view)
{
  fetch_items (source, result, view,
               aw_session_fetch_buildings_finish,
               merge_buildings);
}

static void
fetch_planets_cb (GObject      *source,
                  GAsyncResult *result,
                  gpointer      view)
{
  fetch_items (source, result, view,
               aw_session_fetch_planets_finish,
               aw_tree_view_append_items);

  aw_session_fetch_buildings_async (AW_SESSION (source),
                                    fetch_buildings_cb, view);
}

static gboolean
launch_fleet_cb (AwPopupDialog *dialog,
                 gpointer       user_data)
{
  AwFleet   *fleet  = user_data;
  GtkWindow *parent = NULL;

  if (dialog)
    parent = gtk_window_get_transient_for (GTK_WINDOW (dialog));

  hildon_banner_show_informationf ((GtkWidget *) parent,
                                   NULL, "Launch fleet at %s",
                                   aw_fleet_get_planet_name (fleet));

  return FALSE;
}

static gboolean
show_system_cb (AwPopupDialog *dialog,
                gpointer       user_data)
{
  AwFleet *fleet  = user_data;

  show_star_system (aw_session_get_singleton (),
                    aw_fleet_get_system_id (fleet),
                    aw_fleet_get_system_name (fleet),
                    aw_fleet_get_planet_name (fleet));

  return FALSE;
}

static void
fleet_activated_cb (GtkWidget *view,
                    AwFleet   *fleet,
                    AwSession *session)
{
  GtkWidget *dialog, *parent;
  char      *title;

  if (!aw_fleet_get_arrival_time (fleet))
    {
      title = g_strdup_printf (_("Fleet at %s"),
                               aw_fleet_get_planet_name (fleet));

      parent = gtk_widget_get_ancestor (view, GTK_TYPE_WINDOW);
      dialog = aw_popup_dialog_new ((GtkWindow *) parent, title);
      g_free (title);

      aw_popup_dialog_append (AW_POPUP_DIALOG (dialog),
                              _("Launch Fleet"), NULL, launch_fleet_cb, fleet);
      aw_popup_dialog_append (AW_POPUP_DIALOG (dialog),
                              _("Show System"), NULL, show_system_cb, fleet);

      gtk_widget_show (dialog);
    }
  else
    show_system_cb (NULL, fleet);
}

static void
fetch_fleets_cb (GObject      *source,
                 GAsyncResult *result,
                 gpointer      view)
{
  fetch_items (source, result, view,
               aw_session_fetch_fleets_finish,
               aw_tree_view_append_items);
}

static void
fetch_science_cb (GObject      *source,
                  GAsyncResult *result,
                  gpointer      view)
{
  GError         *error = NULL;
  AwScienceStats  science_stats;
  AwScienceId     science_id;
  GList          *sciences;

  sciences = aw_session_fetch_science_finish (AW_SESSION (source), result,
                                              &science_id, &science_stats,
                                              &error);

  if (error)
    {
      hildon_banner_show_information (view, NULL, error->message);
      g_error_free (error);
    }
  else
    {
      aw_science_view_set_sciences   (view, sciences);
      aw_science_view_set_science_id (view, science_id);
      aw_science_view_set_statistics (view, &science_stats);

      g_signal_connect (view, "notify::science-id",
                        G_CALLBACK (science_id_cb), source);
    }
}

static void
spend_all_response_cb (AwSpendAllDialog *dialog,
                       int               response,
                       GtkWidget        *view)
{
  AwItemType item;

  if (GTK_RESPONSE_OK == response)
    {
      item = aw_spend_all_dialog_get_selected_item (dialog);
      aw_session_spend_all_async (aw_session_get_singleton (),
                                  item, NULL, NULL);
      // FIXME: refresh planet view
    }

  gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
spend_all_cb (GtkWidget *button,
              GtkWidget *view)
{
  GtkWidget *dialog, *parent;
  int        production_points;

  parent = gtk_widget_get_ancestor (view, GTK_TYPE_WINDOW);
  dialog = aw_spend_all_dialog_new ((GtkWindow *) parent);

  production_points = aw_tree_view_accumulate (AW_TREE_VIEW (view),
                                               aw_planet_get_production_points,
                                               NULL);

  aw_spend_all_dialog_set_production_points (AW_SPEND_ALL_DIALOG (dialog),
                                             production_points);

  g_signal_connect (dialog, "response",
                    G_CALLBACK (spend_all_response_cb), view);

  gtk_widget_show (dialog);
}

static void
action_started_cb (GtkWidget        *view,
                   AwMainViewAction  action,
                   AwSession        *session)
{
  switch (action)
    {
    case AW_MAIN_VIEW_ACTION_NEWS:
      view = aw_news_view_new ();
      g_signal_connect (view, "news-activated",
                        G_CALLBACK (news_activated_cb), session);
      aw_session_fetch_news_async (session, 0, fetch_news_cb, view);
      show_view (_("News"), view, NULL);
      break;

    case AW_MAIN_VIEW_ACTION_MAP:
      view = aw_map_view_new ();
      g_signal_connect (view, "star-activated",
                        G_CALLBACK (star_activated_cb), session);
      aw_session_fetch_map_async (session, fetch_map_cb, view);
      show_view (_("Map"), view, NULL);
      break;

    case AW_MAIN_VIEW_ACTION_PLANETS:
      view = aw_planet_view_new ();
      g_signal_connect (view, "planet-activated",
                        G_CALLBACK (planet_activated_cb), session);
      aw_planet_view_set_summary_visible (AW_PLANET_VIEW (view), TRUE);
      aw_session_fetch_planets_async (session, fetch_planets_cb, view);
      show_view (_("Planets"), view,
                 _("Spend all Points"), spend_all_cb, NULL);
      break;

    case AW_MAIN_VIEW_ACTION_SCIENCE:
      view = aw_science_view_new ();
      aw_session_fetch_science_async (session, fetch_science_cb, view);
      show_view (_("Science"), view, NULL);
      break;

    case AW_MAIN_VIEW_ACTION_FLEET:
      view = aw_fleet_view_new ();
      g_signal_connect (view, "fleet-activated",
                        G_CALLBACK (fleet_activated_cb), session);
      aw_session_fetch_fleets_async (session, fetch_fleets_cb, view);
      show_view (_("Fleet"), view, NULL);
      break;

    default:
      hildon_banner_show_informationf (GTK_WIDGET (view),
                                       NULL, "Action started: %s",
                                       aw_main_view_action_get_nick (action));
      break;
    }
}

static void
fetch_profile_cb (GObject      *source,
                  GAsyncResult *result,
                  gpointer      user_data)
{
  AwMainView *view = AW_MAIN_VIEW (user_data);
  AwSession  *session = AW_SESSION (source);
  AwProfile  *profile, *self;
  GError     *error = NULL;

  profile = aw_session_fetch_profile_finish (session, result, &error);
  g_print ("%s: profile=%p\n", G_STRFUNC, profile);

  if (error)
    {
      hildon_banner_show_information (GTK_WIDGET (view), NULL, error->message);
      g_error_free (error);
    }
  else
    {
      self = aw_profile_get_self ();
      aw_profile_reset (self, profile);
      aw_main_view_set_profile (view, self);
      g_object_unref (self);
    }
}

static void
initial_fetch_news_cb (GObject      *source,
                       GAsyncResult *result,
                       gpointer      user_data)
{
  AwMainView *view = AW_MAIN_VIEW (user_data);
  AwSession  *session = AW_SESSION (source);
  GError     *error = NULL;
  GList      *news;

  news = aw_session_fetch_news_finish (session, result, &error);

  if (error)
    {
      hildon_banner_show_information (GTK_WIDGET (view), NULL, error->message);
      g_error_free (error);
    }
  else
    {
      if (news)
        aw_main_view_set_latest_news (view, news->data);

      aw_session_fetch_profile_async (session, 0, fetch_profile_cb, user_data);
    }
}

int
main (int    argc,
      char **argv)
{
  GtkWidget  *window;
  AwSettings *settings;
  AwSession  *session;
  AwProfile  *profile;

#if ENABLE_I18N
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
#endif

  g_thread_init (NULL);
  hildon_gtk_init (&argc, &argv);

  settings = aw_settings_new ();
  session  = aw_session_new ();
  profile  = aw_profile_get_self ();

  window = hildon_stackable_window_new ();
  gtk_window_set_title (GTK_WINDOW (window), _("Astro Wars"));
  gtk_widget_show (window);

  main_view = aw_main_view_new ();
  gtk_container_add (GTK_CONTAINER (window), main_view);
  gtk_widget_show (main_view);

  aw_session_fetch_news_async (session, 0, initial_fetch_news_cb, main_view);

  g_object_add_weak_pointer (G_OBJECT (settings), (gpointer) &settings);
  g_object_add_weak_pointer (G_OBJECT (session),  (gpointer) &session);
  g_object_add_weak_pointer (G_OBJECT (profile),  (gpointer) &profile);
  g_object_add_weak_pointer (G_OBJECT (window),   (gpointer) &window);

  g_signal_connect_swapped (session, "login-required",
                            G_CALLBACK (aw_main_login), window);
  g_signal_connect_swapped (session, "captcha-required",
                            G_CALLBACK (aw_main_solve_captcha), window);

  g_signal_connect (window, "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);

  g_signal_connect (main_view, "action-started",
                    G_CALLBACK (action_started_cb), session);

  gtk_main ();

  g_warn_if_fail (NULL == window);

  g_object_unref (session);
  g_warn_if_fail (NULL == session);

  if (session)
    g_object_run_dispose (G_OBJECT (session));

  g_object_unref (profile);
  g_warn_if_fail (NULL == profile);

  if (profile)
    g_object_run_dispose (G_OBJECT (profile));

  g_object_unref (settings);
  g_warn_if_fail (NULL == settings);

  if (settings)
    g_object_run_dispose (G_OBJECT (settings));

  return 0;
}
