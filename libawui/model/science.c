#include "config.h"
#include "science.h"

#include <glib/gi18n-lib.h>
#include <math.h>

struct _AwScience {
  AwScienceId id;
  int         level;
  int         remaining;
  int         ref_count;
};

static gpointer
aw_science_copy (gpointer boxed)
{
  return aw_science_ref (boxed);
}

static void
aw_science_free (gpointer boxed)
{
  aw_science_unref (boxed);
}

GType
aw_science_get_type  (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    type = g_boxed_type_register_static ("AwScience", aw_science_copy, aw_science_free);

  return type;
}

AwScience *
aw_science_new (AwScienceId id,
                int         level,
                int         remaining)
{
  AwScience *science;

  science = g_slice_new (AwScience);

  science->id        = id;
  science->level     = level;
  science->remaining = remaining;
  science->ref_count = 1;

  return science;
}

void
aw_science_set (AwScience *science,
                int        level,
                int        remaining)
{
  g_return_if_fail (NULL != science);

  science->level     = level;
  science->remaining = remaining;
}

/* ================================================================= */

AwScienceId
aw_science_get_id (const AwScience *science)
{
  g_return_val_if_fail (NULL != science, AW_SCIENCE_INVALID);
  return science->id;
}

G_CONST_RETURN char *
aw_science_get_name (const AwScience *science)
{
  g_return_val_if_fail (NULL != science, NULL);
  return aw_science_id_get_display_name (science->id);
}

int
aw_science_get_level (const AwScience *science)
{
  g_return_val_if_fail (NULL != science, -1);
  return science->level;
}

int
aw_science_get_remaining (const AwScience *science)
{
  g_return_val_if_fail (NULL != science, -1);
  return science->remaining;
}

/* ================================================================= */

AwScience *
aw_science_ref (AwScience *science)
{
  g_return_val_if_fail (NULL != science, NULL);
  g_atomic_int_inc (&science->ref_count);
  return science;
}

void
aw_science_unref (AwScience *science)
{
  if (science && g_atomic_int_dec_and_test (&science->ref_count))
    g_slice_free (AwScience, science);
}

void
aw_science_list_free (GList *list)
{
  while (list)
    {
      aw_science_unref (list->data);
      list = g_list_delete_link (list, list);
    } 
}

G_CONST_RETURN char *
aw_science_id_get_display_name (AwScienceId id)
{
  switch (id)
    {
    case AW_SCIENCE_BIOLOGY:
      return _("Biology");
    case AW_SCIENCE_ECONOMY:
      return _("Economy");
    case AW_SCIENCE_ENERGY:
      return _("Energy");
    case AW_SCIENCE_MATHEMATICS:
      return _("Mathematics");
    case AW_SCIENCE_PHYSICS:
      return _("Physics");
    case AW_SCIENCE_SOCIAL:
      return _("Social");
    case AW_SCIENCE_CULTURE:
      return _("Culture");
    case AW_SCIENCE_WARFARE:
      return _("Player Level");
    case AW_SCIENCE_OVERALL:
      return _("Science Level");
    case AW_SCIENCE_INVALID:
    case AW_SCIENCE_LAST:
      break;
    }

  return NULL;
}

int
aw_science_get_research_cost (AwScienceId id,
                              int         level)
{
  static const int science_levels[] = {
    29, 74, 138, 221, 325, 451, 603, 780, 986, 1223, 1492, 1796,
    2138, 2520, 2945, 3415, 3934, 4505, 5131, 5816, 6563, 7377,
    8261, 9221, 10260, 11382, 12595, 13901, 15400, 16715,
  };

  static const int culture_levels[] = {
    0, 318, 765, 1315, 2084, 2985, 4059, 5320, 6785, 8467,
    10382, 12547, 14979, 17697, 20721, 24071, 27768, 31835,
    36298, 41179, 46507, 52310, 58616, 65456, 72863,
  };

  g_return_val_if_fail (id >= 0,              -1);
  g_return_val_if_fail (id < AW_SCIENCE_LAST, -1);
  g_return_val_if_fail (level >= 0,           -1);

  switch (id)
    {
    case AW_SCIENCE_BIOLOGY:
    case AW_SCIENCE_ECONOMY:
    case AW_SCIENCE_ENERGY:
    case AW_SCIENCE_MATHEMATICS:
    case AW_SCIENCE_PHYSICS:
    case AW_SCIENCE_SOCIAL:
      g_return_val_if_fail (level < G_N_ELEMENTS (science_levels), -1);
        return science_levels[level];

    case AW_SCIENCE_CULTURE:
      g_return_val_if_fail (level < G_N_ELEMENTS (culture_levels), -1);
      return culture_levels[level];

    case AW_SCIENCE_WARFARE:
      return 5 * pow (level, 2.7) * 5 + 0.5;

    case AW_SCIENCE_OVERALL:
    case AW_SCIENCE_INVALID:
    case AW_SCIENCE_LAST:
      break;
    }

  g_return_val_if_reached (-1);
}

