#ifndef __AW_SYSTEM_VIEW_H__
#define __AW_SYSTEM_VIEW_H__ 1

#include "planet-view.h"

#define AW_TYPE_SYSTEM_VIEW             (aw_system_view_get_type ())
#define AW_SYSTEM_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_SYSTEM_VIEW, AwSystemView))
#define AW_SYSTEM_VIEW_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_SYSTEM_VIEW, AwSystemViewClass))
#define AW_IS_SYSTEM_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_SYSTEM_VIEW))
#define AW_IS_SYSTEM_VIEW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_SYSTEM_VIEW))
#define AW_SYSTEM_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_SYSTEM_VIEW, AwSystemViewClass))

typedef struct _AwSystemView            AwSystemView;
typedef struct _AwSystemViewPrivate     AwSystemViewPrivate;
typedef struct _AwSystemViewClass       AwSystemViewClass;

struct _AwSystemView {
  AwPlanetView         parent;
  AwSystemViewPrivate *priv;
};

struct _AwSystemViewClass {
  AwPlanetViewClass parent_class;
};

GType
aw_system_view_get_type              (void) G_GNUC_CONST;

GtkWidget *
aw_system_view_new                   (void);

gboolean
aw_system_view_select_planet_by_name (AwSystemView *view,
                                      const char   *name);

#endif /* __AW_SYSTEM_VIEW_H__ */


