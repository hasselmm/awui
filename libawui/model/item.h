#ifndef __AW_MODEL_ITEM_H__
#define __AW_MODEL_ITEM_H__ 1

#include "profile.h"

typedef enum {
  AW_ITEM_INVALID = -1,

  AW_ITEM_FARM,
  AW_ITEM_FACTORY,
  AW_ITEM_CYBERNET,
  AW_ITEM_LABORATORY,
  AW_ITEM_STARBASE,

  AW_ITEM_TRANSPORT,
  AW_ITEM_COLONY_SHIP,
  AW_ITEM_DESTROYER,
  AW_ITEM_CRUISER,
  AW_ITEM_BATTLESHIP,

  AW_ITEM_TRADE,
  AW_ITEM_LAST,

  AW_ITEM_BUILDINGS_FIRST = AW_ITEM_FARM,
  AW_ITEM_BUILDINGS_LAST = AW_ITEM_STARBASE,
  AW_ITEM_VESSELS_FIRST = AW_ITEM_TRANSPORT,
  AW_ITEM_VESSELS_LAST = AW_ITEM_BATTLESHIP,
} AwItemType;

G_CONST_RETURN char *
aw_item_get_display_name  (AwItemType       item,
                           int              amount);

gboolean
aw_item_is_building       (AwItemType       item) G_GNUC_PURE;

gboolean
aw_item_is_vessel         (AwItemType       item) G_GNUC_PURE;

gboolean
aw_item_can_spend_all     (AwItemType       item) G_GNUC_PURE;

gboolean
aw_item_is_available      (AwItemType       item,
                           const AwProfile *profile) G_GNUC_PURE;

int
aw_item_get_attack_value  (AwItemType       item,
                           int              level);

int
aw_item_get_defense_value (AwItemType       item,
                           int              level);

int
aw_item_get_combat_value  (AwItemType       item,
                           int              level);

#endif /* __AW_MODEL_ITEM__H__ */

