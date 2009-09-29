#ifndef __AW_NEWS_VIEW_H__
#define __AW_NEWS_VIEW_H__ 1

#include "tree-view.h"

#define AW_TYPE_NEWS_VIEW             (aw_news_view_get_type ())
#define AW_NEWS_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_NEWS_VIEW, AwNewsView))
#define AW_NEWS_VIEW_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_NEWS_VIEW, AwNewsViewClass))
#define AW_IS_NEWS_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_NEWS_VIEW))
#define AW_IS_NEWS_VIEW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_NEWS_VIEW))
#define AW_NEWS_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_NEWS_VIEW, AwNewsViewClass))

typedef struct _AwNewsView            AwNewsView;
typedef struct _AwNewsViewPrivate     AwNewsViewPrivate;
typedef struct _AwNewsViewClass       AwNewsViewClass;

struct _AwNewsView {
  AwTreeView         parent;
  AwNewsViewPrivate *priv;
};

struct _AwNewsViewClass {
  AwTreeViewClass parent_class;
};

GType
aw_news_view_get_type    (void) G_GNUC_CONST;

GtkWidget *
aw_news_view_new         (void);

#endif /* __AW_NEWS_VIEW_H__ */

