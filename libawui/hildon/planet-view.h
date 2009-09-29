#ifndef __AW_PLANET_VIEW_H__
#define __AW_PLANET_VIEW_H__ 1

#include "tree-view.h"

#define AW_TYPE_PLANET_VIEW           (aw_planet_view_get_type ())
#define AW_PLANET_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_PLANET_VIEW, AwPlanetView))
#define AW_PLANET_VIEW_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_PLANET_VIEW, AwPlanetViewClass))
#define AW_IS_PLANET_VIEW(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_PLANET_VIEW))
#define AW_IS_PLANET_VIEW_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_PLANET_VIEW))
#define AW_PLANET_VIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_PLANET_VIEW, AwPlanetViewClass))

typedef struct _AwPlanetView           AwPlanetView;
typedef struct _AwPlanetViewPrivate    AwPlanetViewPrivate;
typedef struct _AwPlanetViewClass      AwPlanetViewClass;

struct _AwPlanetView {
  AwTreeView           parent;
  AwPlanetViewPrivate *priv;
};

struct _AwPlanetViewClass {
  AwTreeViewClass parent_class;
};

GType
aw_planet_view_get_type            (void) G_GNUC_CONST;

GtkWidget *
aw_planet_view_new                 (void);

void
aw_planet_view_set_summary_visible (AwPlanetView *view,
                                    gboolean      visible);
  
gboolean
aw_planet_view_get_summary_visible (AwPlanetView *view);

#endif /* __AW_PLANET_VIEW_H__ */
