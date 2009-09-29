#ifndef __AW_MODEL_BUILDING_H__
#define __AW_MODEL_BUILDING_H__ 1

#include <glib-object.h>

typedef enum {
  AW_BUILDING_INVALID = -1,
  AW_BUILDING_FARM,
  AW_BUILDING_FACTORY,
  AW_BUILDING_CYBERNET,
  AW_BUILDING_LABORATORY,
  AW_BUILDING_STARBASE,
  AW_BUILDING_LAST,
} AwBuildingType;

G_CONST_RETURN char *
aw_building_get_display_name (AwBuildingType building,
                             int             amount);

#endif /* __AW_MODEL_BUILDING__H__ */

