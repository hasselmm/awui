#ifndef __AW_MODEL_BONUS_H__
#define __AW_MODEL_BONUS_H__ 1

#include <glib-object.h>

typedef enum {
  AW_BONUS_INVALID = -1,
  AW_BONUS_TRADE,
  AW_BONUS_GROWTH,
  AW_BONUS_SCIENCE,
  AW_BONUS_CULTURE,
  AW_BONUS_PRODUCTION,
  AW_BONUS_SPEED,
  AW_BONUS_ATTACK,
  AW_BONUS_DEFENSE,
  AW_BONUS_LAST,
} AwBonusType;

G_CONST_RETURN char *
aw_bonus_get_display_name (AwBonusType id);

#endif /* __AW_MODEL_BONUS_H__ */
