#ifndef __AW_MODEL_PROFILE_H__
#define __AW_MODEL_PROFILE_H__ 1

#include "bonus.h"
#include "point.h"
#include "science.h"

#define AW_TYPE_PROFILE             (aw_profile_get_type ())
#define AW_PROFILE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_PROFILE, AwProfile))
#define AW_PROFILE_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_PROFILE, AwProfileClass))
#define AW_IS_PROFILE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_PROFILE))
#define AW_IS_PROFILE_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_PROFILE))
#define AW_PROFILE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_PROFILE, AwMainViewClass))

typedef struct _AwProfile        AwProfile;
typedef struct _AwProfilePrivate AwProfilePrivate;
typedef struct _AwProfileClass   AwProfileClass;

struct _AwProfile {
  GObject          parent;
  AwProfilePrivate *priv;
};

struct _AwProfileClass {
  GObjectClass parent;
};

GType
aw_profile_get_type           (void) G_GNUC_CONST;

AwProfile *
aw_profile_new                (int              id,
                               const char      *name);

AwProfile *
aw_profile_get_self           (void);

void
aw_profile_reset              (AwProfile       *profile,
                               const AwProfile *other);

void
aw_profile_set_id             (AwProfile       *profile,
                               int              id);

int
aw_profile_get_id             (const AwProfile *profile);

void
aw_profile_set_name           (AwProfile       *profile,
                               const char      *name);

G_CONST_RETURN char *
aw_profile_get_name           (const AwProfile *profile);

void
aw_profile_set_alliance_tag   (AwProfile       *profile,
                               const char      *tag);

G_CONST_RETURN char *
aw_profile_get_alliance_tag   (const AwProfile *profile);

void
aw_profile_set_start_date     (AwProfile       *profile,
                               const char      *date);

G_CONST_RETURN char *
aw_profile_get_start_date     (const AwProfile *profile);

void
aw_profile_set_country        (AwProfile       *profile,
                               const char      *country);

G_CONST_RETURN char *
aw_profile_get_country        (const AwProfile *profile);

void
aw_profile_set_timezone       (AwProfile       *profile,
                               int              offset);

int
aw_profile_get_timezone       (const AwProfile *profile);

void
aw_profile_set_login_count    (AwProfile       *profile,
                               int              count);

int
aw_profile_get_login_count    (const AwProfile *profile);

void
aw_profile_set_idle_time      (AwProfile       *profile,
                               int              seconds);

int
aw_profile_get_idle_time      (const AwProfile *profile);


void
aw_profile_set_origin         (const AwProfile *profile,
                               int              x,
                               int              y);

G_CONST_RETURN AwPoint *
aw_profile_get_origin         (const AwProfile *profile);

void
aw_profile_set_level          (const AwProfile *profile,
                               AwScienceId      id,
                               float            value);

float
aw_profile_get_level          (const AwProfile *profile,
                               AwScienceId      id);

void
aw_profile_set_bonus          (const AwProfile *profile,
                               AwBonusType      bonus,
                               float            value);

float
aw_profile_get_bonus          (const AwProfile *profile,
                               AwBonusType      bonus);

void
aw_profile_set_score          (AwProfile       *profile,
                               int              score);

int
aw_profile_get_score          (const AwProfile *profile);

void
aw_profile_set_rank           (AwProfile       *profile,
                               int              rank);

int
aw_profile_get_rank           (const AwProfile *profile);

void
aw_profile_set_permanent_rank (AwProfile       *profile,
                               int              rank);

int
aw_profile_get_permanent_rank (const AwProfile *profile);

#endif /* __AW_MODEL_PROFILE_H__ */

