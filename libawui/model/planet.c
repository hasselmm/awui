#include "config.h"
#include "planet.h"

#include <stdio.h>
#include <string.h>

struct _AwPlanet {
  int            ref_count;
  int            id;
  char          *name;
  char          *owner;
  AwPlanetFlags  flags;

  int            building_levels[AW_ITEM_BUILDINGS_LAST -
                                 AW_ITEM_BUILDINGS_FIRST + 1];
  int            production_points;
  int            population;
};

static gpointer
aw_planet_copy (gpointer boxed)
{
  return aw_planet_ref (boxed);
}

static void
aw_planet_free (gpointer boxed)
{
  aw_planet_unref (boxed);
}

GType
aw_planet_get_type  (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    type = g_boxed_type_register_static ("AwPlanet", aw_planet_copy, aw_planet_free);

  return type;
}

AwPlanet *
aw_planet_new (int            id,
               AwPlanetFlags  flags,
               const char    *name,
               const char    *owner,
               int            population)
{
  AwPlanet *planet;
  int       i;

  g_return_val_if_fail (NULL != name, NULL);

  planet = g_slice_new (AwPlanet);

  planet->ref_count         = 1;
  planet->id                = id;
  planet->flags             = flags;
  planet->name              = g_strdup (name);
  planet->owner             = g_strdup (owner);
  planet->population        = population;
  planet->production_points = -1;

  for (i = 0; i < G_N_ELEMENTS (planet->building_levels); ++i)
    planet->building_levels[i] = -1;

  return planet;
}

void
aw_planet_set_building_levels (AwPlanet   *planet,
                               AwItemType  first_type,
                                           ...)
{
  va_list    args;
  AwItemType type;

  g_return_if_fail (NULL != planet);

  va_start (args, first_type);

  for (type = first_type; AW_ITEM_INVALID != type;
       type = va_arg (args, AwItemType))
    {
      g_return_if_fail (aw_item_is_building (type));

      type -= AW_ITEM_BUILDINGS_FIRST;
      planet->building_levels[type] = va_arg (args, int);
    }

  va_end (args);
}

void
aw_planet_set_population (AwPlanet *planet,
                          int       population)
{
  g_return_if_fail (NULL != planet);
  planet->population = population;
}

void
aw_planet_set_production_points (AwPlanet *planet,
                                 int       points)
{
  g_return_if_fail (NULL != planet);
  planet->production_points = points;
}

int
aw_planet_get_id (const AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, -1);
  return planet->id;
}

G_CONST_RETURN char *
aw_planet_get_name (const AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, NULL);
  return planet->name;
}

G_CONST_RETURN char *
aw_planet_get_owner (const AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, NULL);
  return planet->owner;
}

AwPlanetFlags
aw_planet_get_flags (const AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, 0);
  return planet->flags;
}

/* ================================================================= */

int
aw_planet_get_population (const AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, -1);
  return planet->population;
}

int
aw_planet_get_production_points (const AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, -1);
  return planet->production_points;
}

int
aw_planet_get_building_level (const AwPlanet *planet,
                              AwItemType      type)
{
  g_return_val_if_fail (NULL != planet,             -1);
  g_return_val_if_fail (aw_item_is_building (type), -1);
  return planet->building_levels[type - AW_ITEM_BUILDINGS_FIRST];
}

int
aw_planet_get_farm_level (const AwPlanet *planet)
{
  return aw_planet_get_building_level (planet, AW_ITEM_FARM);
}

int
aw_planet_get_factory_level (const AwPlanet *planet)
{
  return aw_planet_get_building_level (planet, AW_ITEM_FACTORY);
}

int
aw_planet_get_cybernet_level (const AwPlanet *planet)
{
  return aw_planet_get_building_level (planet, AW_ITEM_CYBERNET);
}

int
aw_planet_get_laboratory_level (const AwPlanet *planet)
{
  return aw_planet_get_building_level (planet, AW_ITEM_LABORATORY);
}

int
aw_planet_get_starbase_level (const AwPlanet *planet)
{
  return aw_planet_get_building_level (planet, AW_ITEM_STARBASE);
}

int
aw_planet_get_growth_per_hour (const AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, -1);
  return MAX (0, aw_planet_get_farm_level (planet) + 1);
}

int
aw_planet_get_production_per_hour (const AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, -1);

  return MAX (0, aw_planet_get_population (planet)) +
         MAX (0, aw_planet_get_factory_level (planet));
}

/* ================================================================= */

AwPlanet *
aw_planet_ref (AwPlanet *planet)
{
  g_return_val_if_fail (NULL != planet, NULL);
  g_atomic_int_inc (&planet->ref_count);
  return planet;
}

void
aw_planet_unref (AwPlanet *planet)
{
  if (planet && g_atomic_int_dec_and_test (&planet->ref_count))
    {
      g_free (planet->owner);
      g_free (planet->name);

      g_slice_free (AwPlanet, planet);
    }
}

void
aw_planet_list_free (GList *list)
{
  while (list)
    {
      aw_planet_unref (list->data);
      list = g_list_delete_link (list, list);
    }
}

/* ================================================================= */

int
aw_planet_compare_by_name (AwPlanet *planet_a,
                           AwPlanet *planet_b)
{
  const char *name_a = NULL, *tail_a;
  const char *name_b = NULL, *tail_b;
  char       *key_a;
  char       *key_b;
  int         num_a;
  int         num_b;
  int         cmp;

  if (planet_a)
    name_a = aw_planet_get_name (planet_a);
  if (planet_b)
    name_b = aw_planet_get_name (planet_b);

  if (name_a == name_b)
    return 0;
  if (!name_a)
    return -1;
  if (!name_b)
    return +1;

  tail_a = strrchr (name_a, ' ');
  tail_b = strrchr (name_b, ' ');

  key_a = g_utf8_collate_key (name_a, tail_a - name_a);
  key_b = g_utf8_collate_key (name_b, tail_b - name_b);

  cmp = strcmp (key_a, key_b);

  g_free (key_a);
  g_free (key_b);

  if (0 != cmp ||
      1 != sscanf (tail_a, "%d", &num_a) ||
      1 != sscanf (tail_b, "%d", &num_b))
    return cmp;

  if (num_a < num_b)
    return -1;
  if (num_a > num_b)
    return +1;

  return 0;
}

int
aw_planet_compare_by_population (AwPlanet *planet_a,
                                 AwPlanet *planet_b)
{
  int a = 0, b = 0;

  if (planet_a)
    a = aw_planet_get_population (planet_a);
  if (planet_b)
    b = aw_planet_get_population (planet_b);

  if (a != b)
    return (a < b ? +1 : -1);

  return aw_planet_compare_by_name (planet_a, planet_b);
}

int
aw_planet_compare_by_starbase (AwPlanet *planet_a,
                               AwPlanet *planet_b)
{
  int a = 0, b = 0;

  if (planet_a)
    a = aw_planet_get_starbase_level (planet_a);
  if (planet_b)
    b = aw_planet_get_starbase_level (planet_b);

  if (a != b)
    return (a < b ? +1 : -1);

  return aw_planet_compare_by_name (planet_a, planet_b);
}

int
aw_planet_compare_by_owner (AwPlanet *planet_a,
                            AwPlanet *planet_b)
{
  const char *owner_a = NULL;
  const char *owner_b = NULL;
  int         result;

  if (planet_a)
    owner_a = aw_planet_get_owner (planet_a);
  if (planet_b)
    owner_b = aw_planet_get_owner (planet_b);

  if (0 != (result = g_strcmp0 (owner_a, owner_b)))
    return result;

  return aw_planet_compare_by_name (planet_a, planet_b);
}

