#ifndef __AW_MODEL_NEWS_H__
#define __AW_MODEL_NEWS_H__ 1

#include <glib-object.h>

#define AW_TYPE_NEWS             (aw_news_get_type ())
#define AW_NEWS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_NEWS, AwNews))
#define AW_NEWS_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_NEWS, AwNewsClass))
#define AW_IS_NEWS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_NEWS))
#define AW_IS_NEWS_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_NEWS))
#define AW_NEWS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_NEWS, AwMainViewClass))

typedef enum {
  AW_NEWS_DEFAULT   =       0,
  AW_NEWS_IMPORTANT = (1 << 0),
} AwNewsFlags;

typedef struct _AwNews         AwNews;
typedef struct _AwNewsPrivate  AwNewsPrivate;
typedef struct _AwNewsClass    AwNewsClass;

struct _AwNews {
  GObject        parent;
  AwNewsPrivate *priv;
};

struct _AwNewsClass {
  GObjectClass parent_class;
};

GType
aw_news_get_type  (void) G_GNUC_CONST;

AwNews *
aw_news_new       (const char   *date,
                   const char   *time,
                   const char   *text,
                   char        **links,
                   AwNewsFlags   flags);

G_CONST_RETURN char *
aw_news_get_date  (const AwNews *news);

G_CONST_RETURN char *
aw_news_get_time  (const AwNews *news);

G_CONST_RETURN char *
aw_news_get_text  (const AwNews *news);

char **
aw_news_get_links (const AwNews *news);

AwNewsFlags
aw_news_get_flags (const AwNews *news);

void
aw_news_list_free (GList        *list);

#endif /* __AW_MODEL_NEWS_H__ */
