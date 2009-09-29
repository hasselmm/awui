#ifndef __AW_MAP_VIEW_H__
#define __AW_MAP_VIEW_H__ 1

#include "tree-view.h"
#include <libawui/model.h>

#define AW_TYPE_MAP_VIEW             (aw_map_view_get_type ())
#define AW_MAP_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_MAP_VIEW, AwMapView))
#define AW_MAP_VIEW_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_MAP_VIEW, AwMapViewClass))
#define AW_IS_MAP_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_MAP_VIEW))
#define AW_IS_MAP_VIEW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_MAP_VIEW))
#define AW_MAP_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_MAP_VIEW, AwMapViewClass))

typedef struct _AwMapView            AwMapView;
typedef struct _AwMapViewPrivate     AwMapViewPrivate;
typedef struct _AwMapViewClass       AwMapViewClass;

struct _AwMapView {
  AwTreeView        parent;
  AwMapViewPrivate *priv;
};

struct _AwMapViewClass {
  AwTreeViewClass parent_class;
};

GType
aw_map_view_get_type   (void) G_GNUC_CONST;

GtkWidget *
aw_map_view_new        (void);

void
aw_map_view_set_origin (AwMapView *view,
                        AwStar    *star);

AwStar *
aw_map_view_get_origin (AwMapView *view);

#endif /* __AW_MAP_VIEW_H__ */

