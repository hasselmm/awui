#ifndef __AW_MAIN_VIEW_H__
#define __AW_MAIN_VIEW_H__ 1

#include <gtk/gtk.h>
#include <libawui/model.h>

#define AW_TYPE_MAIN_VIEW             (aw_main_view_get_type ())
#define AW_MAIN_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_MAIN_VIEW, AwMainView))
#define AW_MAIN_VIEW_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_MAIN_VIEW, AwMainViewClass))
#define AW_IS_MAIN_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_MAIN_VIEW))
#define AW_IS_MAIN_VIEW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_MAIN_VIEW))
#define AW_MAIN_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_MAIN_VIEW, AwMainViewClass))

typedef enum {
  AW_MAIN_VIEW_ACTION_INVALID = -1,
  AW_MAIN_VIEW_ACTION_GLOSSARY,
  AW_MAIN_VIEW_ACTION_NEWS,
  AW_MAIN_VIEW_ACTION_MAP,
  AW_MAIN_VIEW_ACTION_PLANETS,
  AW_MAIN_VIEW_ACTION_FLEET,
  AW_MAIN_VIEW_ACTION_TRADE,
  AW_MAIN_VIEW_ACTION_SCIENCE,
  AW_MAIN_VIEW_ACTION_ALLIANCE,
  AW_MAIN_VIEW_ACTION_LAST,
} AwMainViewAction;

typedef struct _AwMainView            AwMainView;
typedef struct _AwMainViewPrivate     AwMainViewPrivate;
typedef struct _AwMainViewClass       AwMainViewClass;

struct _AwMainView {
  GtkAlignment       parent;
  AwMainViewPrivate *priv;
};

struct _AwMainViewClass {
  GtkAlignmentClass parent_class;
};

GType
aw_main_view_get_type        (void) G_GNUC_CONST;

GtkWidget *
aw_main_view_new             (void);

void
aw_main_view_set_latest_news (AwMainView *view,
                              AwNews     *news);

AwNews *
aw_main_view_get_latest_news (AwMainView *view);

void
aw_main_view_set_profile     (AwMainView *view,
                              AwProfile  *profile);

AwProfile *
aw_main_view_get_profile     (AwMainView *view);

#endif /* __AW_MAIN_VIEW_H__ */

