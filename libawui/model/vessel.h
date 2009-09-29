#ifndef __AW_MODEL_VESSEL_H__
#define __AW_MODEL_VESSEL_H__ 1

#include <glib-object.h>

typedef enum {
  AW_VESSEL_INVALID = -1,
  AW_VESSEL_TRANSPORT,
  AW_VESSEL_COLONY_SHIP,
  AW_VESSEL_DESTROYER,
  AW_VESSEL_CRUISER,
  AW_VESSEL_BATTLESHIP,
  AW_VESSEL_LAST,
} AwVesselType;

G_CONST_RETURN char *
aw_vessel_get_display_name  (AwVesselType vessel,
                             int          amount);

int
aw_vessel_get_attack_value  (AwVesselType vessel);

int
aw_vessel_get_defense_value (AwVesselType vessel);

int
aw_vessel_get_combat_value  (AwVesselType vessel);

#endif /* __AW_MODEL_VESSEL__H__ */


