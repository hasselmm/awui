#include "config.h"
#include "news.h"

struct _AwNewsPrivate {
  AwNewsFlags   flags;
  char         *date;
  char         *time;
  char         *text;
  char        **links;
};

G_DEFINE_TYPE (AwNews, aw_news, G_TYPE_OBJECT);

static void
aw_news_init (AwNews *news)
{
  news->priv = G_TYPE_INSTANCE_GET_PRIVATE (news, AW_TYPE_NEWS, AwNewsPrivate);
}

static void
aw_news_finalize (GObject *object)
{
  AwNews *news = AW_NEWS (object);

  g_free     (news->priv->date);
  g_free     (news->priv->time);
  g_free     (news->priv->text);
  g_strfreev (news->priv->links);

  G_OBJECT_CLASS (aw_news_parent_class)->finalize (object);
}

static void
aw_news_class_init (AwNewsClass *class)
{
  GObjectClass *object_class;

  object_class           = G_OBJECT_CLASS (class);
  object_class->finalize = aw_news_finalize;

  g_type_class_add_private (class, sizeof (AwNewsPrivate));
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

  news = g_object_new (AW_TYPE_NEWS, NULL);

  news->priv->date  = g_strdup  (date);
  news->priv->time  = g_strdup  (time);
  news->priv->text  = g_strdup  (text);
  news->priv->links = g_strdupv (links);
  news->priv->flags = flags;

  return news;
}

G_CONST_RETURN char *
aw_news_get_date (const AwNews *news)
{
  g_return_val_if_fail (AW_IS_NEWS (news), NULL);
  return news->priv->date;
}

G_CONST_RETURN char *
aw_news_get_time (const AwNews *news)
{
  g_return_val_if_fail (AW_IS_NEWS (news), NULL);
  return news->priv->time;
}

G_CONST_RETURN char *
aw_news_get_text (const AwNews *news)
{
  g_return_val_if_fail (AW_IS_NEWS (news), NULL);
  return news->priv->text;
}

char **
aw_news_get_links (const AwNews *news)
{
  g_return_val_if_fail (AW_IS_NEWS (news), NULL);
  return news->priv->links;
}

AwNewsFlags
aw_news_get_flags (const AwNews *news)
{
  g_return_val_if_fail (AW_IS_NEWS (news), 0);
  return news->priv->flags;
}

void
aw_news_list_free (GList *list)
{
  while (list)
    {
      g_object_unref (list->data);
      list = g_list_delete_link (list, list);
    }
}

