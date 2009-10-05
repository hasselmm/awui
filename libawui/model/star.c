#include "config.h"
#include "star.h"

struct _AwStar {
  int           ref_count;
  int           id;
  char         *name;
  AwPoint       location;
  int           level;
};

static gpointer
aw_star_copy (gpointer boxed)
{
  return aw_star_ref (boxed);
}

static void
aw_star_free (gpointer boxed)
{
  aw_star_unref (boxed);
}

GType
aw_star_get_type  (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    type = g_boxed_type_register_static ("AwStar", aw_star_copy, aw_star_free);

  return type;
}

AwStar *
aw_star_new (int         id,
             const char *name,
             int         level,
             int         x,
             int         y)
{
  AwStar *star;

  g_return_val_if_fail (NULL != name, NULL);

  star = g_slice_new (AwStar);

  star->id         = id;
  star->name       = g_strdup (name);
  star->level      = level;
  star->location.x = x;
  star->location.y = y;
  star->ref_count  = 1;

  return star;
}

/* ================================================================= */

int
aw_star_get_id (const AwStar *star)
{
  g_return_val_if_fail (NULL != star, -1);
  return star->id;
}

G_CONST_RETURN char *
aw_star_get_name (const AwStar *star)
{
  g_return_val_if_fail (NULL != star, NULL);
  return star->name;
}

int
aw_star_get_level (const AwStar *star)
{
  g_return_val_if_fail (NULL != star, -1);
  return star->level;
}

int
aw_star_get_x (const AwStar *star)
{
  g_return_val_if_fail (NULL != star, 0);
  return star->location.x;
}

int
aw_star_get_y (const AwStar *star)
{
  g_return_val_if_fail (NULL != star, 0);
  return star->location.y;
}

G_CONST_RETURN AwPoint *
aw_star_get_location (const AwStar *star)
{
  g_return_val_if_fail (NULL != star, NULL);
  return &star->location;
}

/* ================================================================= */

AwStar *
aw_star_ref (AwStar *star)
{
  g_return_val_if_fail (NULL != star, NULL);
  g_atomic_int_inc (&star->ref_count);
  return star;
}

void
aw_star_unref (AwStar *star)
{
  if (star && g_atomic_int_dec_and_test (&star->ref_count))
    {
      g_free (star->name);
      g_slice_free (AwStar, star);
    }
}

void
aw_star_list_free (GList *list)
{
  while (list)
    {
      aw_star_unref (list->data);
      list = g_list_delete_link (list, list);
    }
}

/* ================================================================= */

int
aw_star_compare_by_name (AwStar *star_a,
                         AwStar *star_b)
{
  const char *name_a = NULL;
  const char *name_b = NULL;

  if (star_a)
    name_a = aw_star_get_name (star_a);
  if (star_b)
    name_b = aw_star_get_name (star_b);

  return g_strcmp0 (name_a, name_b);
}

int
aw_star_compare_by_id (AwStar *star_a,
                       AwStar *star_b)
{
  int id_a = 0, id_b = 0;

  if (star_a)
    id_a = aw_star_get_id (star_a);
  if (star_b)
    id_b = aw_star_get_id (star_b);

  if (id_a != id_b)
    return (id_a < id_b ? -1 : +1);

  return aw_star_compare_by_name (star_a, star_b);
}

int
aw_star_compare_by_level (AwStar *star_a,
                          AwStar *star_b)
{
  int level_a = 0, level_b = 0;

  if (star_a)
    level_a = aw_star_get_level (star_a);
  if (star_b)
    level_b = aw_star_get_level (star_b);

  if (level_a != level_b)
    return (level_a < level_b ? +1 : -1);

  return aw_star_compare_by_name (star_a, star_b);
}

double
aw_star_get_distance (const AwStar *star_a,
                      const AwStar *star_b)
{
  const AwPoint *a = NULL;
  const AwPoint *b = NULL;

  if (star_a)
    a = aw_star_get_location (star_a);
  if (star_b)
    b = aw_star_get_location (star_b);

  return aw_point_get_distance (a, b);
}

