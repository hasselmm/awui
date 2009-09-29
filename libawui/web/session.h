#ifndef __AW_WEB_SESSION_H__
#define __AW_WEB_SESSION_H__ 1

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libawui/model.h>

#define AW_SESSION_ERROR                (aw_session_error_quark ())

#define AW_TYPE_SESSION                 (aw_session_get_type ())
#define AW_SESSION(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_SESSION, AwSession))
#define AW_SESSION_CLASS(cls)           (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_SESSION, AwSessionClass))
#define AW_IS_SESSION(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_SESSION))
#define AW_IS_SESSION_CLASS(obj)        (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_SESSION))
#define AW_SESSION_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_SESSION, AwSessionClass))

typedef struct _AwSession                AwSession;
typedef struct _AwSessionClass           AwSessionClass;

typedef enum {
  AW_SESSION_ERROR_NONE,
  AW_SESSION_ERROR_REQUEST_FAILED,
} AwSessionError;

GQuark
aw_session_error_quark            (void) G_GNUC_CONST;

GType
aw_session_get_type               (void) G_GNUC_CONST;

AwSession *
aw_session_new                    (void);

AwSession *
aw_session_get_singleton          (void);

void
aw_session_login_async            (AwSession          *session,
                                   const char         *username,
                                   const char         *password,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

gboolean
aw_session_login_finish           (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_fetch_captcha_async    (AwSession          *session,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

GdkPixbuf *
aw_session_fetch_captcha_finish   (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_solve_captcha_async    (AwSession          *session,
                                   const char         *response,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

gboolean
aw_session_solve_captcha_finish   (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_fetch_news_async       (AwSession          *session,
                                   int                 page,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

GList *
aw_session_fetch_news_finish      (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_fetch_map_async        (AwSession          *session,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

GList *
aw_session_fetch_map_finish       (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_fetch_system_async     (AwSession          *session,
                                   int                 system_id,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

GList *
aw_session_fetch_system_finish    (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_fetch_planets_async    (AwSession          *session,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

GList *
aw_session_fetch_planets_finish   (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_fetch_buildings_async  (AwSession          *session,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

GList *
aw_session_fetch_buildings_finish (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_fetch_science_async    (AwSession          *session,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

GList *
aw_session_fetch_science_finish   (AwSession          *session,
                                   GAsyncResult       *result,
                                   AwScienceId        *current,
                                   AwScienceStats     *stats,
                                   GError            **error);

void
aw_session_fetch_fleets_async     (AwSession          *session,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

GList *
aw_session_fetch_fleets_finish    (AwSession          *session,
                                   GAsyncResult       *result,
                                   GError            **error);

void
aw_session_select_science_async   (AwSession          *session,
                                   AwScienceId         science_id,
                                   GAsyncReadyCallback callback,
                                   gpointer            user_data);

#endif /* __AW_WEB_SESSION_H__ */
