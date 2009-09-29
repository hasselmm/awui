#ifndef __AW_MODEL_STAR_H__
#define __AW_MODEL_STAR_H__ 1

#include <glib-object.h>

#define AW_TYPE_STAR (aw_star_get_type ())

typedef struct _AwStar  AwStar;

typedef int (*AwStarCompareFunc) (AwStar *star_a,
                                  AwStar *star_b);

GType
aw_star_get_type         (void) G_GNUC_CONST;

AwStar *
aw_star_new              (int           id,
                          const char   *name,
                          int           level,
                          int           x,
                          int           y);

int
aw_star_get_id           (const AwStar *star);

G_CONST_RETURN char *
aw_star_get_name         (const AwStar *star);

int
aw_star_get_level        (const AwStar *star);

int
aw_star_get_x            (const AwStar *star);

int
aw_star_get_y            (const AwStar *star);

AwStar *
aw_star_ref              (AwStar       *star);

void
aw_star_unref            (AwStar       *star);

void
aw_star_list_free        (GList        *list);

int
aw_star_compare_by_name  (AwStar       *star_a,
                          AwStar       *star_b);

int
aw_star_compare_by_id    (AwStar       *star_a,
                          AwStar       *star_b);

int
aw_star_compare_by_level (AwStar       *star_a,
                          AwStar       *star_b);

double
aw_star_get_distance     (AwStar       *star_a,
                          AwStar       *star_b);

#endif /* __AW_MODEL_STAR_H__ */
