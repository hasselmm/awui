#include "config.h"
#include "profile.h"
#include "settings.h"

#include <math.h>

struct _AwProfilePrivate {
  int      id;

  char    *name;
  char    *tag;
  char    *country;
  char    *start_date;

  int      timezone;
  int      login_count;
  int      idle_time;

  int      score;
  int      rank;
  int      permanent_rank;

  AwPoint  origin;

  float    levels[AW_SCIENCE_LAST];
  float    boni[AW_BONUS_LAST];
};

G_DEFINE_TYPE (AwProfile, aw_profile, G_TYPE_OBJECT);

static void
aw_profile_init (AwProfile *profile)
{
  int i;

  profile->priv = G_TYPE_INSTANCE_GET_PRIVATE (profile, AW_TYPE_PROFILE,
                                               AwProfilePrivate);

  for (i = 0; i < G_N_ELEMENTS (profile->priv->levels); ++i)
    profile->priv->levels[i] = -HUGE_VAL;
  for (i = 0; i < G_N_ELEMENTS (profile->priv->boni); ++i)
    profile->priv->boni[i] = -HUGE_VAL;
}

static void
aw_profile_finalize (GObject *object)
{
  AwProfile *profile = AW_PROFILE (object);

  g_free (profile->priv->start_date);
  g_free (profile->priv->country);
  g_free (profile->priv->name);
  g_free (profile->priv->tag);

  G_OBJECT_CLASS (aw_profile_parent_class)->finalize (object);
}

static void
aw_profile_class_init (AwProfileClass *class)
{
  GObjectClass *object_class;

  object_class           = G_OBJECT_CLASS (class);
  object_class->finalize = aw_profile_finalize;

  g_type_class_add_private (class, sizeof (AwProfilePrivate));
}

AwProfile *
aw_profile_new (int            id,
               const char    *name)
{
#if 0
  return g_object_new (AW_TYPE_PROFILE, "id", id, "name", name, NULL);
#else
  AwProfile *profile;

  profile = g_object_new (AW_TYPE_PROFILE, NULL);
  profile->priv->name = g_strdup (name);
  profile->priv->id = id;

  return profile;
#endif
}

AwProfile *
aw_profile_get_self (void)
{
  static AwProfile *profile = NULL;

  if (G_LIKELY (profile))
    return g_object_ref (profile);

  profile = g_object_new (AW_TYPE_PROFILE, NULL);

  g_object_get (aw_settings_get_singleton (),
                "profile-id", &profile->priv->id,
                "username",   &profile->priv->name, NULL);

  g_object_add_weak_pointer (G_OBJECT (profile), (gpointer) &profile);

  return profile;
}

void
aw_profile_reset (AwProfile       *profile,
                  const AwProfile *other)
{
  g_return_if_fail (AW_IS_PROFILE (profile));
  g_return_if_fail (AW_IS_PROFILE (other));

  g_free (profile->priv->start_date);
  g_free (profile->priv->country);
  g_free (profile->priv->name);
  g_free (profile->priv->tag);

  *profile->priv = *other->priv;

  profile->priv->start_date = g_strdup (profile->priv->start_date);
  profile->priv->country = g_strdup (profile->priv->country);
  profile->priv->name = g_strdup (profile->priv->name);
  profile->priv->tag = g_strdup (profile->priv->tag);
}

int
aw_profile_get_id (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), -1);
  return profile->priv->id;
}

G_CONST_RETURN char *
aw_profile_get_name (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), NULL);
  return profile->priv->name;
}

void
aw_profile_set_alliance_tag (AwProfile  *profile,
                             const char *tag)
{
  g_return_if_fail (AW_IS_PROFILE (profile));

  g_free (profile->priv->tag);
  profile->priv->tag = g_strdup (tag);
}

G_CONST_RETURN char *
aw_profile_get_alliance_tag (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), NULL);
  return profile->priv->tag;
}

void
aw_profile_set_origin (const AwProfile *profile,
                       int              x,
                       int              y)
{
  g_return_if_fail (AW_IS_PROFILE (profile));
  profile->priv->origin.x = x;
  profile->priv->origin.y = y;
}


G_CONST_RETURN AwPoint *
aw_profile_get_origin (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), NULL);
  return &profile->priv->origin;
}

void
aw_profile_set_level (const AwProfile *profile,
                      AwScienceId      id,
                      float            value)
{
  g_return_if_fail (id >= 0);
  g_return_if_fail (id < AW_SCIENCE_LAST);
  g_return_if_fail (AW_IS_PROFILE (profile));

  profile->priv->levels[id] = value;

  if (id < AW_SCIENCE_CULTURE)
    profile->priv->levels[AW_SCIENCE_OVERALL] =
      MAX (profile->priv->levels[AW_SCIENCE_OVERALL], value);
}

float
aw_profile_get_level (const AwProfile *profile,
                      AwScienceId      id)
{
  g_return_val_if_fail (id >= 0,                 -HUGE_VAL);
  g_return_val_if_fail (id < AW_SCIENCE_LAST,    -HUGE_VAL);
  g_return_val_if_fail (AW_IS_PROFILE (profile), -HUGE_VAL);
  return profile->priv->levels[id];
}

void
aw_profile_set_bonus (const AwProfile *profile,
                      AwBonusType      bonus,
                      float            value)
{
  g_return_if_fail (bonus >= 0);
  g_return_if_fail (bonus < AW_BONUS_LAST);
  g_return_if_fail (AW_IS_PROFILE (profile));
  profile->priv->boni[bonus] = value;
}

float
aw_profile_get_bonus (const AwProfile *profile,
                      AwBonusType      bonus)
{
  g_return_val_if_fail (bonus >= 0,              -HUGE_VAL);
  g_return_val_if_fail (bonus < AW_BONUS_LAST,   -HUGE_VAL);
  g_return_val_if_fail (AW_IS_PROFILE (profile), -HUGE_VAL);
  return profile->priv->boni[bonus];
}

void
aw_profile_set_score (AwProfile *profile,
                      int        score)
{
  g_return_if_fail (AW_IS_PROFILE (profile));
  profile->priv->score = score;
}

int
aw_profile_get_score (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), -1);
  return profile->priv->score;
}

void
aw_profile_set_rank (AwProfile *profile,
                     int        rank)
{
  g_return_if_fail (AW_IS_PROFILE (profile));
  profile->priv->rank = rank;
}

int
aw_profile_get_rank (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), -1);
  return profile->priv->rank;
}

void
aw_profile_set_permanent_rank (AwProfile *profile,
                               int        rank)
{
  g_return_if_fail (AW_IS_PROFILE (profile));
  profile->priv->permanent_rank = rank;
}

int
aw_profile_get_permanent_rank (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), -1);
  return profile->priv->permanent_rank;
}

void
aw_profile_set_start_date (AwProfile  *profile,
                           const char *date)
{
  g_return_if_fail (AW_IS_PROFILE (profile));

  g_free (profile->priv->start_date);
  profile->priv->start_date = g_strdup (date);
}

G_CONST_RETURN char *
aw_profile_get_start_date (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), NULL);
  return profile->priv->start_date;
}

void
aw_profile_set_login_count (AwProfile *profile,
                            int        count)
{
  g_return_if_fail (AW_IS_PROFILE (profile));
  profile->priv->login_count = count;
}

int
aw_profile_get_login_count (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), -1);
  return profile->priv->login_count;
}

void
aw_profile_set_idle_time (AwProfile *profile,
                          int        seconds)
{
  g_return_if_fail (AW_IS_PROFILE (profile));
  profile->priv->idle_time = seconds;
}

int
aw_profile_get_idle_time (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), -1);
  return profile->priv->idle_time;
}

void
aw_profile_set_timezone (AwProfile *profile,
                         int        offset)
{
  g_return_if_fail (AW_IS_PROFILE (profile));
  profile->priv->timezone = offset;
}

int
aw_profile_get_timezone (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), -1);
  return profile->priv->timezone;
}

void
aw_profile_set_country (AwProfile  *profile,
                        const char *country)
{
  g_return_if_fail (AW_IS_PROFILE (profile));

  g_free (profile->priv->country);
  profile->priv->country = g_strdup (country);
}

G_CONST_RETURN char *
aw_profile_get_country (const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), NULL);
  return profile->priv->country;
}

