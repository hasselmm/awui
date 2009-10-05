#include <libawui/model.h>
#include <libawui/web.h>

#define CHECK_ERROR(error)                                                      \
G_STMT_START                                                                    \
{                                                                               \
  GError *err = (error);                                                        \
                                                                                \
  if (err)                                                                      \
    {                                                                           \
      g_error ("%s: %s\n", G_STRFUNC, err->message);                            \
      g_error_free (err);                                                       \
    }                                                                           \
}                                                                               \
G_STMT_END

#define CHECK_NEWS(time, date, text, link, flags)                               \
G_STMT_START                                                                    \
{                                                                               \
  AwNews  *item;                                                                \
  char   **links;                                                               \
                                                                                \
  item  = list->data;                                                           \
  list  = g_list_delete_link (list, list);                                      \
  links = aw_news_get_links (item);                                             \
                                                                                \
  g_assert         (NULL != item);                                              \
  g_assert_cmpstr  (aw_news_get_time  (item), ==, (time));                      \
  g_assert_cmpstr  (aw_news_get_date  (item), ==, (date));                      \
  g_assert_cmpstr  (aw_news_get_text  (item), ==, (text));                      \
  g_assert_cmpuint (aw_news_get_flags (item), ==, (flags));                     \
                                                                                \
  if (link)                                                                     \
    {                                                                           \
      g_assert (NULL != links);                                                 \
      g_assert_cmpint (g_strv_length (links), ==, 1);                           \
      g_assert_cmpstr (links[0],              ==, (link));                      \
    }                                                                           \
  else                                                                          \
    g_assert (NULL == links);                                                   \
                                                                                \
  aw_news_unref (item);                                                         \
}                                                                               \
G_STMT_END

#define CHECK_STAR(id, name, level, x, y)                                       \
G_STMT_START                                                                    \
{                                                                               \
  AwStar *item;                                                                 \
                                                                                \
  item = list->data;                                                            \
  list = g_list_delete_link (list, list);                                       \
                                                                                \
  g_assert        (NULL != item);                                               \
  g_assert_cmpint (aw_star_get_id    (item), ==, (id));                         \
  g_assert_cmpstr (aw_star_get_name  (item), ==, (name));                       \
  g_assert_cmpint (aw_star_get_level (item), ==, (level));                      \
  g_assert_cmpint (aw_star_get_x     (item), ==, (x));                          \
  g_assert_cmpint (aw_star_get_y     (item), ==, (y));                          \
                                                                                \
  aw_star_unref (item);                                                         \
}                                                                               \
G_STMT_END

#define CHECK_BUILDINGS(name, owner, pop, pp, farm, fab, net, lab, sb, flags)   \
G_STMT_START                                                                    \
{                                                                               \
  AwPlanet *item;                                                               \
                                                                                \
  item = list->data;                                                            \
  list = g_list_delete_link (list, list);                                       \
                                                                                \
  g_assert         (NULL != item);                                              \
  g_assert_cmpstr  (aw_planet_get_name              (item), ==, (name));        \
  g_assert_cmpstr  (aw_planet_get_owner             (item), ==, (owner));       \
  g_assert_cmpint  (aw_planet_get_population        (item), ==, (pop));         \
  g_assert_cmpint  (aw_planet_get_production_points (item), ==, (pp));          \
  g_assert_cmpint  (aw_planet_get_farm_level        (item), ==, (farm));        \
  g_assert_cmpint  (aw_planet_get_factory_level     (item), ==, (fab));         \
  g_assert_cmpint  (aw_planet_get_cybernet_level    (item), ==, (net));         \
  g_assert_cmpint  (aw_planet_get_laboratory_level  (item), ==, (lab));         \
  g_assert_cmpint  (aw_planet_get_starbase_level    (item), ==, (sb));          \
  g_assert_cmpuint (aw_planet_get_flags             (item), ==, (flags));       \
                                                                                \
  aw_planet_unref (item);                                                       \
}                                                                               \
G_STMT_END

#define CHECK_SCIENCE(id, level, remaining)                                     \
G_STMT_START                                                                    \
{                                                                               \
  AwScience *item;                                                              \
                                                                                \
  item = list->data;                                                            \
  list = g_list_delete_link (list, list);                                       \
                                                                                \
  g_assert         (NULL != item);                                              \
  g_assert_cmpuint (aw_science_get_id        (item), ==, (id));                 \
  g_assert_cmpint  (aw_science_get_level     (item), ==, (level));              \
  g_assert_cmpint  (aw_science_get_remaining (item), ==, (remaining));          \
                                                                                \
  aw_science_unref (item);                                                      \
}                                                                               \
G_STMT_END

#define CHECK_FLEET(time, date, sid, system, pid, tr, co, ds, cs, bs, flags)    \
G_STMT_START                                                                    \
{                                                                               \
  AwFleet *item;                                                                \
  char    *planet;                                                              \
                                                                                \
  item   = list->data;                                                          \
  list   = g_list_delete_link (list, list);                                     \
  planet = g_strdup_printf ("%s %d", (system), (pid));                          \
                                                                                \
  g_assert         (NULL != item);                                              \
  g_assert_cmpstr  (aw_fleet_get_arrival_time (item), ==, (time));              \
  g_assert_cmpstr  (aw_fleet_get_arrival_date (item), ==, (date));              \
  g_assert_cmpstr  (aw_fleet_get_planet_name  (item), ==, (planet));            \
  g_assert_cmpstr  (aw_fleet_get_system_name  (item), ==, (system));            \
  g_assert_cmpint  (aw_fleet_get_system_id    (item), ==, (sid));               \
  g_assert_cmpint  (aw_fleet_get_transports   (item), ==, (tr));                \
  g_assert_cmpint  (aw_fleet_get_colonizers   (item), ==, (co));                \
  g_assert_cmpint  (aw_fleet_get_destroyers   (item), ==, (ds));                \
  g_assert_cmpint  (aw_fleet_get_cruisers     (item), ==, (cs));                \
  g_assert_cmpint  (aw_fleet_get_battleships  (item), ==, (bs));                \
  g_assert_cmpuint (aw_fleet_get_flags        (item), ==, (flags));             \
                                                                                \
  aw_fleet_unref (item);                                                        \
  g_free (planet);                                                              \
}                                                                               \
G_STMT_END

static void
test_parse_news (void)
{
  GList  *list  = NULL;
  GError *error = NULL;
  char   *data  = NULL;
  gsize   length;

  if (g_file_get_contents ("tests/data/news.html", &data, &length, &error))
    list = aw_parser_read_news (data, length, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_assert_cmpint (5, ==, g_list_length (list));

  CHECK_NEWS ("22:01:54", "Sep 24",
              "<b>warning:</b> 2 failed logins",
              "/about/glossary/id/3/0/", AW_NEWS_IMPORTANT);
  CHECK_NEWS ("20:53:59", "Sep 24",
              "<b>warning:</b> 1 failed logins",
              "/about/glossary/id/3/0/", AW_NEWS_DEFAULT);
  CHECK_NEWS ("15:48:20", "Sep 24",
              "<b>warning:</b> 1 failed logins",
              "/about/glossary/id/3/0/", AW_NEWS_DEFAULT);
  CHECK_NEWS ("15:14:14", "Sep 24",
              "<b>warning:</b> 1 failed logins",
              "/about/glossary/id/3/0/", AW_NEWS_DEFAULT);
  CHECK_NEWS ("10:21:38", "Sep 24",
              "We just colonized Alpha Alya 3!\nCongratulations!",
              NULL, AW_NEWS_DEFAULT);

  g_assert (NULL == list);
}

static void
test_parse_map (void)
{
  GList  *list  = NULL;
  GError *error = NULL;
  char   *data  = NULL;
  gsize   length;

  if (g_file_get_contents ("tests/data/map.html", &data, &length, &error))
    list = aw_parser_read_map (data, length, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_assert_cmpint (7, ==, g_list_length (list));

  CHECK_STAR (1216, "Alpha Rukbat al Rami",     0, -52, -24);
  CHECK_STAR (954,  "Alpha Mirza (UMa)",        5, -46, -25);
  CHECK_STAR (1081, "Alpha Alya",               3, -48, -25);
  CHECK_STAR (1082, "Alpha Vindemiatrix",       2, -48, -26);
  CHECK_STAR (1217, "Alpha Al Minliar al Asad", 0, -50, -26);
  CHECK_STAR (955,  "Alpha Alaraph",            1, -46, -28);
  CHECK_STAR (1218, "Alpha Homan",              0, -51, -29);

  g_assert (NULL == list);
}

static void
test_parse_system (void)
{
  GList  *list  = NULL;
  GError *error = NULL;
  char   *data  = NULL;
  gsize   length;

  if (g_file_get_contents ("tests/data/system.html", &data, &length, &error))
    list = aw_parser_read_system (data, length, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_assert_cmpint (12, ==, g_list_length (list));

  CHECK_BUILDINGS ("Alpha Alya 1",  "tbf",     4, -1, -1, -1, -1, -1, 2, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 2",  "tbf",     5, -1, -1, -1, -1, -1, 6, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 3",  "tbf",     4, -1, -1, -1, -1, -1, 2, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 4",  "tbf",     3, -1, -1, -1, -1, -1, 1, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 5",  "Senaeke", 3, -1, -1, -1, -1, -1, 0, AW_PLANET_SIEGED);
  CHECK_BUILDINGS ("Alpha Alya 6",   NULL,     0, -1, -1, -1, -1, -1, 0, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 7",   NULL,     0, -1, -1, -1, -1, -1, 0, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 8",  "mostor5", 3, -1, -1, -1, -1, -1, 1, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 9",   NULL,     0, -1, -1, -1, -1, -1, 0, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 10",  NULL,     0, -1, -1, -1, -1, -1, 0, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 11", "bogdans", 3, -1, -1, -1, -1, -1, 1, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 12",  NULL,     0, -1, -1, -1, -1, -1, 0, AW_PLANET_DEFAULT);

  g_assert (NULL == list);
}

static void
test_parse_planets (void)
{
  GList      *list  = NULL;
  GError     *error = NULL;
  char       *data  = NULL;
  gsize       length;
  char       *username;

  if (g_file_get_contents ("tests/data/planets.html", &data, &length, &error))
    list = aw_parser_read_planets (data, length, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_object_get (aw_settings_get_singleton (),
                "username", &username, NULL);

  g_assert_cmpint (4, ==, g_list_length (list));

  CHECK_BUILDINGS ("Alpha Alya 2", username, 5, 8, 8, 9, -1, -1, -1, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 1", username, 4, 6, 6, 6, -1, -1, -1, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 3", username, 4, 6, 6, 6, -1, -1, -1, AW_PLANET_SIEGED);
  CHECK_BUILDINGS ("Alpha Alya 4", username, 2, 3, 2, 4, -1, -1, -1, AW_PLANET_DEFAULT);

  g_assert (NULL == list);
  g_free (username);
}

static void
test_parse_buildings (void)
{
  GList      *list  = NULL;
  GError     *error = NULL;
  char       *data  = NULL;
  gsize       length;
  char       *username;

  if (g_file_get_contents ("tests/data/buildings.html", &data, &length, &error))
    list = aw_parser_read_buildings (data, length, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_object_get (aw_settings_get_singleton (),
                "username", &username, NULL);

  g_assert_cmpint (4, ==, g_list_length (list));

  CHECK_BUILDINGS ("Alpha Alya 2", username, 5, 8, 8, 9, 9, 9, -1, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 1", username, 4, 6, 6, 6, 7, 6, -1, AW_PLANET_DEFAULT);
  CHECK_BUILDINGS ("Alpha Alya 3", username, 4, 6, 6, 6, 7, 6, -1, AW_PLANET_SIEGED);
  CHECK_BUILDINGS ("Alpha Alya 4", username, 2, 3, 2, 4, 3, 0, -1, AW_PLANET_DEFAULT);

  g_assert (NULL == list);
  g_free (username);
}

static void
test_parse_science (void)
{
  GList          *list    = NULL;
  AwScienceId     current = AW_SCIENCE_INVALID;
  AwScienceStats  stats   = { 0, };
  GError         *error   = NULL;
  char           *data    = NULL;
  gsize           length;

  if (g_file_get_contents ("tests/data/science.html", &data, &length, &error))
    list = aw_parser_read_sciences (data, length, &current, &stats, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_assert_cmpint (7, ==, g_list_length (list));

  CHECK_SCIENCE (AW_SCIENCE_BIOLOGY,     10, 501);
  CHECK_SCIENCE (AW_SCIENCE_ECONOMY,      9, 502);
  CHECK_SCIENCE (AW_SCIENCE_ENERGY,       8, 534);
  CHECK_SCIENCE (AW_SCIENCE_MATHEMATICS,  7, 504);
  CHECK_SCIENCE (AW_SCIENCE_PHYSICS,      6, 505);
  CHECK_SCIENCE (AW_SCIENCE_SOCIAL,       5, 506);
  CHECK_SCIENCE (AW_SCIENCE_CULTURE,      4, 427);

  g_assert (NULL == list);

  g_assert_cmpint  (36, ==, stats.science_output);
  g_assert_cmpint  (0,  ==, stats.science_bonus);
  g_assert_cmpint  (26, ==, stats.culture_output);
  g_assert_cmpint  (10, ==, stats.culture_bonus);

  g_assert_cmpuint (AW_SCIENCE_ENERGY, ==, current);
}

static void
test_parse_science2 (void)
{
  GList          *list    = NULL;
  AwScienceId     current = AW_SCIENCE_INVALID;
  AwScienceStats  stats   = { 0, };
  GError         *error   = NULL;
  char           *data    = NULL;
  gsize           length;

  if (g_file_get_contents ("tests/data/science2.html", &data, &length, &error))
    list = aw_parser_read_sciences (data, length, &current, &stats, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_assert_cmpint (7, ==, g_list_length (list));

  CHECK_SCIENCE (AW_SCIENCE_BIOLOGY,      9, 1073);
  CHECK_SCIENCE (AW_SCIENCE_ECONOMY,     10, 1489);
  CHECK_SCIENCE (AW_SCIENCE_ENERGY,      11, 1755);
  CHECK_SCIENCE (AW_SCIENCE_MATHEMATICS, 10, 1410);
  CHECK_SCIENCE (AW_SCIENCE_PHYSICS,      8,  222);
  CHECK_SCIENCE (AW_SCIENCE_SOCIAL,       9, 1164);
  CHECK_SCIENCE (AW_SCIENCE_CULTURE,      6, 1730);

  g_assert (NULL == list);

  g_assert_cmpint  (88, ==, stats.science_output);
  g_assert_cmpint  (0,  ==, stats.science_bonus);
  g_assert_cmpint  (59, ==, stats.culture_output);
  g_assert_cmpint  (10, ==, stats.culture_bonus);

  g_assert_cmpuint (AW_SCIENCE_PHYSICS, ==, current);
}

static void
test_parse_fleet (void)
{
  GList  *list  = NULL;
  GError *error = NULL;
  char   *data  = NULL;
  gsize   length;

  if (g_file_get_contents ("tests/data/fleet.html", &data, &length, &error))
    list = aw_parser_read_fleets (data, length, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_assert_cmpint (8, ==, g_list_length (list));

  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",         11, 5, 0,  0, 0, 0, AW_FLEET_PENDING);
  CHECK_FLEET ("01:48:10", "Oct 02", 1082, "Alpha Vindemiatrix", 12, 0, 0, 10, 0, 0, 0);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",          6, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",          7, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",          9, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",         10, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",         11, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",         12, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);

  g_assert (NULL == list);
}

static void
test_parse_fleet2 (void)
{
  GList  *list  = NULL;
  GError *error = NULL;
  char   *data  = NULL;
  gsize   length;

  if (g_file_get_contents ("tests/data/fleet2.html", &data, &length, &error))
    list = aw_parser_read_fleets (data, length, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_assert_cmpint (8, ==, g_list_length (list));

  CHECK_FLEET ("22:36:39", "Oct 01", 1081, "Alpha Alya",         11, 5, 0,  0, 0, 0, 0);
  CHECK_FLEET ("01:48:10", "Oct 02", 1082, "Alpha Vindemiatrix", 12, 0, 0, 10, 0, 0, 0);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",          6, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",          7, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",          9, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",         10, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",         11, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL,       NULL,     1081, "Alpha Alya",         12, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);

  g_assert (NULL == list);
}

static void
test_parse_fleet3 (void)
{
  GList  *list  = NULL;
  GError *error = NULL;
  char   *data  = NULL;
  gsize   length;

  if (g_file_get_contents ("tests/data/fleet3.html", &data, &length, &error))
    list = aw_parser_read_fleets (data, length, &error);

  g_free (data);

  CHECK_ERROR (error);

  g_assert_cmpint (8, ==, g_list_length (list));

  CHECK_FLEET (NULL, NULL, 1081, "Alpha Alya",          6, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL, NULL, 1082, "Alpha Vindemiatrix", 10, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL, NULL, 1082, "Alpha Vindemiatrix", 11, 3, 0, 17, 0, 0, 0);
  CHECK_FLEET (NULL, NULL, 1082, "Alpha Vindemiatrix", 12, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL, NULL, 1081, "Alpha Alya",          7, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL, NULL, 1081, "Alpha Alya",          9, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL, NULL, 1081, "Alpha Alya",         10, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);
  CHECK_FLEET (NULL, NULL, 1081, "Alpha Alya",         12, 0, 0,  1, 0, 0, AW_FLEET_SIEGING);

  g_assert (NULL == list);
}

int
main (int    argc,
      char **argv)
{
  AwSettings *settings;
  int         result;

  g_test_init (&argc, &argv, NULL);
  g_type_init ();

  g_test_add_func ("/parser/news",      test_parse_news);
  g_test_add_func ("/parser/map",       test_parse_map);
  g_test_add_func ("/parser/system",    test_parse_system);
  g_test_add_func ("/parser/planets",   test_parse_planets);
  g_test_add_func ("/parser/buildings", test_parse_buildings);
  g_test_add_func ("/parser/science/1", test_parse_science);
  g_test_add_func ("/parser/science/2", test_parse_science2);
  g_test_add_func ("/parser/fleet/1",   test_parse_fleet);
  g_test_add_func ("/parser/fleet/2",   test_parse_fleet2);
  g_test_add_func ("/parser/fleet/3",   test_parse_fleet3);

  settings = aw_settings_new ();
  g_object_add_weak_pointer (G_OBJECT (settings), (gpointer) &settings);

  result = g_test_run ();

  g_object_unref (settings);
  g_assert (NULL == settings);

  return result;
}

