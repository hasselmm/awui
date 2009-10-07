#include "config.h"
#include "item.h"

#include <glib/gi18n-lib.h>

G_CONST_RETURN char *
aw_item_get_display_name (AwItemType item,
                          int        amount)
{
  switch (item)
    {
    case AW_ITEM_FARM:
      return g_dngettext (GETTEXT_PACKAGE, "Farm", "Farms", amount);
    case AW_ITEM_FACTORY:
      return g_dngettext (GETTEXT_PACKAGE, "Factory", "Factories", amount);
    case AW_ITEM_CYBERNET:
      return g_dngettext (GETTEXT_PACKAGE, "Cybernet", "Cybernets", amount);
    case AW_ITEM_LABORATORY:
      return g_dngettext (GETTEXT_PACKAGE, "Laboratory", "Laboratories", amount);
    case AW_ITEM_STARBASE:
      return g_dngettext (GETTEXT_PACKAGE, "Starbase", "Starbases", amount);

    case AW_ITEM_TRANSPORT:
      return g_dngettext (GETTEXT_PACKAGE, "Transport", "Transports", amount);
    case AW_ITEM_COLONY_SHIP:
      return g_dngettext (GETTEXT_PACKAGE, "Colony Ship", "Colony Ships", amount);
    case AW_ITEM_DESTROYER:
      return g_dngettext (GETTEXT_PACKAGE, "Destroyer", "Destroyers", amount);
    case AW_ITEM_CRUISER:
      return g_dngettext (GETTEXT_PACKAGE, "Cruiser", "Cruisers", amount);
    case AW_ITEM_BATTLESHIP:
      return g_dngettext (GETTEXT_PACKAGE, "Battleship", "Battleships", amount);

    case AW_ITEM_TRADE:
      return _("Interstellar Trade");

    case AW_ITEM_INVALID:
    case AW_ITEM_LAST:
      break;
    }

  return NULL;
}

gboolean
aw_item_is_building (AwItemType item)
{
  return (item >= AW_ITEM_BUILDINGS_FIRST &&
          item <= AW_ITEM_BUILDINGS_LAST);
}

gboolean
aw_item_is_vessel (AwItemType item)
{
  return (item >= AW_ITEM_VESSELS_FIRST &&
          item <= AW_ITEM_VESSELS_LAST);
}

gboolean
aw_item_can_spend_all (AwItemType item)
{
  switch (item)
    {
    case AW_ITEM_DESTROYER:
    case AW_ITEM_CRUISER:
    case AW_ITEM_BATTLESHIP:
    case AW_ITEM_TRADE:
      return TRUE;

    case AW_ITEM_FARM:
    case AW_ITEM_FACTORY:
    case AW_ITEM_CYBERNET:
    case AW_ITEM_LABORATORY:
    case AW_ITEM_STARBASE:
    case AW_ITEM_TRANSPORT:
    case AW_ITEM_COLONY_SHIP:
    case AW_ITEM_INVALID:
    case AW_ITEM_LAST:
      break;
    }

  return FALSE;
}

gboolean
aw_item_is_available (AwItemType       item,
                      const AwProfile *profile)
{
  g_return_val_if_fail (AW_IS_PROFILE (profile), FALSE);

  switch (item)
    {
    case AW_ITEM_CRUISER:
      return aw_profile_get_level (profile, AW_SCIENCE_MATHEMATICS) >= 15;
    case AW_ITEM_BATTLESHIP:
      return aw_profile_get_level (profile, AW_SCIENCE_PHYSICS) >= 15;
    case AW_ITEM_FARM:
    case AW_ITEM_FACTORY:
    case AW_ITEM_CYBERNET:
    case AW_ITEM_LABORATORY:
    case AW_ITEM_STARBASE:
    case AW_ITEM_TRANSPORT:
    case AW_ITEM_COLONY_SHIP:
    case AW_ITEM_DESTROYER:
    case AW_ITEM_TRADE:
    case AW_ITEM_INVALID:
    case AW_ITEM_LAST:
      break;
    }

  return TRUE;
}

int
aw_item_get_attack_value (AwItemType item,
                          int        item_count)
{
  switch (item)
    {
    case AW_ITEM_STARBASE:
    case AW_ITEM_TRANSPORT:
    case AW_ITEM_COLONY_SHIP:
      return 0;
    case AW_ITEM_DESTROYER:
      return 2 * item_count;
    case AW_ITEM_CRUISER:
      return 8 * item_count;
    case AW_ITEM_BATTLESHIP:
      return 36 * item_count;

    case AW_ITEM_INVALID:
    case AW_ITEM_FARM:
    case AW_ITEM_FACTORY:
    case AW_ITEM_CYBERNET:
    case AW_ITEM_LABORATORY:
    case AW_ITEM_TRADE:
    case AW_ITEM_LAST:
      break;
    }

  g_return_val_if_reached (-1);
}

int
aw_item_get_defense_value (AwItemType item,
                           int        item_count)
{
  switch (item)
    {
    case AW_ITEM_STARBASE:
      break; /* FIXME: build cost * factor */
    case AW_ITEM_TRANSPORT:
    case AW_ITEM_COLONY_SHIP:
      return 0;
    case AW_ITEM_DESTROYER:
      return 1 * item_count;
    case AW_ITEM_CRUISER:
      return 16 * item_count;
    case AW_ITEM_BATTLESHIP:
      return 24 * item_count;

    case AW_ITEM_INVALID:
    case AW_ITEM_FARM:
    case AW_ITEM_FACTORY:
    case AW_ITEM_CYBERNET:
    case AW_ITEM_LABORATORY:
    case AW_ITEM_TRADE:
    case AW_ITEM_LAST:
      break;
    }

  g_return_val_if_reached (-1);
}

int
aw_item_get_combat_value (AwItemType item,
                          int        item_count)
{
  return (aw_item_get_attack_value (item, item_count) +
          aw_item_get_defense_value (item, item_count));
}
