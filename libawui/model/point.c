#include "config.h"
#include "point.h"

#include <math.h>

static gpointer
aw_point_copy_boxed (gpointer boxed)
{
  return aw_point_copy (boxed);
}

static void
aw_point_free_boxed (gpointer boxed)
{
  aw_point_free (boxed);
}

GType
aw_point_get_type  (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    type = g_boxed_type_register_static ("AwPoint", aw_point_copy_boxed, aw_point_free_boxed);

  return type;
}

AwPoint *
aw_point_new (int x,
              int y)
{
  AwPoint *point;

  point    = g_slice_new (AwPoint);
  point->x = x;
  point->y = y;

  return point;
}

double
aw_point_get_distance (const AwPoint *a,
                       const AwPoint *b)
{
  const int xa = (a ? a->x : 0);
  const int ya = (a ? a->y : 0);
  const int xb = (b ? b->x : 0);
  const int yb = (b ? b->y : 0);
  const int dx = (xa - xb);
  const int dy = (ya - yb);

  return sqrt (dx * dx + dy * dy);
}

AwPoint *
aw_point_copy (const AwPoint *point)
{
  if (point)
    return aw_point_new (point->x, point->y);

  return NULL;
}

void
aw_point_free (AwPoint *point)
{
  if (point)
    g_slice_free (AwPoint, point);
}

