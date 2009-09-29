#include "config.h"
#include "vessel.h"

#include <glib/gi18n-lib.h>

G_CONST_RETURN char *
aw_vessel_get_display_name (AwVesselType vessel,
                            int          amount)
{
  switch (vessel)
    {
    case AW_VESSEL_TRANSPORT:
      return dngettext (GETTEXT_PACKAGE, "Transport", "Transports", amount);
    case AW_VESSEL_COLONY_SHIP:
      return dngettext (GETTEXT_PACKAGE, "Colony Ship", "Colony Ships", amount);
    case AW_VESSEL_DESTROYER:
      return dngettext (GETTEXT_PACKAGE, "Destroyer", "Destroyers", amount);
    case AW_VESSEL_CRUISER:
      return dngettext (GETTEXT_PACKAGE, "Cruiser", "Cruisers", amount);
    case AW_VESSEL_BATTLESHIP:
      return dngettext (GETTEXT_PACKAGE, "Battleship", "Battleships", amount);
    case AW_VESSEL_INVALID:
    case AW_VESSEL_LAST:
      break;
    }

  return NULL;
}

int
aw_vessel_get_attack_value (AwVesselType vessel)
{
  switch (vessel)
    {
    case AW_VESSEL_TRANSPORT:
    case AW_VESSEL_COLONY_SHIP:
      return 0;
    case AW_VESSEL_DESTROYER:
      return 2;
    case AW_VESSEL_CRUISER:
      return 8;
    case AW_VESSEL_BATTLESHIP:
      return 36;
    case AW_VESSEL_INVALID:
    case AW_VESSEL_LAST:
      break;
    }

  g_warning ("type=%d\n", vessel);
  g_return_val_if_reached (-1);
}

int
aw_vessel_get_defense_value (AwVesselType vessel)
{
  switch (vessel)
    {
    case AW_VESSEL_TRANSPORT:
    case AW_VESSEL_COLONY_SHIP:
      return 0;
    case AW_VESSEL_DESTROYER:
      return 1;
    case AW_VESSEL_CRUISER:
      return 16;
    case AW_VESSEL_BATTLESHIP:
      return 24;
    case AW_VESSEL_INVALID:
    case AW_VESSEL_LAST:
      break;
    }

  g_return_val_if_reached (-1);
}

int
aw_vessel_get_combat_value (AwVesselType vessel)
{
  return aw_vessel_get_attack_value (vessel)
       + aw_vessel_get_defense_value (vessel);
}


