#include <libawui/hildon.h>
#include <libawui/web.h>

enum {
  COLUMM_TITLE,
  COLUMM_CALLBACK,
  COLUMM_USER_DATA,
  COLUMM_TEXT,
  COLUMM_ATTRIBUTES,
  LAST_COLUMN,
};

typedef void (*AwDemoCallback) (GtkWindow  *parent,
                                const char *title,
                                gpointer    user_data);

static HildonAppMenu *
ensure_app_menu (HildonWindow *window)
{
  HildonAppMenu *menu;

  if (!(menu = hildon_window_get_app_menu (window)))
    {
      menu = HILDON_APP_MENU (hildon_app_menu_new ());
      hildon_window_set_app_menu (window, menu);
    }

  return menu;
}

/* ================================================================= */

static void
empty_text_cb (GtkWindow  *parent,
               const char *title,
               gpointer    user_data)
{
  GtkWidget    *window, *hbox, *widget;
  GtkListStore *store;

  window = aw_window_new (title);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), hbox);

  widget = aw_tree_view_new (HILDON_UI_MODE_NORMAL);
  gtk_box_pack_start (GTK_BOX (hbox), widget, TRUE, TRUE, 0);

  widget = gtk_vseparator_new ();
  gtk_box_pack_start (GTK_BOX (hbox), widget, FALSE, FALSE, 0);

  store = gtk_list_store_new (1, G_TYPE_STRING);
  widget = aw_tree_view_new_with_model (HILDON_UI_MODE_NORMAL,
                                        GTK_TREE_MODEL (store));
  gtk_box_pack_start (GTK_BOX (hbox), widget, TRUE, TRUE, 0);
  aw_tree_view_set_empty_text (AW_TREE_VIEW (widget), "(No Kittens)");
  g_object_unref (store);

  gtk_widget_show_all (window);
}

/* ================================================================= */

static void
action_started_cb (AwMainView       *window,
                   AwMainViewAction  action)
{
  hildon_banner_show_informationf (GTK_WIDGET (window),
                                   NULL, "Action started: %s",
                                   aw_main_view_action_get_nick (action));
}

static void
main_window_cb (GtkWindow  *parent,
                const char *title,
                gpointer    user_data)
{
  GtkWidget *window, *view;

  window = aw_window_new (title);

  view = aw_main_view_new ();
  gtk_container_add (GTK_CONTAINER (window), view);

  g_signal_connect (view, "action-started",
                    G_CALLBACK (action_started_cb), NULL);

  gtk_widget_show_all (window);
}

/* ================================================================= */

static void
news_activated_cb (AwNewsView *view,
                   AwNews     *news)
{
  char *markup, *link, **links;

  links = aw_news_get_links (news);

  if (links)
    {
      link = g_markup_escape_text (links[0], -1);

      markup = g_strdup_printf ("%s\n<small>&lt;%s&gt;</small>",
                                aw_news_get_text (news), link);

      g_free (link);
    }
  else
    markup = g_strdup (aw_news_get_text (news));

  hildon_banner_show_information_with_markup (GTK_WIDGET (view),
                                              NULL, markup);

  g_free (markup);
}

static void
news_view_cb (GtkWindow  *parent,
              const char *title,
              gpointer    user_data)
{
  GtkWidget *window, *view, *button;
  char      *data = NULL;
  gsize      length;
  GList     *news;

  window = aw_window_new (title);

  view = aw_news_view_new ();
  gtk_container_add (GTK_CONTAINER (window), view);
  g_signal_connect (view, "news-activated", G_CALLBACK (news_activated_cb), NULL);

  if (g_file_get_contents ("tests/data/news.html", &data, &length, NULL))
    {
      news = aw_parser_read_news (data, length, NULL);
      aw_tree_view_append_items (AW_TREE_VIEW (view), news);
      aw_news_list_free (news);
      g_free (data);
    }

  hildon_app_menu_append (ensure_app_menu (HILDON_WINDOW (window)),
                          GTK_BUTTON (button = gtk_button_new_with_label ("Clear News")));
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_list_store_clear),
                            gtk_tree_view_get_model (GTK_TREE_VIEW (view)));
  gtk_widget_show (button);

  gtk_widget_show_all (window);
}

/* ================================================================= */

static void
star_activated_cb (AwMapView *view,
                   AwStar    *star)
{
  char *markup;

  markup = g_markup_printf_escaped ("<b>%s</b> (%d/%d)\n"
                                    "<small>ID: %d, Level: %d</small>",
                                    aw_star_get_name  (star),
                                    aw_star_get_x     (star),
                                    aw_star_get_y     (star),
                                    aw_star_get_id    (star),
                                    aw_star_get_level (star));

  hildon_banner_show_information_with_markup (GTK_WIDGET (view),
                                              NULL, markup);

  g_free (markup);
}

static void
map_view_cb (GtkWindow  *parent,
             const char *title,
             gpointer    user_data)
{
  GtkWidget     *window, *view, *button;
  char          *data = NULL;
  gsize          length;
  GList         *stars;

  window = aw_window_new (title);

  view = aw_map_view_new ();
  gtk_container_add (GTK_CONTAINER (window), view);
  g_signal_connect (view, "star-activated", G_CALLBACK (star_activated_cb), NULL);

  if (g_file_get_contents ("tests/data/map.html", &data, &length, NULL))
    {
      stars = aw_parser_read_map (data, length, NULL);
      aw_tree_view_append_items (AW_TREE_VIEW (view), stars);
      aw_map_view_set_origin (AW_MAP_VIEW (view), g_list_nth_data (stars, 2));
      aw_star_list_free (stars);
      g_free (data);
    }

  hildon_app_menu_append (ensure_app_menu (HILDON_WINDOW (window)),
                          GTK_BUTTON (button = gtk_button_new_with_label ("Clear Map")));
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_list_store_clear),
                            gtk_tree_view_get_model (GTK_TREE_VIEW (view)));
  gtk_widget_show (button);

  gtk_widget_show_all (window);
}

/* ================================================================= */

static void
planet_activated_cb (AwMapView *view,
                     AwPlanet  *planet)
{
  hildon_banner_show_information (GTK_WIDGET (view), NULL,
                                  aw_planet_get_name (planet));
}

static void
system_view_cb (GtkWindow  *parent,
                const char *title,
                gpointer    user_data)
{
  GtkWidget     *window, *view, *button;
  char          *data = NULL;
  gsize          length;
  GList         *planets;

  window = aw_window_new (title);

  view = aw_system_view_new ();
  gtk_container_add (GTK_CONTAINER (window), view);
  g_signal_connect (view, "planet-activated",
                    G_CALLBACK (planet_activated_cb), NULL);

  if (g_file_get_contents ("tests/data/system.html", &data, &length, NULL))
    {
      planets = aw_parser_read_system (data, length, NULL);
      aw_tree_view_append_items (AW_TREE_VIEW (view), planets);
      aw_planet_list_free (planets);
      g_free (data);
    }

  hildon_app_menu_append (ensure_app_menu (HILDON_WINDOW (window)),
                          GTK_BUTTON (button = gtk_button_new_with_label ("Clear Planets")));
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_list_store_clear),
                            gtk_tree_view_get_model (GTK_TREE_VIEW (view)));
  gtk_widget_show (button);

  gtk_widget_show_all (window);
}

/* ================================================================= */

static void
update_buildings (GList *planets,
                  GList *buildings)
{
  while (planets && buildings)
    {
      aw_planet_set_buildings (planets->data,
                               AW_BUILDING_FARM, aw_planet_get_farm (buildings->data),
                               AW_BUILDING_FACTORY, aw_planet_get_factory (buildings->data),
                               AW_BUILDING_CYBERNET, aw_planet_get_cybernet (buildings->data),
                               AW_BUILDING_LABORATORY, aw_planet_get_laboratory (buildings->data),
                               AW_BUILDING_INVALID);

      aw_planet_set_production_points (planets->data, aw_planet_get_production_points (buildings->data));
      aw_planet_set_population (planets->data, aw_planet_get_population (buildings->data));

      buildings = buildings->next;
      planets = planets->next;
    }
}

static void
planet_view_cb (GtkWindow  *parent,
                const char *title,
                gpointer    user_data)
{
  GList     *planets = NULL, *buildings = NULL;
  GtkWidget *window, *view, *button;
  char      *data = NULL;
  gsize      length;

  window = aw_window_new (title);

  view = aw_planet_view_new ();
  gtk_container_add (GTK_CONTAINER (window), view);
  g_signal_connect (view, "planet-activated",
                    G_CALLBACK (planet_activated_cb), NULL);

  if (g_file_get_contents ("tests/data/planets.html", &data, &length, NULL))
    {
      planets = aw_parser_read_planets (data, length, NULL);
      aw_tree_view_append_items (AW_TREE_VIEW (view), planets);
      g_free (data);
    }

  if (g_file_get_contents ("tests/data/buildings.html", &data, &length, NULL))
    {
      buildings = aw_parser_read_buildings (data, length, NULL);
      update_buildings (planets, buildings);
      g_free (data);
    }

  aw_planet_list_free (buildings);
  aw_planet_list_free (planets);

  hildon_app_menu_append (ensure_app_menu (HILDON_WINDOW (window)),
                          GTK_BUTTON (button = gtk_button_new_with_label ("Clear Planets")));
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_list_store_clear),
                            gtk_tree_view_get_model (GTK_TREE_VIEW (view)));
  gtk_widget_show (button);

  gtk_widget_show_all (window);
}

/* ================================================================= */

static void
fleet_activated_cb (AwMapView *view,
                    AwFleet   *fleet)
{
  hildon_banner_show_informationf (GTK_WIDGET (view), NULL, "Fleet at: %s",
                                   aw_fleet_get_planet_name (fleet));
}

static void
fleets_view_cb (GtkWindow  *parent,
                const char *title,
                gpointer    user_data)
{
  GList     *fleets = NULL;
  GtkWidget *window, *view, *button;
  char      *data = NULL;
  gsize      length;

  window = aw_window_new (title);

  view = aw_fleet_view_new ();
  gtk_container_add (GTK_CONTAINER (window), view);
  g_signal_connect (view, "fleet-activated",
                    G_CALLBACK (fleet_activated_cb), NULL);

  if (g_file_get_contents ("tests/data/fleet2.html", &data, &length, NULL))
    {
      fleets = aw_parser_read_fleets (data, length, NULL);
      aw_tree_view_append_items (AW_TREE_VIEW (view), fleets);
      g_free (data);
    }

  aw_fleet_list_free (fleets);

  hildon_app_menu_append (ensure_app_menu (HILDON_WINDOW (window)),
                          GTK_BUTTON (button = gtk_button_new_with_label ("Clear Fleets")));
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_list_store_clear),
                            gtk_tree_view_get_model (GTK_TREE_VIEW (view)));
  gtk_widget_show (button);

  gtk_widget_show_all (window);
}

/* ================================================================= */

static void
notify_science_id_cb (AwScienceView *view)
{
  const AwScience *science;
  const char      *name;

  science = aw_science_view_get_science (view);
  name = aw_science_get_name (science);

  hildon_banner_show_information (GTK_WIDGET (view), NULL, name);
}

static void
science_view_cb (GtkWindow  *parent,
                 const char *title,
                 gpointer    user_data)
{
  GtkWidget      *window, *view;
  char           *data = NULL;
  gsize           length;
  GList          *sciences;
  AwScienceStats  statistics;
  AwScienceId     current_science;

  window = aw_window_new (title);

  view = aw_science_view_new ();
  gtk_container_add (GTK_CONTAINER (window), view);

  if (g_file_get_contents ("tests/data/science.html", &data, &length, NULL))
    {
      sciences = aw_parser_read_sciences (data, length,
                                          &current_science,
                                          &statistics, NULL);

      aw_science_view_set_sciences   (AW_SCIENCE_VIEW (view), sciences);
      aw_science_view_set_statistics (AW_SCIENCE_VIEW (view), &statistics);
      aw_science_view_set_science_id (AW_SCIENCE_VIEW (view), current_science);

      aw_science_list_free (sciences);
      g_free (data);
    }

  g_signal_connect (view, "notify::science-id",
                    G_CALLBACK (notify_science_id_cb), NULL);

  gtk_widget_show_all (window);
}

/* ================================================================= */

static void
login_dialog_cb (GtkWindow  *parent,
                 const char *title,
                 gpointer    user_data)
{
  const char *username;
  const char *password;
  GtkWidget  *dialog;

  dialog = aw_login_dialog_new (parent);

  if (GTK_RESPONSE_OK == gtk_dialog_run (GTK_DIALOG (dialog)))
    {
      username = aw_login_dialog_get_username (AW_LOGIN_DIALOG (dialog));
      password = aw_login_dialog_get_password (AW_LOGIN_DIALOG (dialog));

      hildon_banner_show_informationf (GTK_WIDGET (parent), NULL,
                                       "Username: '%s'; password: '%s'",
                                       username, password);
    }

  gtk_widget_destroy (dialog);
}

static void
captcha_dialog_cb (GtkWindow  *parent,
                   const char *title,
                   gpointer    user_data)
{
  GdkPixbuf  *challenge;
  const char *response;
  GtkWidget  *dialog;

  dialog = aw_captcha_dialog_new (parent);

  challenge = gdk_pixbuf_new_from_file ("tests/data/captcha.png", NULL);

  if (challenge)
    {
      aw_captcha_dialog_set_challenge (AW_CAPTCHA_DIALOG (dialog), challenge);
      g_object_unref (challenge);
    }

  if (GTK_RESPONSE_OK == gtk_dialog_run (GTK_DIALOG (dialog)))
    {
      response = aw_captcha_dialog_get_response (AW_CAPTCHA_DIALOG (dialog));
      hildon_banner_show_informationf (GTK_WIDGET (parent), NULL,
                                       "Response: '%s'", response);
    }

  gtk_widget_destroy (dialog);
}

/* ================================================================= */

static void
register_demo (GtkListStore   *store,
               const char     *title,
               const char     *details,
               AwDemoCallback  callback,
               gpointer        user_data)
{
  GString        *text       = g_string_new (NULL);
  PangoAttrList  *attributes = pango_attr_list_new ();
  PangoAttribute *attr;
  GtkTreeIter     iter;

  g_string_append (text, title);
  g_string_append (text, "\n");

  attr = pango_attr_scale_new (PANGO_SCALE_SMALL);
  attr->start_index = text->len;

  pango_attr_list_insert (attributes, attr);

  /* FIXME */
  attr = pango_attr_foreground_new (0x9999, 0x9999, 0x9999);
  attr->start_index = text->len;

  pango_attr_list_insert (attributes, attr);

  g_string_append (text, details);

  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                      COLUMM_TITLE, title, COLUMM_CALLBACK, callback,
                      COLUMM_USER_DATA, user_data, COLUMM_TEXT, text->str,
                      COLUMM_ATTRIBUTES, attributes, -1);

  pango_attr_list_unref (attributes);
  g_string_free (text, TRUE);
}

static void
register_group (GtkListStore *store,
                const char   *title)
{
  GtkTreeIter iter;
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter, COLUMM_TITLE, title, -1);
}

static void
row_tapped_cb (GtkTreeView *view,
               GtkTreePath *path,
               GtkWindow   *parent)
{
  GtkTreeModel *model;
  GtkTreeIter   iter;

  model = gtk_tree_view_get_model (view);

  if (gtk_tree_model_get_iter (model, &iter, path))
    {
      AwDemoCallback  callback;
      gpointer        user_data;
      char           *title;

      gtk_tree_model_get (model, &iter,
                          COLUMM_TITLE, &title,
                          COLUMM_CALLBACK, &callback,
                          COLUMM_USER_DATA, &user_data, -1);

      callback (parent, title, user_data);

      g_free (title);
    }
}

static gboolean
header_cb (GtkTreeModel  *model,
           GtkTreeIter   *iter,
           char         **title,
           gpointer       data)
{
  AwDemoCallback callback = NULL;

  gtk_tree_model_get (model, iter, COLUMM_CALLBACK, &callback, -1);

  if (callback)
    return FALSE;
  if (title)
    gtk_tree_model_get (model, iter, COLUMM_TITLE, title, -1);

  return TRUE;
}

int
main (int    argc,
      char **argv)
{
  GtkWidget       *window, *view;
  GtkCellRenderer *text_cell;
  AwSettings      *settings;
  GtkListStore    *store;

  g_set_application_name ("AWUI/Hildon Showcase");

  hildon_gtk_init (&argc, &argv);

  settings = aw_settings_new ();

  store = gtk_list_store_new (LAST_COLUMN,
                              G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER,
                              G_TYPE_STRING, PANGO_TYPE_ATTR_LIST);

  register_group (store, "Action Panes");
  register_demo  (store, "AwMainWindow",       "Capturing main window actions",   main_window_cb,    NULL);
  register_demo  (store, "AwScienceView",      "Science control screen",          science_view_cb,   NULL);
#if 0
  register_demo  (store, "AwPlanetDetailView", "Planet control screen",           planet_detail_view_cb,     NULL);
  register_demo  (store, "AwTradeView",        "Trade control screen",            trade_view_cb,     NULL);
  register_demo  (store, "AwAllianceView",     "Alliance overview screen",        alliance_view_cb,  NULL);
#endif

  register_group (store, "Dialogs");
  register_demo  (store, "AwLoginDialog",      "Login Dialog",                    login_dialog_cb,   NULL);
  register_demo  (store, "AwCaptchaDialog",    "Captcha Dialog",                  captcha_dialog_cb, NULL);

  register_group (store, "Tree Views");
  register_demo  (store, "AwTreeView",         "Custom text for empty models",    empty_text_cb,     NULL);
  register_demo  (store, "AwNewsView",         "List of recent news",             news_view_cb,      NULL);
  register_demo  (store, "AwMapView",          "List of known stars",             map_view_cb,       NULL);
  register_demo  (store, "AwSystemView",       "List of planets within a system", system_view_cb,    NULL);
  register_demo  (store, "AwPlanetView",       "List of a player's planets",      planet_view_cb,    NULL);
  register_demo  (store, "AwFleetView",        "List of a player's fleets",       fleets_view_cb,    NULL);

  window = aw_window_new (NULL);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  view = aw_tree_view_new_with_model (HILDON_UI_MODE_NORMAL,
                                      GTK_TREE_MODEL (store));
  gtk_container_add (GTK_CONTAINER (window), view);

  text_cell = gtk_cell_renderer_text_new ();
  g_object_set (text_cell, "xpad", HILDON_MARGIN_DEFAULT, NULL);

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), -1, NULL,
                                               text_cell, "text", COLUMM_TEXT,
                                               "attributes", COLUMM_ATTRIBUTES, NULL);

  hildon_tree_view_set_row_header_func (GTK_TREE_VIEW (view),
                                        header_cb, NULL, NULL);

  g_signal_connect (view, "hildon-row-tapped",
                    G_CALLBACK (row_tapped_cb), window);

  gtk_widget_show_all (window);

  g_object_add_weak_pointer (G_OBJECT (settings), (gpointer) &settings);
  g_object_add_weak_pointer (G_OBJECT (window), (gpointer) &window);
  g_object_add_weak_pointer (G_OBJECT (store), (gpointer) &store);

  gtk_main ();

  g_object_unref (store);
  g_object_unref (settings);

  g_warn_if_fail (NULL == settings);
  g_warn_if_fail (NULL == window);
  g_warn_if_fail (NULL == store);

  return 0;
}
