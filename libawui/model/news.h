#ifndef __AW_MODEL_NEWS_H__
#define __AW_MODEL_NEWS_H__ 1

#include <glib-object.h>

#define AW_TYPE_NEWS (aw_news_get_type ())

typedef enum {
  AW_NEWS_DEFAULT   =       0,
  AW_NEWS_IMPORTANT = (1 << 0),
} AwNewsFlags;

typedef struct _AwNews  AwNews;

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

AwNews *
aw_news_ref       (AwNews       *news);

void
aw_news_unref     (AwNews       *news);

void
aw_news_list_free (GList        *list);

#endif /* __AW_MODEL_NEWS_H__ */
