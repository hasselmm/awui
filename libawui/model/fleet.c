#include "config.h"
#include "fleet.h"

#include <string.h>

struct _AwFleet {
  int           ref_count;
  AwFleetFlags  flags;
  char         *arrival_time;
  char         *arrival_date;
  char         *planet_name;
  char         *system_name;
  int           system_id;
  int           vessels[AW_ITEM_VESSELS_LAST -
                        AW_ITEM_VESSELS_FIRST + 1];
};

static gpointer
aw_fleet_copy (gpointer boxed)
{
  return aw_fleet_ref (boxed);
}

static void
aw_fleet_free (gpointer boxed)
{
  aw_fleet_unref (boxed);
}

GType
aw_fleet_get_type  (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    type = g_boxed_type_register_static ("AwFleet", aw_fleet_copy, aw_fleet_free);

  return type;
}

AwFleet *
aw_fleet_new (const char    *arrival_time,
              const char    *arrival_date,
              int            system_id,
              const char    *planet_name,
              AwFleetFlags   flags)
{
  AwFleet    *fleet;
  const char *end;
  int         i;

  g_return_val_if_fail (NULL != planet_name, NULL);

  fleet = g_slice_new (AwFleet);

  fleet->arrival_time = g_strdup (arrival_time);
  fleet->arrival_date = g_strdup (arrival_date);
  fleet->planet_name  = g_strdup (planet_name);
  fleet->system_name  = NULL;
  fleet->system_id    = system_id;
  fleet->flags        = flags;
  fleet->ref_count    = 1;

  for (i = 0; i < G_N_ELEMENTS (fleet->vessels); ++i)
    fleet->vessels[i] = -1;

  // FIXME: avoid this ad-hoc code
  end = strrchr (fleet->planet_name, ' ');

  if (end)
    fleet->system_name = g_strndup (fleet->planet_name,
                                    end - fleet->planet_name);

  return fleet;
}

void
aw_fleet_set_vessel_counts (AwFleet    *fleet,
                            AwItemType  first_type,
                                        ...)
{
  va_list    args;
  AwItemType type;

  g_return_if_fail (NULL != fleet);

  va_start (args, first_type);

  for (type = first_type; AW_ITEM_INVALID != type;
       type = va_arg (args, AwItemType))
    {
      g_return_if_fail (aw_item_is_vessel (type));

      type -= AW_ITEM_VESSELS_FIRST;
      fleet->vessels[type] = va_arg (args, int);
    }

  va_end (args);
}

G_CONST_RETURN char *
aw_fleet_get_arrival_time (const AwFleet *fleet)
{
  g_return_val_if_fail (NULL != fleet, NULL);
  return fleet->arrival_time;
}

G_CONST_RETURN char *
aw_fleet_get_arrival_date (const AwFleet *fleet)
{
  g_return_val_if_fail (NULL != fleet, NULL);
  return fleet->arrival_date;
}

G_CONST_RETURN char *
aw_fleet_get_system_name (const AwFleet *fleet)
{
  g_return_val_if_fail (NULL != fleet, NULL);
  return fleet->system_name;
}

G_CONST_RETURN char *
aw_fleet_get_planet_name (const AwFleet *fleet)
{
  g_return_val_if_fail (NULL != fleet, NULL);
  return fleet->planet_name;
}

int
aw_fleet_get_system_id (const AwFleet *fleet)
{
  g_return_val_if_fail (NULL != fleet, -1);
  return fleet->system_id;
}

AwFleetFlags
aw_fleet_get_flags (const AwFleet *fleet)
{
  g_return_val_if_fail (NULL != fleet, 0);
  return fleet->flags;
}

/* ================================================================= */

int
aw_fleet_get_vessel_count (const AwFleet *fleet,
                           AwItemType     type)
{
  g_return_val_if_fail (NULL != fleet,            -1);
  g_return_val_if_fail (aw_item_is_vessel (type), -1);
  return fleet->vessels[type - AW_ITEM_VESSELS_FIRST];
}

int
aw_fleet_get_transports (const AwFleet *fleet)
{
  return aw_fleet_get_vessel_count (fleet, AW_ITEM_TRANSPORT);
}

int
aw_fleet_get_colonizers (const AwFleet *fleet)
{
  return aw_fleet_get_vessel_count (fleet, AW_ITEM_COLONY_SHIP);
}

int
aw_fleet_get_destroyers (const AwFleet *fleet)
{
  return aw_fleet_get_vessel_count (fleet, AW_ITEM_DESTROYER);
}

int
aw_fleet_get_cruisers (const AwFleet *fleet)
{
  return aw_fleet_get_vessel_count (fleet, AW_ITEM_CRUISER);
}

int
aw_fleet_get_battleships (const AwFleet *fleet)
{
  return aw_fleet_get_vessel_count (fleet, AW_ITEM_BATTLESHIP);
}

int
aw_fleet_get_attack_value (const AwFleet *fleet)
{
  int        count, value = 0;
  AwItemType type;

  g_return_val_if_fail (NULL != fleet, -1);

  for (type = AW_ITEM_VESSELS_FIRST; type <= AW_ITEM_VESSELS_LAST; ++type)
    {
      count  = aw_fleet_get_vessel_count (fleet, type);
      value += aw_item_get_attack_value (type, count);
    }

  return value;
}

int
aw_fleet_get_defense_value (const AwFleet *fleet)
{
  int        count, value = 0;
  AwItemType type;

  g_return_val_if_fail (NULL != fleet, -1);

  for (type = AW_ITEM_VESSELS_FIRST; type <= AW_ITEM_VESSELS_LAST; ++type)
    {
      count  = aw_fleet_get_vessel_count (fleet, type);
      value += aw_item_get_defense_value (type, count);
    }

  return value;
}

int
aw_fleet_get_combat_value (const AwFleet *fleet)
{
  g_return_val_if_fail (NULL != fleet, -1);

  return aw_fleet_get_attack_value (fleet) +
         aw_fleet_get_defense_value (fleet);
}

/* ================================================================= */

AwFleet *
aw_fleet_ref (AwFleet *fleet)
{
  g_return_val_if_fail (NULL != fleet, NULL);
  g_atomic_int_inc (&fleet->ref_count);
  return fleet;
}

void
aw_fleet_unref (AwFleet *fleet)
{
  if (fleet && g_atomic_int_dec_and_test (&fleet->ref_count))
    {
      g_free (fleet->system_name);
      g_free (fleet->planet_name);
      g_free (fleet->arrival_time);
      g_free (fleet->arrival_date);

      g_slice_free (AwFleet, fleet);
    }
}

void
aw_fleet_list_free (GList *list)
{
  while (list)
    {
      aw_fleet_unref (list->data);
      list = g_list_delete_link (list, list);
    }
}
