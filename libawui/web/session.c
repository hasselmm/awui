#include "config.h"
#include "session.h"
#include "parser.h"

#include <glib/gi18n-lib.h>
#include <libawui/model.h>
#include <libsoup/soup.h>

#if 0
#define AW_DEBUG_REF_COUNT(obj) G_STMT_START                                    \
{                                                                               \
  GObject *tmp = (GObject *) (obj);                                             \
  g_print ("%s:%d: rc(%s)=%d\n", G_STRFUNC, __LINE__,                           \
           #obj, tmp ? tmp->ref_count : -1);                                    \
}                                                                               \
G_STMT_END
#else
#define AW_DEBUG_REF_COUNT(obj)
#endif

enum {
  LOGIN_REQUIRED,
  CAPTCHA_REQUIRED,
  LAST_SIGNAL,
};

typedef GList * (* AwSessionParseItemsFunc) (const char  *data,
                                             gssize       length,
                                             GError     **error);

typedef void    (* AwSessionFreeItemsFunc)  (GList       *items);

/* ================================================================= */
/* === GObject boilerplate                                       === */
/* ================================================================= */

struct _AwSession {
  GObject         parent;
  char           *base_uri;
  SoupSession    *client;
  SoupCookieJar  *cookies;
  GQueue          postponed;
};

struct _AwSessionClass {
  GObjectClass parent_class;
};

static unsigned   signals[LAST_SIGNAL] = { 0, };
static AwSession *singleton = NULL;


G_DEFINE_TYPE (AwSession, aw_session, G_TYPE_OBJECT);

GQuark
aw_session_error_quark (void)
{
  static GQuark quark = 0;

  if (G_UNLIKELY (!quark))
    quark = g_quark_from_static_string ("aw-session-error-quark");

  return quark;
}

static void
aw_session_init (AwSession *session)
{
  char *cookies;

  session->base_uri = g_strdup_printf ("http://www1.astrowars.com/");
  session->cookies  = soup_cookie_jar_new ();

  session->client = soup_session_async_new_with_options
    (SOUP_SESSION_USER_AGENT, PACKAGE "/" VERSION " ",
     SOUP_SESSION_ADD_FEATURE, session->cookies, NULL);

  g_object_get (aw_settings_get_singleton (),
                "session-cookies", &cookies, NULL);

  if (cookies)
    {
      SoupURI *uri = soup_uri_new (session->base_uri);
      soup_cookie_jar_set_cookie (session->cookies, uri, cookies);
      soup_uri_free (uri);
      g_free (cookies);
    }
}

static GObject *
aw_session_constructor (GType                  type,
                        unsigned               n_params,
                        GObjectConstructParam *params)
{
  GObjectClass *object_parent_class;
  GObject      *object;

  if (G_UNLIKELY (!singleton))
    {
      object_parent_class = G_OBJECT_CLASS (aw_session_parent_class);
      object = object_parent_class->constructor (type, n_params, params);
      g_object_add_weak_pointer (object, (gpointer) &singleton);
      singleton = AW_SESSION (object);
    }
  else
    object = g_object_ref (singleton);

  return object;
}

static char *
aw_session_get_cookies (AwSession *session)
{
  char    *cookies = NULL;
  SoupURI *uri;

  if (session->cookies)
    {
      uri = soup_uri_new (session->base_uri);
      cookies = soup_cookie_jar_get_cookies (session->cookies, uri, TRUE);
      soup_uri_free (uri);
    }

  return cookies;
}

static void
aw_settion_save_cookies (AwSession *session)
{
  char *cookies = aw_session_get_cookies (session);

  if (cookies)
    {
      g_object_set (aw_settings_get_singleton (),
                    "session-cookies", cookies, NULL);
      g_free (cookies);
    }
}

static void
aw_session_dispose (GObject *object)
{
  AwSession *session = AW_SESSION (object);

  if (session->cookies)
    {
      aw_settion_save_cookies (session);
      g_object_unref (session->cookies);
      session->cookies = NULL;
    }

  if (session->client)
    {
      g_object_unref (session->client);
      session->client = NULL;
    }

  g_queue_foreach (&session->postponed, (GFunc) g_object_unref, NULL);
  g_queue_free (&session->postponed);

  G_OBJECT_CLASS (aw_session_parent_class)->dispose (object);
}

static void
aw_session_finalize (GObject *object)
{
  AwSession *session = AW_SESSION (object);

  g_free (session->base_uri);

  G_OBJECT_CLASS (aw_session_parent_class)->finalize (object);
}

static void
aw_session_class_init (AwSessionClass *class)
{
  GObjectClass *object_class;

  object_class               = G_OBJECT_CLASS (class);
  object_class->constructor  = aw_session_constructor;
  object_class->dispose      = aw_session_dispose;
  object_class->finalize     = aw_session_finalize;

  signals[LOGIN_REQUIRED] = g_signal_new
    ("login-required", AW_TYPE_SESSION,
     G_SIGNAL_RUN_LAST, 0, NULL, NULL,
     g_cclosure_marshal_VOID__VOID,
     G_TYPE_NONE, 0);

  signals[CAPTCHA_REQUIRED] = g_signal_new
    ("captcha-required", AW_TYPE_SESSION,
     G_SIGNAL_RUN_LAST, 0, NULL, NULL,
     g_cclosure_marshal_VOID__VOID,
     G_TYPE_NONE, 0);
}

AwSession *
aw_session_new (void)
{
  return g_object_new (AW_TYPE_SESSION, NULL);
}

AwSession *
aw_session_get_singleton (void)
{
  g_warn_if_fail (AW_IS_SESSION (singleton));
  return singleton;
}

/* ================================================================= */
/* === Methods for starting async requests                       === */
/* ================================================================= */

static void
aw_session_postpone (AwSession    *session,
                     SoupMessage  *message,
                     GAsyncResult *result)
{
  g_object_set_data_full (G_OBJECT (message), "async-result",
                          g_object_ref (result), g_object_unref);
  g_queue_push_tail (&session->postponed, g_object_ref (message));
}

static void
aw_session_message_cb (SoupSession *client,
                       SoupMessage *message,
                       gpointer     result)
{
  GObject   *source  = g_async_result_get_source_object (result);
  AwSession *session = AW_SESSION (source);
  char      *uri;

AW_DEBUG_REF_COUNT (session);

  uri = soup_uri_to_string (soup_message_get_uri (message), FALSE);
  g_print ("%s: %d %s (%s %s)\n", G_STRFUNC, message->status_code,
           message->reason_phrase, message->method, uri);
  g_free (uri);

  if (SOUP_STATUS_OK != message->status_code)
    {
AW_DEBUG_REF_COUNT (session);

      g_simple_async_result_set_error (result, AW_SESSION_ERROR,
                                       AW_SESSION_ERROR_REQUEST_FAILED,
                                       _("Request failed: %s"),
                                       message->reason_phrase);

      g_simple_async_result_complete_in_idle (result);
    }
  else if (g_strstr_len (message->response_body->data,
                         message->response_body->length,
                         "<b>Please Login Again.</b>"))
    {
AW_DEBUG_REF_COUNT (session);
      aw_session_postpone (session, message, result);
AW_DEBUG_REF_COUNT (session);
      g_signal_emit (session, signals[LOGIN_REQUIRED], 0);
AW_DEBUG_REF_COUNT (session);
    }
  else if (g_strstr_len (message->response_body->data,
                         message->response_body->length,
                         "<b>Security Measure</b>"))
    {
      aw_session_postpone (session, message, result);
      g_signal_emit (session, signals[CAPTCHA_REQUIRED], 0);
    }
  else
    {
AW_DEBUG_REF_COUNT (session);
      g_simple_async_result_set_op_res_gpointer (result, g_object_ref (message), g_object_unref);
      g_simple_async_result_complete_in_idle (result);
AW_DEBUG_REF_COUNT (session);
    }

AW_DEBUG_REF_COUNT (result);
AW_DEBUG_REF_COUNT (session);
   g_object_unref (session);
//   g_object_unref (result);
AW_DEBUG_REF_COUNT (session);
}

static void
aw_session_run_postponed (AwSession *session)
{
  SoupMessage        *message;
  GSimpleAsyncResult *result;

  message = g_queue_pop_tail (&session->postponed);

  if (message)
    {
      result = g_object_get_data (G_OBJECT (message), "async-result");
g_print ("%s: %s\n", G_STRFUNC, soup_message_get_uri (message)->path);
AW_DEBUG_REF_COUNT (session);

static int rc = 0; g_assert (rc++ < 300); // FIXME
      soup_session_queue_message (session->client, message,
                                  aw_session_message_cb, result);
    }
}

static void
aw_session_queue_message (AwSession           *session,
                          SoupMessage         *message,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data,
                          gpointer             source_tag)
{
  GSimpleAsyncResult *result;
static int rc = 0; g_assert (rc++ < 300); // FIXME

AW_DEBUG_REF_COUNT (session);
  result = g_simple_async_result_new (G_OBJECT (session),
                                      callback, user_data, source_tag);
AW_DEBUG_REF_COUNT (session);

  soup_session_queue_message (session->client, message,
                              aw_session_message_cb, result);
AW_DEBUG_REF_COUNT (session);
}

static void
aw_session_fetch_async (AwSession            *session,
                        GAsyncReadyCallback   callback,
                        gpointer              user_data,
                        gpointer              source_tag,
                        const char           *path,
                                              ...)
{
  SoupMessage *message;
  va_list      args;
  GString     *uri;

  g_return_if_fail (AW_IS_SESSION (session));
  g_return_if_fail (NULL != source_tag);

  if ('/' == *path)
    ++path;

  va_start (args, path);
  uri = g_string_new (session->base_uri);
  g_string_append_vprintf (uri, path, args);
  message = soup_message_new (SOUP_METHOD_GET, uri->str);
  g_string_free (uri, TRUE);
  va_end (args);

AW_DEBUG_REF_COUNT (session);
  aw_session_queue_message (session, message, callback, user_data, source_tag);
}

static void
aw_session_post_async (AwSession            *session,
                       GAsyncReadyCallback   callback,
                       gpointer              user_data,
                       gpointer              source_tag,
                       const char           *path,
                       const char           *first_first,
                                              ...)
{
  SoupMessage *message;
  char        *request;
  va_list      args;
  char        *uri;

  g_return_if_fail (AW_IS_SESSION (session));
  g_return_if_fail (NULL != source_tag);

  if ('/' == *path)
    ++path;

  va_start (args, first_first);
  request = soup_form_encode_valist (first_first, args);
  va_end (args);

  uri = g_strconcat (session->base_uri, path, NULL);
  message = soup_message_new (SOUP_METHOD_POST, uri);
  g_free (uri);

  soup_message_set_request (message,
                            "application/x-www-form-urlencoded",
                            SOUP_MEMORY_TAKE, request, strlen (request));

AW_DEBUG_REF_COUNT (session);
  aw_session_queue_message (session, message, callback, user_data, source_tag);
}

void
aw_session_login_async (AwSession           *session,
                        const char          *username,
                        const char          *password,
                        GAsyncReadyCallback  callback,
                        gpointer             user_data)
{
  SoupURI     *redirect = NULL;
  SoupMessage *postponed;

  g_return_if_fail (AW_IS_SESSION (session));
  g_return_if_fail (NULL != username);
  g_return_if_fail (NULL != password);

  postponed = g_queue_peek_tail (&session->postponed);

  if (postponed)
    redirect = soup_message_get_uri (postponed);

  aw_session_post_async (session, callback, user_data,
                         aw_session_login_async, "/register/login.php",
                         "redirect", redirect ? redirect->path : "/0/News/",
                         "user", username, "passwort", password,
                         "submit2",  "value", NULL);
}

void
aw_session_solve_captcha_async (AwSession           *session,
                                const char          *response,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  char        *normalized_response = NULL;
  SoupURI     *redirect = NULL;
  SoupMessage *postponed;

  g_return_if_fail (AW_IS_SESSION (session));
  g_return_if_fail (NULL != response);

  postponed = g_queue_peek_tail (&session->postponed);

  if (postponed)
    redirect = soup_message_get_uri (postponed);

  normalized_response = g_strstrip (g_utf8_strdown (response, -1));

  aw_session_post_async (session, callback, user_data,
                         aw_session_solve_captcha_async,
                         redirect ? redirect->path : "/0/News/",
                         "secure", normalized_response,
                         "submit2", "submit", NULL);

  g_free (normalized_response);
}

void
aw_session_fetch_captcha_async (AwSession           *session,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_captcha_async,
                          "/0/secure.php");
}

void
aw_session_fetch_news_async (AwSession           *session,
                             int                  page,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_news_async,
                          "/0/News/?p=%d", page);
}

void
aw_session_fetch_map_async (AwSession           *session,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data)
{
  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_map_async,
                          "/0/Map/");
}

void
aw_session_fetch_system_async (AwSession          *session,
                               int                 system_id,
                               GAsyncReadyCallback callback,
                               gpointer            user_data)
{
  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_system_async,
                          "/0/Map/Detail.php/?nr=%d",
                          system_id);
}

void
aw_session_fetch_planets_async (AwSession           *session,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_planets_async,
                          "/0/Planets/");
}

void
aw_session_fetch_buildings_async (AwSession           *session,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_buildings_async,
                          "/0/Planets/Buildings.php");
}

void
aw_session_fetch_science_async (AwSession           *session,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_science_async,
                          "/0/Science/");
}

void
aw_session_fetch_fleets_async (AwSession           *session,
                               GAsyncReadyCallback  callback,
                               gpointer             user_data)
{
  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_fleets_async,
                          "/0/Fleet/");
}

/* ================================================================= */
/* === Methods for finishing async calls                         === */
/* ================================================================= */

static SoupMessage *
aw_session_finish_message (AwSession            *session,
                           GAsyncResult         *result,
                           gpointer              source_tag,
                           GError              **error)
{
  SoupMessage        *message = NULL;
  GSimpleAsyncResult *simple_result;

  g_return_val_if_fail (AW_IS_SESSION (session),           FALSE);
  g_return_val_if_fail (G_IS_SIMPLE_ASYNC_RESULT (result), FALSE);

  simple_result = G_SIMPLE_ASYNC_RESULT (result);

  g_return_val_if_fail
    (g_simple_async_result_get_source_tag (simple_result) ==
     source_tag, FALSE);

  if (!g_simple_async_result_propagate_error (simple_result, error))
    message = g_simple_async_result_get_op_res_gpointer (simple_result);

  return message;
}

gboolean
aw_session_login_finish (AwSession     *session,
                         GAsyncResult  *result,
                         GError       **error)
{
  gboolean     success = FALSE;
  char        *cookies = NULL;
  SoupMessage *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_login_async,
                                       error);

  if (NULL != message &&
      NULL == g_strstr_len (message->response_body->data,
                            message->response_body->length,
                            "<b>Incorrect password</b>"))
    {
      if (session->cookies)
        cookies = aw_session_get_cookies (session);

      if (cookies)
        {
          int         profile_id;
          const char *login;

          login = strstr (cookies, "login=");

          if (login && 1 == sscanf (login, "login=%d", &profile_id))
            {
              g_object_set (aw_settings_get_singleton (),
                            "profile-id", profile_id, NULL);
            }

          g_free (cookies);
        }

      aw_session_run_postponed (session);
      success = TRUE;
    }

  return success;
}

gboolean
aw_session_solve_captcha_finish (AwSession     *session,
                                 GAsyncResult  *result,
                                 GError       **error)
{
  gboolean     success = FALSE;
  SoupMessage *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_solve_captcha_async,
                                       error);

  if (NULL != message &&
      NULL == g_strstr_len (message->response_body->data,
                            message->response_body->length,
                            "<b>Security Measure</b>"))
    {
      aw_session_run_postponed (session);
      success = TRUE;
    }

  return success;
}

GdkPixbuf *
aw_session_fetch_captcha_finish (AwSession     *session,
                                 GAsyncResult  *result,
                                 GError       **error)
{
  GdkPixbuf    *pixbuf = NULL;
  SoupMessage  *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_captcha_async,
                                       error);

  if (message)
    {
      gsize         length = message->response_body->length;
      const char   *data = message->response_body->data;
      GInputStream *stream;

      stream = g_memory_input_stream_new_from_data (data, length, NULL);
      pixbuf = gdk_pixbuf_new_from_stream (stream, NULL, error);

      g_object_unref (stream);
AW_DEBUG_REF_COUNT (session);
AW_DEBUG_REF_COUNT (result);
AW_DEBUG_REF_COUNT (message);
    }

  return pixbuf;
}

GList *
aw_session_fetch_news_finish (AwSession     *session,
                              GAsyncResult  *result,
                              GError       **error)
{
  GList       *news = NULL;
  SoupMessage *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_news_async,
                                       error);

  if (message)
    news = aw_parser_read_news (message->response_body->data,
                                message->response_body->length, error);

  return news;
}

GList *
aw_session_fetch_map_finish (AwSession     *session,
                             GAsyncResult  *result,
                             GError       **error)
{
  GList       *stars = NULL;
  SoupMessage *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_map_async,
                                       error);

  if (message)
    stars = aw_parser_read_map (message->response_body->data,
                                message->response_body->length, error);

  return stars;
}

GList *
aw_session_fetch_system_finish (AwSession          *session,
                                GAsyncResult       *result,
                                GError            **error)
{
  GList       *planets = NULL;
  SoupMessage *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_system_async,
                                       error);

  if (message)
    planets = aw_parser_read_system (message->response_body->data,
                                     message->response_body->length, error);

  return planets;
}

GList *
aw_session_fetch_planets_finish (AwSession     *session,
                                 GAsyncResult  *result,
                                 GError       **error)
{
  GList       *planets = NULL;
  SoupMessage *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_planets_async,
                                       error);

  if (message)
    planets = aw_parser_read_planets (message->response_body->data,
                                      message->response_body->length, error);

  return planets;
}

GList *
aw_session_fetch_buildings_finish (AwSession     *session,
                                   GAsyncResult  *result,
                                   GError       **error)
{
  GList       *planets = NULL;
  SoupMessage *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_buildings_async,
                                       error);

  if (message)
    planets = aw_parser_read_buildings (message->response_body->data,
                                        message->response_body->length, error);

  return planets;
}

GList *
aw_session_fetch_science_finish (AwSession       *session,
                                 GAsyncResult    *result,
                                 AwScienceId     *current,
                                 AwScienceStats  *statistics,
                                 GError         **error)
{
  GList       *sciences = NULL, *l;
  SoupMessage *message;
  AwProfile   *profile;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_science_async,
                                       error);

  if (message)
    sciences = aw_parser_read_sciences (message->response_body->data,
                                        message->response_body->length,
                                        current, statistics, error);

  profile = aw_profile_get_self ();

  for (l = sciences; l; l = l->next)
    {
      aw_profile_set_level (profile, aw_science_get_id (l->data),
                            aw_science_get_level (l->data));
    }

  g_object_unref (profile);

  return sciences;
}

GList *
aw_session_fetch_fleets_finish (AwSession     *session,
                                GAsyncResult  *result,
                                GError       **error)
{
  GList       *fleets = NULL;
  SoupMessage *message;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_fleets_async,
                                       error);

  if (message)
    fleets = aw_parser_read_fleets (message->response_body->data,
                                    message->response_body->length, error);

  return fleets;
}

void
aw_session_select_science_async (AwSession          *session,
                                 AwScienceId         science_id,
                                 GAsyncReadyCallback callback,
                                 gpointer            user_data)
{
  const char *field_value = NULL;

  g_return_if_fail (AW_IS_SESSION (session));

  switch (science_id)
    {
    case AW_SCIENCE_BIOLOGY:
      field_value = "f_bio";
      break;
    case AW_SCIENCE_ECONOMY:
      field_value = "f_eco";
      break;
    case AW_SCIENCE_ENERGY:
      field_value = "f_energy";
      break;
    case AW_SCIENCE_MATHEMATICS:
      field_value = "f_mathematics";
      break;
    case AW_SCIENCE_PHYSICS:
      field_value = "f_physics";
      break;
    case AW_SCIENCE_SOCIAL:
      field_value = "f_social";
      break;
    case AW_SCIENCE_CULTURE:
    case AW_SCIENCE_WARFARE:
    case AW_SCIENCE_OVERALL:
    case AW_SCIENCE_INVALID:
    case AW_SCIENCE_LAST:
      break;
    }

  g_return_if_fail (NULL != field_value);

  aw_session_post_async (session, callback, user_data,
                         aw_session_select_science_async,
                         "/0/Science/submit.php",
                         "science", field_value, NULL);
}

void
aw_session_fetch_profile_async (AwSession           *session,
                                int                  profile_id,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  if (profile_id < 1)
    g_object_get (aw_settings_get_singleton (),
                  "profile-id", &profile_id, NULL);

  aw_session_fetch_async (session, callback, user_data,
                          aw_session_fetch_profile_async,
                          "/0/Player/Profile.php?id=%d", profile_id);
}

AwProfile *
aw_session_fetch_profile_finish (AwSession     *session,
                                 GAsyncResult  *result,
                                 GError       **error)
{
  AwProfile   *profile = NULL;
  int          profile_id;
  SoupMessage *message;
  const char  *query;

  message = aw_session_finish_message (session, result,
                                       aw_session_fetch_profile_async,
                                       error);


  if (message)
    {
      query = soup_message_get_uri (message)->query;

      if (query)
        query = strstr (query, "id=");
      if (query && 1 != sscanf (query, "id=%d", &profile_id))
        profile_id = 0;

      profile = aw_parser_read_profile (message->response_body->data,
                                        message->response_body->length,
                                        profile_id, error);
    }

  if (profile)
    {
      g_object_get (aw_settings_get_singleton (),
                    "profile-id", &profile_id, NULL);

      if (profile_id && profile_id == aw_profile_get_id (profile))
        {
          AwProfile *self = aw_profile_get_self ();
          aw_profile_reset (self, profile);
          g_object_unref (self);
        }
    }

  return profile;
}
