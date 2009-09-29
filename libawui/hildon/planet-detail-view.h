#ifndef __AW_PLANET_DETAIL_VIEW_H__
#define __AW_PLANET_DETAIL_VIEW_H__ 1

#include <hildon/hildon.h>
#include <libawui/model.h>

#define AW_TYPE_PLANET_DETAIL_VIEW             (aw_planet_detail_view_get_type ())
#define AW_PLANET_DETAIL_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_PLANET_DETAIL_VIEW, AwPlanetDetailView))
#define AW_PLANET_DETAIL_VIEW_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_PLANET_DETAIL_VIEW, AwPlanetDetailViewClass))
#define AW_IS_PLANET_DETAIL_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_PLANET_DETAIL_VIEW))
#define AW_IS_PLANET_DETAIL_VIEW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_PLANET_DETAIL_VIEW))
#define AW_PLANET_DETAIL_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_PLANET_DETAIL_VIEW, AwPlanetDetailViewClass))

typedef struct _AwPlanetDetailView            AwPlanetDetailView;
typedef struct _AwPlanetDetailViewPrivate     AwPlanetDetailViewPrivate;
typedef struct _AwPlanetDetailViewClass       AwPlanetDetailViewClass;

struct _AwPlanetDetailView {
  HildonStackableWindow      parent;
  AwPlanetDetailViewPrivate *priv;
};

struct _AwPlanetDetailViewClass {
  HildonStackableWindowClass parent_class;
};

GType
aw_planet_detail_view_get_type   (void) G_GNUC_CONST;

GtkWidget *
aw_planet_detail_view_new        (AwPlanet           *planet);

void
aw_planet_detail_view_set_planet (AwPlanetDetailView *view,
                                  AwPlanet           *planet);

AwPlanet *
aw_planet_detail_view_get_planet (AwPlanetDetailView *view);

#endif /* __AW_PLANET_DETAIL_VIEW_H__ */

