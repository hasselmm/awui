#include "config.h"
#include "news.h"

struct _AwNews {
  int           ref_count;
  AwNewsFlags   flags;
  char         *date;
  char         *time;
  char         *text;
  char        **links;
};

static gpointer
aw_news_copy (gpointer boxed)
{
  return aw_news_ref (boxed);
}

static void
aw_news_free (gpointer boxed)
{
  aw_news_unref (boxed);
}

GType
aw_news_get_type  (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    type = g_boxed_type_register_static ("AwNews", aw_news_copy, aw_news_free);

  return type;
}

AwNews *
aw_news_new (const char   *date,
             const char   *time,
             const char   *text,
             char        **links,
             AwNewsFlags   flags)
{
  AwNews *news;

  g_return_val_if_fail (NULL != date, NULL);
  g_return_val_if_fail (NULL != time, NULL);
  g_return_val_if_fail (NULL != text, NULL);

  news = g_slice_new (AwNews);

  news->ref_count = 1;
  news->date      = g_strdup  (date);
  news->time      = g_strdup  (time);
  news->text      = g_strdup  (text);
  news->links     = g_strdupv (links);
  news->flags     = flags;

  return news;
}

G_CONST_RETURN char *
aw_news_get_date (const AwNews *news)
{
  g_return_val_if_fail (NULL != news, NULL);
  return news->date;
}

G_CONST_RETURN char *
aw_news_get_time (const AwNews *news)
{
  g_return_val_if_fail (NULL != news, NULL);
  return news->time;
}

G_CONST_RETURN char *
aw_news_get_text (const AwNews *news)
{
  g_return_val_if_fail (NULL != news, NULL);
  return news->text;
}

char **
aw_news_get_links (const AwNews *news)
{
  g_return_val_if_fail (NULL != news, NULL);
  return (gpointer) news->links;
}

AwNewsFlags
aw_news_get_flags (const AwNews *news)
{
  g_return_val_if_fail (NULL != news, 0);
  return news->flags;
}

AwNews *
aw_news_ref (AwNews *news)
{
  g_return_val_if_fail (NULL != news, NULL);
  g_atomic_int_inc (&news->ref_count);
  return news;
}

void
aw_news_unref (AwNews *news)
{
  if (news && g_atomic_int_dec_and_test (&news->ref_count))
    {
      g_strfreev (news->links);

      g_free (news->text);
      g_free (news->time);
      g_free (news->date);

      g_slice_free (AwNews, news);
    }
}

void
aw_news_list_free (GList *list)
{
  while (list)
    {
      aw_news_unref (list->data);
      list = g_list_delete_link (list, list);
    }
}

