#include "config.h"
#include "parser.h"

#include <libawui/model.h>

#include <stdio.h>
#include <string.h>

gboolean
aw_parser_find_table (const char  *data,
                      gssize       length,
                      const char  *title,
                      int         *start,
                      int         *end,
                      GError     **error)
{
  gboolean    success = FALSE;
  char       *pattern, *safe_title;
  GRegex     *regex   = NULL;
  GMatchInfo *info    = NULL;

  if (-1 == length)
    length = strlen (data);

  safe_title = g_regex_escape_string (title, -1);

  pattern = g_strconcat ("<table\\b.*?"
                         "(?:<tr\\b.*?#202060.*?<td\\b.*?>|"
                            "<td\\b.*?#202060.*?>)"
                         "(?:<small>|<b>)*",
                         safe_title,
                         "(?:</b>|</small>)*"
                         "</td>.*?</tr>(.*?)</table>", NULL);

  regex = g_regex_new (pattern, G_REGEX_CASELESS | G_REGEX_DOTALL, 0, error);

  if (!regex ||
      !g_regex_match_full (regex, data, length, 0, 0, &info, error) ||
      !g_match_info_fetch_pos (info, 1, start, end))
    goto out;

  success = TRUE;

out:
  if (info)
    g_match_info_free (info);
  if (regex)
    g_regex_unref (regex);

  g_free (safe_title);
  g_free (pattern);

  return success;
}

GList *
aw_parser_read_table (const char  *data,
                      gssize       length,
                      int          columns,
                      GError     **error)
{
  GString    *pattern = g_string_new (NULL);
  GRegex     *regex = NULL;
  GMatchInfo *info = NULL;
  GList      *rows = NULL;
  int         i;

  if (-1 == length)
    length = strlen (data);

  g_string_append (pattern, "<tr\\b(?:[^>]*?bgcolor=.?(#[0-9a-f]{6}))?[^>]*?>[^<>]*?");

  for (i = 0; i < columns; ++i)
    g_string_append (pattern, "<td\\b(?:[^>]*?bgcolor=.?(#[0-9a-f]{6}))?[^>]*?>((?:.(?!<tr))*?)</td>[^<>]*?");

  g_string_append (pattern, "</tr[^>]*>");

  regex = g_regex_new (pattern->str, G_REGEX_CASELESS | G_REGEX_DOTALL, 0, error);

  if (!regex ||
      !g_regex_match_full (regex, data, length, 0, 0, &info, error))
    goto out;

  do
    {
      char **cells = g_new (char *, columns * 2 + 2);

      for (i = 0; i < columns * 2 + 1; ++i)
        cells[i] = g_match_info_fetch (info, i + 1);

      cells[i] = NULL;

      rows = g_list_prepend (rows, cells);
    }
  while (g_match_info_next (info, error));

out:
  if (info)
    g_match_info_free (info);
  if (regex)
    g_regex_unref (regex);

  g_string_free (pattern, TRUE);
  return g_list_reverse (rows);
}

static gboolean
aw_parser_reduce_markup_cb (const GMatchInfo *info,
                            GString          *result,
                            gpointer          user_data)
{
  char *text = g_match_info_fetch (info, 0);

  if (!g_ascii_strcasecmp (text, "<b>") ||
      !g_ascii_strcasecmp (text, "</b>"))
    {
      g_string_append (result, text);
      goto out;
    }

  if (!g_ascii_strncasecmp (text, "<br", 3) && strchr (" />", text[3]))
    {
      g_string_append (result, "\n");
      goto out;
    }

out:
  g_free (text);
  return FALSE;
}

char *
aw_parser_reduce_markup (const char  *data,
                         gssize       length,
                         GError     **error)
{
  GRegex *regex = NULL;
  char   *markup = NULL;


  regex = g_regex_new ("<br[^>]*>\\s*|<[^>]*>", G_REGEX_DOTALL, 0, error);

  if (!regex)
    goto out;

  markup = g_regex_replace_eval (regex, data, length, 0, 0,
                                 aw_parser_reduce_markup_cb, NULL, error);

out:
  if (regex)
    g_regex_unref (regex);

  return markup;
}

char **
aw_parser_find_links (const char  *data,
                      gssize       length,
                      GError     **error)
{
  GRegex     *regex  = NULL;
  GMatchInfo *info   = NULL;
  GPtrArray  *links  = NULL;
  char      **result = NULL;

  if (-1 == length)
    length = strlen (data);

  regex = g_regex_new ("<a[^>]*?href=(?:([\"'])(.*?)\\1|([^>[:space:]]+))",
                       G_REGEX_CASELESS | G_REGEX_DOTALL, 0, error);

  if (!regex ||
      !g_regex_match_full (regex, data, length, 0, 0, &info, error))
    goto out;

  links = g_ptr_array_new ();

  do
    {
      char *link = g_match_info_fetch (info, 2);

      if (link && !link[0])
        link = (g_free (link), NULL);
      if (!link)
        link = g_match_info_fetch (info, 3);
      if (link)
        g_ptr_array_add (links, link);
    }
  while (g_match_info_next (info, error));

  g_ptr_array_add (links, NULL);

out:
  if (info)
    g_match_info_free (info);
  if (regex)
    g_regex_unref (regex);
  if (links)
    result = (char **) g_ptr_array_free (links, FALSE);

  return result;
}

static gboolean
aw_parser_find_element_text (const char  *data,
                             gssize       length,
                             int         *start,
                             int         *end)
{
  const char *label_start, *label_end;

  if (-1 == length)
    length = strlen (data);

  label_start = g_strstr_len (data, length, ">");

  if (!label_start++)
    return FALSE;

  length -= (label_start - data);
  label_end = g_strstr_len (label_start, length, "<");

  if (!label_end)
    return FALSE;

  *start = (label_start - data);
  *end = (label_end - data);

  return TRUE;
}

static char *
aw_parser_read_element_text (const char  *data,
                             gsize        length)
{
  char *label = NULL;
  int   start, end;

  if (aw_parser_find_element_text (data, length, &start, &end))
    label = g_strndup (data + start, end - start);

  return label;
}

static gboolean
aw_parser_read_planet_name (const char  *text,
                            char       **name,
                            int         *id)
{
  const char *end;

  if (!(text = strstr (text, "i=")))
    return FALSE;
  if (1 != sscanf (text, "i=%d", id))
    return FALSE;
  if (!(text = strchr (text, '>')))
    return FALSE;
  if (!(end = strchr (++text, '<')))
    return FALSE;

  *name = g_strndup (text, end - text);

  return TRUE;
}

static char *
aw_parser_read_small_name (const char *text)
{
  const char *end;

  if (!(text = strstr (text, "<small>")))
    return NULL;
  if (!(end = strstr (text += 7, "</small>")))
    return FALSE;

  return g_strndup (text, end - text);
}

static void
aw_parser_list_free (GList  *head,
                     GList  *pivot,
                     void  (*destroy_item)())
{
  while (head != pivot)
    {
      destroy_item (head->data);
      head = g_list_delete_link (head, head);
    }

  while (head)
    {
      g_strfreev (head->data);
      head = g_list_delete_link (head, head);
    }
}

GList *
aw_parser_read_news (const char  *data,
                     gssize       length,
                     GError     **error)
{
  GList *news = NULL, *l;
  int    start, end;

  if (aw_parser_find_table (data, length, "News", &start, &end, error))
    news = aw_parser_read_table (data + start, end - start, 2, error);

  for (l = news; l; l = l->next)
    {
      char        **cells = l->data;
      AwNewsFlags   flags = AW_NEWS_DEFAULT;
      char        **date_time;
      char        **links;
      char         *text;

      if (g_strcmp0 (cells[1], "#404040"))
        flags |= AW_NEWS_IMPORTANT;

      text = aw_parser_reduce_markup (cells[4], -1, NULL);
      links = aw_parser_find_links (cells[4], -1, NULL);
      date_time = g_strsplit (cells[2], " - ", -1);

      l->data = aw_news_new (date_time[1], date_time[0], text, links, flags);

      g_free (text);

      g_strfreev (date_time);
      g_strfreev (links);

      g_strfreev (cells);
    }

  return news;
}

GList *
aw_parser_read_map (const char  *data,
                    gssize       length,
                    GError     **error)
{
  GList      *list = NULL;
  GRegex     *regex = NULL;
  GMatchInfo *info  = NULL;

  regex = g_regex_new ("title=\"(.*?) \\(([+-]?\\d+)/([+-]?\\d+)\\)\\s+?"
                       "ID:(\\d+)\\s+?Level: (\\d+)\"",
                       G_REGEX_CASELESS | G_REGEX_DOTALL, 0, error);

  if (!regex || !g_regex_match_full (regex, data, length, 0, 0, &info, error))
    goto out;

  do
    {
      char  **cells = g_match_info_fetch_all (info);
      int     id, x, y, level;
      AwStar *star;

      if ((1 != sscanf (cells[2], "%d", &x)) ||
          (1 != sscanf (cells[3], "%d", &y)) ||
          (1 != sscanf (cells[4], "%d", &id)) ||
          (1 != sscanf (cells[5], "%d", &level)))
        {
          aw_parser_list_free (list, NULL, aw_planet_unref);
          g_strfreev (cells);
          list = NULL;
          break;
        }

      star = aw_star_new (id, cells[1], level, x, y);
      list = g_list_prepend (list, star);

      g_strfreev (cells);
    }
  while (g_match_info_next (info, error));

out:
  if (info)
    g_match_info_free (info);
  if (regex)
    g_regex_unref (regex);

  return list;
}

GList *
aw_parser_read_system (const char  *data,
                       gssize       length,
                       GError     **error)
{
  GList      *rows = NULL, *l, *n;
  int         start, end;
  char       *star_name;
  const char *tmp;

  if (aw_parser_find_table (data, length, "ID", &start, &end, error))
    rows = aw_parser_read_table (data + start, end - start, 4, error);

  if (!(tmp = g_strstr_len (data, length, "Planets at")) ||
      !(star_name = aw_parser_read_element_text (tmp, length - (tmp - data))))
    return NULL;

  for (l = rows; l && (n = l->next, l); l = n)
    {
      char          **cells = l->data;
      AwPlanetFlags   flags = AW_PLANET_DEFAULT;
      int             population, starbase;
      char           *name, *owner;

      if ((1 != sscanf (cells[4], "%d", &population)) ||
          (1 != sscanf (cells[6], "%d", &starbase)))
        break;

      if (g_strcmp0 (cells[0], "#404040") &&
          g_strcmp0 (cells[0], "#305050"))
        flags |= AW_PLANET_SIEGED;

      name  = g_strconcat (star_name, " ", cells[2], NULL);
      owner = aw_parser_read_element_text (cells[8], -1);

      l->data = aw_planet_new (-1, flags, name, owner, population);

      aw_planet_set_building_levels (l->data,
                                     AW_ITEM_STARBASE, starbase,
                                     AW_ITEM_INVALID);

      g_strfreev (cells);
      g_free (owner);
      g_free (name);
    }

  if (l)
    rows = (aw_parser_list_free (rows, l, aw_planet_unref), NULL);

  g_free (star_name);

  return rows;
}

GList *
aw_parser_read_planets (const char  *data,
                        gssize       length,
                        GError     **error)
{
  GList      *rows = NULL, *l, *n;
  char       *username = NULL;
  int         start, end;

  if (aw_parser_find_table (data, length, "Name", &start, &end, error))
    rows = aw_parser_read_table (data + start, end - start, 6, error);

  g_object_get (aw_settings_get_singleton (),
                "username", &username, NULL);

  for (l = rows; l && (n = l->next, l); l = n)
    {
      char          **cells = l->data;
      AwPlanetFlags   flags = AW_PLANET_DEFAULT;
      int             population, production_points;
      int             gph, pph, id;
      char           *name;

      if ((1 != sscanf (cells[4],  "%d", &population))        ||
          (1 != sscanf (cells[8],  "%d", &gph))               ||
          (1 != sscanf (cells[10], "%d", &production_points)) ||
          (1 != sscanf (cells[12], "%d", &pph)))
        break;

      if (!aw_parser_read_planet_name (cells[2], &name, &id))
        break;

      if (g_strcmp0 (cells[0], "#404040"))
        flags |= AW_PLANET_SIEGED;

      l->data = aw_planet_new (id, flags, name, username, population);
      aw_planet_set_building_levels (l->data, AW_ITEM_FARM, gph - 1,
                                     AW_ITEM_FACTORY, pph - population,
                                     AW_ITEM_INVALID);
      aw_planet_set_production_points (l->data, production_points);

      g_strfreev (cells);
      g_free (name);
    }

  if (l)
    rows = (aw_parser_list_free (rows, l, aw_planet_unref), NULL);

  g_free (username);

  return rows;
}

GList *
aw_parser_read_buildings (const char  *data,
                          gssize       length,
                          GError     **error)
{
  GList      *rows = NULL, *l, *n;
  char       *username = NULL;
  int         start, end;

  if (aw_parser_find_table (data, length, "Name", &start, &end, error))
    rows = aw_parser_read_table (data + start, end - start, 7, error);

  g_object_get (aw_settings_get_singleton (),
                "username", &username, NULL);

  for (l = rows; l && (n = l->next, l); l = n)
    {
      char          **cells = l->data;
      AwPlanetFlags   flags = AW_PLANET_DEFAULT;

      if (g_strcmp0 (cells[2], "Average") && g_strcmp0 (cells[2], "Sum"))
        {
          int   farm, factory, cybernet, laboratory;
          int   population, production_points, id;
          char *name;

          if ((1 != sscanf (cells[4],  "%d", &population)) ||
              (1 != sscanf (cells[6],  "%d", &farm))       ||
              (1 != sscanf (cells[8],  "%d", &factory))    ||
              (1 != sscanf (cells[10], "%d", &cybernet))   ||
              (1 != sscanf (cells[12], "%d", &laboratory)) ||
              (1 != sscanf (cells[14], "%d", &production_points)))
            break;

          if (!aw_parser_read_planet_name (cells[2], &name, &id))
            break;

          if (g_strcmp0 (cells[0], "#404040"))
            flags |= AW_PLANET_SIEGED;

          l->data = aw_planet_new (id, flags, name, username, population);

          aw_planet_set_building_levels (l->data,
                                         AW_ITEM_FARM,       farm,
                                         AW_ITEM_FACTORY,    factory,
                                         AW_ITEM_CYBERNET,   cybernet,
                                         AW_ITEM_LABORATORY, laboratory,
                                         AW_ITEM_INVALID);

          aw_planet_set_production_points (l->data, production_points);

          g_free (name);
        }
      else
        rows = g_list_delete_link (rows, l);

      g_strfreev (cells);
    }

  if (l)
    rows = (aw_parser_list_free (rows, l, aw_planet_unref), NULL);

  g_free (username);

  return rows;
}

static gboolean
aw_parser_read_science_stats (const char  *data,
                              gssize       length,
                              const char  *key,
                              int         *output,
                              int         *bonus,
                              GError     **error)
{
  gboolean    success = FALSE;
  char       *pattern = NULL;
  char       *text    = NULL;
  GRegex     *regex   = NULL;
  GMatchInfo *info    = NULL;

  pattern = g_strconcat ("<b>", key, "</b>.*?"
                         "\\(([+-]\\d+) per hour\\)(?:(?:.(?!</td>))*?"
                         "<b>([+-]\\d+)%</b>)?", NULL);

  regex = g_regex_new (pattern, G_REGEX_CASELESS | G_REGEX_DOTALL, 0, error);

  if (!regex || !g_regex_match_full (regex, data, length, 0, 0, &info, error))
    goto out;

  text = g_match_info_fetch (info, 1);

  if (!text || (1 != sscanf (text, "%d", output)))
    goto out;

  g_free (text);

  text = g_match_info_fetch (info, 2);

  if (!text)
    *bonus = 0;
  else if (1 != sscanf (text, "%d", bonus))
    goto out;

  g_free (text);
  text = NULL;

  success = TRUE;

out:
  if (info)
    g_match_info_free (info);
  if (regex)
    g_regex_unref (regex);

  g_free (pattern);
  g_free (text);

  return success;
}

GList *
aw_parser_read_sciences (const char      *data,
                         gssize           length,
                         AwScienceId     *current,
                         AwScienceStats  *stats,
                         GError         **error)
{
  GList       *rows = NULL, *l, *n;
  int          start, end;
  AwScienceId  id = 0;

  if (stats)
    {
      if (!aw_parser_read_science_stats (data, length, "Science",
                                         &stats->science_output,
                                         &stats->science_bonus, error) ||
          !aw_parser_read_science_stats (data, length, "Culture",
                                         &stats->culture_output,
                                         &stats->culture_bonus, error))
        return NULL;
    }

  if (aw_parser_find_table (data, length, "lvl", &start, &end, error))
    rows = aw_parser_read_table (data + start, end - start, 5, error);

  for (l = rows; l && (n = l->next, l); l = n)
    {
      char       **cells = l->data;
      int          level, remaining;
      const char  *strval;

      if (!aw_parser_find_element_text (cells[2], -1, &start, &end) ||
          g_ascii_strncasecmp (aw_science_id_get_nick (id), cells[2] + start, end - start))
        break;

      if (NULL != (strval = strstr (cells[8], "value=")))
        {
          if (current && id < AW_SCIENCE_CULTURE)
            *current = id;

          strval += strlen ("value=");

          if (strchr ("\"'", *strval))
            ++strval;
        }
      else
        strval = cells[8];

      if ((1 != sscanf (cells[4], "%d", &level)) ||
          (1 != sscanf (strval,   "%d", &remaining)))
        break;

      l->data = aw_science_new (id++, level, remaining);
      g_strfreev (cells);
    }

  if (l || (AW_SCIENCE_CULTURE + 1) != id)
    rows = (aw_parser_list_free (rows, l, aw_science_unref), NULL);

  return rows;
}

GList *
aw_parser_read_fleets (const char  *data,
                       gssize       length,
                       GError     **error)
{
  GList *rows = NULL, *l, *n;
  int    start, end;

  if (aw_parser_find_table (data, length, "Estimated Arrival", &start, &end, error) ||
      aw_parser_find_table (data, length, "Location", &start, &end, error))
    rows = aw_parser_read_table (data + start, end - start, 7, error);

  for (l = rows; l && (n = l->next, l); l = n)
    {
      char         **cells = l->data;
      int            destroyers, cruisers, battleships;
      int            transports, colony_ships, system_id;
      char         **date_time   = NULL;
      char          *planet_name = NULL;
      AwFleetFlags   flags = 0;
      const char    *tmp;

      if (!(planet_name = aw_parser_read_small_name (cells[4])) ||
          !(tmp = strstr (cells[4], "hl=")))
        break;

      if ((1 != sscanf (tmp + 3,   "%d", &system_id))    ||
          (1 != sscanf (cells[6],  "%d", &transports))   ||
          (1 != sscanf (cells[8],  "%d", &colony_ships)) ||
          (1 != sscanf (cells[10], "%d", &destroyers))   ||
          (1 != sscanf (cells[12], "%d", &cruisers))     ||
          (1 != sscanf (cells[14], "%d", &battleships)))
        break;

      if (strstr (cells[2], " - "))
        date_time = g_strsplit (cells[2], " - ", -1);
      else if (strstr (cells[2], "pending"))
        flags |= AW_FLEET_PENDING;
      if (!g_strcmp0 (cells[0], "#602020"))
        flags |= AW_FLEET_SIEGING;

      l->data = aw_fleet_new (date_time ? date_time[0] : NULL,
                              date_time ? date_time[1] : NULL,
                              system_id, planet_name, flags);

      aw_fleet_set_vessel_counts (l->data,
                                  AW_ITEM_TRANSPORT,   transports,
                                  AW_ITEM_COLONY_SHIP, colony_ships,
                                  AW_ITEM_DESTROYER,   destroyers,
                                  AW_ITEM_CRUISER,     cruisers,
                                  AW_ITEM_BATTLESHIP,  battleships,
                                  AW_ITEM_INVALID);

      if (date_time)
        g_strfreev (date_time);

      g_free (planet_name);
      g_strfreev (cells);
    }

  if (l)
    rows = (aw_parser_list_free (rows, l, aw_fleet_unref), NULL);

  return rows;
}

static gboolean
aw_parser_read_timespan (const char *text,
                         int        *result)
{
  if (1 != sscanf (text, "%d", result))
    return FALSE;

  text += strspn (text, "0123456789 ");

  if (g_str_has_prefix (text, "minute"))
    *result *= 60;
  else if (g_str_has_prefix (text, "hour"))
    *result *= 60 * 60;
  else if (g_str_has_prefix (text, "day"))
    *result *= 60 * 60 * 24;
  else
    return FALSE;

  return TRUE;
}

static AwProfile *
aw_parser_read_basic_profile (const char  *data,
                              gssize       length,
                              int          profile_id,
                              GError     **error)
{
  gboolean    success = FALSE;
  AwProfile  *profile = NULL;
  GList      *rows = NULL, *l, *n;
  const char *data_end, *start, *end;
  GRegex     *regex = NULL;
  GMatchInfo *info = NULL;
  int         hour, min;
  int         ival, ival2;
  char       *sval;

  data_end = data + length;

  if (!(end = g_strstr_len (data, length, "</title>")) ||
      2 != sscanf (end - 8, "%02d:%02d:", &hour, &min))
    goto out;

  if (!(end = g_strstr_len (data, length, "Local Time")) ||
      !(start = g_strrstr_len (data, end - data, "<table")))
    goto out;

  regex = g_regex_new ("<center><b>(?:"
                       "(?:<a\\b.*?/Alliances/.*?\\btag=(.*?)[&\"].*</a>)?\\s*"
                       "(?:<font\\b.*?>(.*?)\\s+\\((\\d+)(?:st|nd|rd|th)\\)</font>|(.*?))"
                       ")</b></center>",
                       G_REGEX_CASELESS | G_REGEX_DOTALL, 0, error);

  if (!regex || !g_regex_match_full (regex, start, end - start, 0, 0, &info, NULL))
    goto out;

  if (!(end = g_strstr_len (start, data_end - start, "</table>")) ||
      !(rows = aw_parser_read_table (start, end - start, 2, error)))
    goto out;

  sval = g_match_info_fetch (info, 2);

  if (sval && *sval)
    {
      profile = aw_profile_new (profile_id, sval);

      g_free (sval);
      sval = g_match_info_fetch (info, 3);

      if (1 == sscanf (sval, "%d", &ival))
        aw_profile_set_permanent_rank (profile, ival);
    }
  else
    {
      g_free (sval);
      sval = g_match_info_fetch (info, 4);

      profile = aw_profile_new (profile_id, sval);
    }

  g_free (sval);

  sval = g_match_info_fetch (info, 1);

  if (sval && *sval)
    aw_profile_set_alliance_tag (profile, sval);

  g_free (sval);

  for (l = rows; l && (n = l->next, l); l = n)
    {
      char **cells = l->data;

      if (!g_strcmp0 (cells[2], "Local Time"))
        {
          if (2 != sscanf (cells[4], "%d:%d", &ival, &ival2))
            break;

          ival = (ival * 60 + ival2) - (hour * 60 + min);
          aw_profile_set_timezone (profile, ival);
          continue;
        }

      if (!g_strcmp0 (cells[2], "Joined"))
        {
          aw_profile_set_start_date (profile, g_strstrip (cells[4]));
          continue;
        }

      if (!g_strcmp0 (cells[2], "Logins"))
        {
          if (1 != sscanf (cells[4], "%d", &ival))
            break;

          aw_profile_set_login_count (profile, ival);
          continue;
        }

      if (!g_strcmp0 (cells[2], "Idle"))
        {
          if (!aw_parser_read_timespan (cells[4], &ival))
            break;

          aw_profile_set_idle_time (profile, ival);
          continue;
        }

      if (!g_strcmp0 (cells[2], "Sciencelevel"))
        {
          if (1 != sscanf (cells[4], "%d", &ival))
            break;

          aw_profile_set_level (profile, AW_SCIENCE_OVERALL, ival);
          continue;
        }

      if (!g_strcmp0 (cells[2], "Culturelevel"))
        {
          if (1 != sscanf (cells[4], "%d", &ival))
            break;

          aw_profile_set_level (profile, AW_SCIENCE_CULTURE, ival);
          continue;
        }

      if (!g_strcmp0 (cells[2], "Rank (Points Scored)"))
        {
          if (2 != sscanf (cells[4], "#%d (%d)", &ival, &ival2))
            break;

          aw_profile_set_rank (profile, ival);
          aw_profile_set_score (profile, ival2);
          continue;
        }

      if (!g_strcmp0 (cells[2], "Origin"))
        {
          if (!(sval = strstr (cells[4], "x:y")) ||
              2 != sscanf (sval, "x:y(%d/%d)", &ival, &ival2))
            break;

          aw_profile_set_origin (profile, ival, ival2);
          continue;
        }

      if (!g_strcmp0 (cells[2], "Plays from"))
        {
          if (!(sval = strstr (cells[4], "/country/")))
            break;

          sval += strlen ("/country/");
          sval[strcspn (sval, ".")] = '\0';

          aw_profile_set_country (profile, sval);
          continue;
        }

      if (strstr (cells[2], ">Playerlevel<"))
        {
          if (2 != sscanf (cells[4], "%d - %d%%", &ival, &ival2))
            break;

          aw_profile_set_level (profile, AW_SCIENCE_WARFARE, ival + ival2 * 0.01);
          continue;
        }
    }

  success = TRUE;

out:
  if (!success)
    profile = (g_object_unref (profile), NULL);

  g_list_foreach (rows, (GFunc) g_strfreev, NULL);
  g_list_free (rows);

  return profile;
}

static gboolean
aw_parser_read_science_profile (AwProfile   *profile,
                                const char  *data,
                                gssize       length,
                                GError     **error)
{
  GList *rows = NULL, *l;
  int    start, end;

  if (aw_parser_find_table (data, length, "Intelligence Report", &start, &end, error))
    rows = aw_parser_read_table (data + start, end - start, 2, error);

  for (l = rows; l; l = l->next)
    {
      const char  **cells = l->data;
      int           value;
      AwScienceId   id;

      if (1 != (sscanf (cells[4], "%d", &value)))
        break;

      if (!strcmp (cells[2], "Trade Revenue"))
        {
          aw_profile_set_bonus (profile, AW_BONUS_TRADE, 1 + value * 0.01);
          continue;
        }

      for (id = 0; id < AW_SCIENCE_CULTURE; ++id)
        {
          if (!g_ascii_strcasecmp (aw_science_id_get_nick (id), cells[2]))
            {
              aw_profile_set_level (profile, id, value);
              break;
            }
        }
    }

  g_list_foreach (rows, (GFunc) g_strfreev, NULL);
  g_list_free (rows);

  return (NULL == l);
}

static gboolean
aw_parser_read_race_profile (AwProfile   *profile,
                             const char  *data,
                             gssize       length,
                             GError     **error)
{
  const char *data_end = data + length;
  const char *start, *end = NULL;

  if (!(start = g_strstr_len (data, length, "<b>Race Summary</b>")) ||
      !(end = g_strstr_len (start, data_end - start, "</ul>")))
    return TRUE;

  while (start < end)
    {
      AwBonusType bonus;
      int         value;

      if (!(start = g_strstr_len (start, data_end - start, "<li>")))
        break;

      start += strspn (start, "<li>+");

      if (1 != sscanf (start, "%d", &value))
        break;

      start += strspn (start, "0123456789%h- ");

      for (bonus = 0; bonus < AW_BONUS_LAST; ++bonus)
        {
          const char *nick = aw_bonus_type_get_nick (bonus);

          if (!g_ascii_strncasecmp (nick, start, strlen (nick)))
            {
              if (AW_BONUS_SPEED != bonus)
                aw_profile_set_bonus (profile, bonus, 1 + value * 0.01);
              else
                aw_profile_set_bonus (profile, bonus, value);

              break;
            }
        }

      if (bonus >= AW_BONUS_LAST)
        break;
    }

  return !start;
}

AwProfile *
aw_parser_read_profile (const char  *data,
                        gssize       length,
                        int          profile_id,
                        GError     **error)
{
  AwProfile *profile;

  if (!(profile = aw_parser_read_basic_profile (data, length, profile_id, error)))
    return NULL;

  if (!aw_parser_read_science_profile (profile, data, length, error) ||
      !aw_parser_read_race_profile (profile, data, length, error))
    profile = (g_object_unref (profile), NULL);

  return profile;
}

