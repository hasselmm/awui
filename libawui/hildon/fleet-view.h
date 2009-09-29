#ifndef __AW_FLEET_VIEW_H__
#define __AW_FLEET_VIEW_H__ 1

#include "tree-view.h"

#define AW_TYPE_FLEET_VIEW           (aw_fleet_view_get_type ())
#define AW_FLEET_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_FLEET_VIEW, AwFleetView))
#define AW_FLEET_VIEW_CLASS(cls)     (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_FLEET_VIEW, AwFleetViewClass))
#define AW_IS_FLEET_VIEW(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_FLEET_VIEW))
#define AW_IS_FLEET_VIEW_CLASS(obj)  (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_FLEET_VIEW))
#define AW_FLEET_VIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_FLEET_VIEW, AwFleetViewClass))

typedef struct _AwFleetView           AwFleetView;
typedef struct _AwFleetViewPrivate    AwFleetViewPrivate;
typedef struct _AwFleetViewClass      AwFleetViewClass;

struct _AwFleetView {
  AwTreeView           parent;
  AwFleetViewPrivate *priv;
};

struct _AwFleetViewClass {
  AwTreeViewClass parent_class;
};

GType
aw_fleet_view_get_type (void) G_GNUC_CONST;

GtkWidget *
aw_fleet_view_new      (void);

#endif /* __AW_FLEET_VIEW_H__ */

