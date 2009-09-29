#include "config.h"
#include "building.h"

#include <glib/gi18n-lib.h>

G_CONST_RETURN char *
aw_building_get_display_name (AwBuildingType building,
                              int            amount)
{
  switch (building)
    {
    case AW_BUILDING_FARM:
      return dngettext (GETTEXT_PACKAGE, "Farm", "Farms", amount);
    case AW_BUILDING_FACTORY:
      return dngettext (GETTEXT_PACKAGE, "Factory", "Factories", amount);
    case AW_BUILDING_CYBERNET:
      return dngettext (GETTEXT_PACKAGE, "Cybernet", "Cybernets", amount);
    case AW_BUILDING_LABORATORY:
      return dngettext (GETTEXT_PACKAGE, "Laboratory", "Laboratories", amount);
    case AW_BUILDING_STARBASE:
      return dngettext (GETTEXT_PACKAGE, "Starbase", "Starbases", amount);
    case AW_BUILDING_INVALID:
    case AW_BUILDING_LAST:
      break;
    }

  return NULL;
}

