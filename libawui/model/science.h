#ifndef __AW_MODEL_SCIENCE_H__
#define __AW_MODEL_SCIENCE_H__ 1

#include <glib-object.h>

#define AW_TYPE_SCIENCE (aw_science_get_type ())

typedef enum {
  AW_SCIENCE_INVALID = -1,
  AW_SCIENCE_BIOLOGY,
  AW_SCIENCE_ECONOMY,
  AW_SCIENCE_ENERGY,
  AW_SCIENCE_MATHEMATICS,
  AW_SCIENCE_PHYSICS,
  AW_SCIENCE_SOCIAL,
  AW_SCIENCE_CULTURE,
  AW_SCIENCE_LAST,
} AwScienceId;

typedef struct _AwScience      AwScience;
typedef struct _AwScienceStats AwScienceStats;

struct _AwScienceStats {
  int science_output;
  int science_bonus;

  int culture_output;
  int culture_bonus;
};

GType
aw_science_get_type            (void) G_GNUC_CONST;

AwScience *
aw_science_new                 (AwScienceId      id,
                                int              level,
                                int              remaining);

void
aw_science_set                 (AwScience       *science,
                                int              level,
                                int              remaining);

AwScienceId
aw_science_get_id              (const AwScience *science);

G_CONST_RETURN char *
aw_science_get_name            (const AwScience *science);

int
aw_science_get_level           (const AwScience *science);

int
aw_science_get_remaining       (const AwScience *science);

AwScience *
aw_science_ref                 (AwScience       *science);

void
aw_science_unref               (AwScience       *science);

void
aw_science_list_free           (GList           *list);

G_CONST_RETURN char *
aw_science_id_get_display_name (AwScienceId      id);

int
aw_science_get_research_cost   (AwScienceId      id,
                                int              level);

#endif /* __AW_MODEL_SCIENCE_H__ */
