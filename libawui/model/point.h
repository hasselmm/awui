#ifndef __AW_MODEL_POINT_H__
#define __AW_MODEL_POINT_H__ 1

#include <glib-object.h>

#define AW_TYPE_POINT (aw_point_get_type ())

typedef struct _AwPoint AwPoint;

struct _AwPoint {
  int x;
  int y;
};

GType
aw_point_get_type     (void) G_GNUC_CONST;

AwPoint *
aw_point_new          (int            x,
                       int            y);

double
aw_point_get_distance (const AwPoint *a,
                       const AwPoint *b);

AwPoint *
aw_point_copy         (const AwPoint *point);

void
aw_point_free         (AwPoint       *point);

#endif /* __AW_MODEL_POINT_H__ */

