#ifndef __AW_WEB_PARSER_H__
#define __AW_WEB_PARSER_H__ 1

#include <libawui/model.h>

char *
aw_parser_reduce_markup  (const char     *data,
                          gssize          length,
                          GError        **error);

char **
aw_parser_find_links     (const char     *data,
                          gssize          length,
                          GError        **error);

gboolean
aw_parser_find_table     (const char     *data,
                          gssize          length,
                          const char     *title,
                          int            *start,
                          int            *end,
                          GError        **error);

GList *
aw_parser_read_table     (const char     *data,
                          gssize          length,
                          int             columns,
                          GError        **error);

GList *
aw_parser_read_news      (const char     *data,
                          gssize          length,
                          GError        **error);

GList *
aw_parser_read_map       (const char     *data,
                          gssize          length,
                          GError        **error);

GList *
aw_parser_read_system    (const char     *data,
                          gssize          length,
                          GError        **error);

GList *
aw_parser_read_planets   (const char     *data,
                          gssize          length,
                          GError        **error);

GList *
aw_parser_read_buildings (const char     *data,
                          gssize          length,
                          GError        **error);

GList *
aw_parser_read_sciences  (const char     *data,
                          gssize          length,
                          AwScienceId    *current,
                          AwScienceStats *stats,
                          GError        **error);

GList *
aw_parser_read_fleets    (const char     *data,
                          gssize          length,
                          GError        **error);

AwProfile *
aw_parser_read_profile   (const char     *data,
                          gssize          length,
                          int             profile_id,
                          GError        **error);

#endif /* __AW_WEB_PARSER_H__ */
