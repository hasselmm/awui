#ifndef __AW_MODEL_PLANET_H__
#define __AW_MODEL_PLANET_H__ 1

#include "building.h"

#define AW_TYPE_PLANET (aw_planet_get_type ())

typedef enum {
  AW_PLANET_DEFAULT =       0,
  AW_PLANET_SIEGED  = (1 << 0),
} AwPlanetFlags;

typedef struct _AwPlanet  AwPlanet;

typedef int (*AwPlanetCompareFunc) (AwPlanet *planet_a,
                                    AwPlanet *planet_b);

GType
aw_planet_get_type                (void) G_GNUC_CONST;

AwPlanet *
aw_planet_new                     (int             id,
                                   AwPlanetFlags   flags,
                                   const char     *name,
                                   const char     *owner,
                                   int             population);

void
aw_planet_set_buildings           (AwPlanet       *planet,
                                   AwBuildingType  first_type,
                                                   ...);

void
aw_planet_set_population          (AwPlanet       *planet,
                                   int             population);

void
aw_planet_set_production_points   (AwPlanet       *planet,
                                   int             points);

int
aw_planet_get_id                  (const AwPlanet *planet);

AwPlanetFlags
aw_planet_get_flags               (const AwPlanet *planet);

G_CONST_RETURN char *
aw_planet_get_name                (const AwPlanet *planet);

G_CONST_RETURN char *
aw_planet_get_owner               (const AwPlanet *planet);

int
aw_planet_get_population          (const AwPlanet *planet);

int
aw_planet_get_production_points   (const AwPlanet *planet);

int
aw_planet_get_farm                (const AwPlanet *planet);

int
aw_planet_get_factory             (const AwPlanet *planet);

int
aw_planet_get_cybernet            (const AwPlanet *planet);

int
aw_planet_get_laboratory          (const AwPlanet *planet);

int
aw_planet_get_starbase            (const AwPlanet *planet);

int
aw_planet_get_growth_per_hour     (const AwPlanet *planet);

int
aw_planet_get_production_per_hour (const AwPlanet *planet);

AwPlanet *
aw_planet_ref                     (AwPlanet       *planet);

void
aw_planet_unref                   (AwPlanet       *planet);

void
aw_planet_list_free               (GList          *list);

int
aw_planet_compare_by_name         (AwPlanet       *planet_a,
                                   AwPlanet       *planet_b);

int
aw_planet_compare_by_population   (AwPlanet       *planet_a,
                                   AwPlanet       *planet_b);

int
aw_planet_compare_by_starbase     (AwPlanet       *planet_a,
                                   AwPlanet       *planet_b);

int
aw_planet_compare_by_owner        (AwPlanet       *planet_a,
                                   AwPlanet       *planet_b);

#endif /* __AW_MODEL_PLANET_H__ */
