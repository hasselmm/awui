#ifndef __AW_SCIENCE_VIEW_H__
#define __AW_SCIENCE_VIEW_H__ 1

#include <hildon/hildon.h>
#include <libawui/model.h>

#define AW_TYPE_SCIENCE_VIEW             (aw_science_view_get_type ())
#define AW_SCIENCE_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_SCIENCE_VIEW, AwScienceView))
#define AW_SCIENCE_VIEW_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_SCIENCE_VIEW, AwScienceViewClass))
#define AW_IS_SCIENCE_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_SCIENCE_VIEW))
#define AW_IS_SCIENCE_VIEW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_SCIENCE_VIEW))
#define AW_SCIENCE_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_SCIENCE_VIEW, AwScienceViewClass))

typedef struct _AwScienceView            AwScienceView;
typedef struct _AwScienceViewClass       AwScienceViewClass;

GType
aw_science_view_get_type       (void) G_GNUC_CONST;

GtkWidget *
aw_science_view_new            (void);

void
aw_science_view_set_sciences   (AwScienceView        *view,
                                GList                *sciences);

void
aw_science_view_set_statistics (AwScienceView        *view,
                                const AwScienceStats *stats);

void
aw_science_view_set_science_id (AwScienceView        *view,
                                AwScienceId           id);

AwScienceId
aw_science_view_get_science_id (AwScienceView        *view);

G_CONST_RETURN AwScience *
aw_science_view_get_science    (AwScienceView        *view);


#endif /* __AW_SCIENCE_VIEW_H__ */


