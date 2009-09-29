#ifndef __AW_MODEL_FLEET_H__
#define __AW_MODEL_FLEET_H__ 1

#include "vessel.h"

#define AW_TYPE_FLEET (aw_fleet_get_type ())

typedef enum {
  AW_FLEET_PENDING  = (1 << 0),
  AW_FLEET_SIEGING  = (1 << 1),
} AwFleetFlags;

typedef struct _AwFleet  AwFleet;

GType
aw_fleet_get_type          (void) G_GNUC_CONST;

AwFleet *
aw_fleet_new               (const char    *arrival_time,
                            const char    *arrival_date,
                            int            system_id,
                            const char    *planet_name,
                            AwFleetFlags   flags);

void
aw_fleet_set_vessels       (AwFleet       *fleet,
                            AwVesselType   first_vessel,
                                           ...);

AwFleetFlags
aw_fleet_get_flags         (const AwFleet *fleet);

G_CONST_RETURN char *
aw_fleet_get_arrival_time  (const AwFleet *fleet);

G_CONST_RETURN char *
aw_fleet_get_arrival_date  (const AwFleet *fleet);

int
aw_fleet_get_system_id     (const AwFleet *fleet);

G_CONST_RETURN char *
aw_fleet_get_system_name   (const AwFleet *fleet);

G_CONST_RETURN char *
aw_fleet_get_planet_name   (const AwFleet *fleet);

int
aw_fleet_get_vessels       (const AwFleet *fleet,
                            AwVesselType   type);

int
aw_fleet_get_transports    (const AwFleet *fleet);

int
aw_fleet_get_colonizers    (const AwFleet *fleet);

int
aw_fleet_get_destroyers    (const AwFleet *fleet);

int
aw_fleet_get_cruisers      (const AwFleet *fleet);

int
aw_fleet_get_battleships   (const AwFleet *fleet);

int
aw_fleet_get_attack_value  (const AwFleet *fleet);

int
aw_fleet_get_defense_value (const AwFleet *fleet);

int
aw_fleet_get_combat_value  (const AwFleet *fleet);

AwFleet *
aw_fleet_ref               (AwFleet       *fleet);

void
aw_fleet_unref             (AwFleet       *fleet);

void
aw_fleet_list_free         (GList         *list);

#endif /* __AW_MODEL_FLEET_H__ */

