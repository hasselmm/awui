#include "config.h"
#include "bonus.h"

#include <glib/gi18n-lib.h>

G_CONST_RETURN char *
aw_bonus_get_display_name (AwBonusType id)
{
  switch (id)
    {
    case AW_BONUS_TRADE:
      return _("Trade Revenue");
    case AW_BONUS_GROWTH:
      return _("Growth");
    case AW_BONUS_SCIENCE:
      return _("Science");
    case AW_BONUS_CULTURE:
      return _("Culture");
    case AW_BONUS_PRODUCTION:
      return _("Production");
    case AW_BONUS_SPEED:
      return _("Speed");
    case AW_BONUS_ATTACK:
      return _("Attach");
    case AW_BONUS_DEFENSE:
      return _("Defense");
    case AW_BONUS_INVALID:
    case AW_BONUS_LAST:
      break;
    }

  g_return_val_if_reached (NULL);
}

