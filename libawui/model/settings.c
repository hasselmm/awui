#include "config.h"
#include "settings.h"

enum {
  PROP_0,
  PROP_USERNAME,
  PROP_PASSWORD,
  PROP_STORE_CREDENTIALS,
  PROP_PLAYER_LEVEL,
  PROP_ALLIANCE_TAG,
  PROP_SESSION_COOKIES,
  PROP_ORIGIN,
};

struct _AwSettings {
  GObject   parent;
  GKeyFile *keyfile;
  char     *filename;
  gulong    save_id;
};

struct _AwSettingsClass {
  GObjectClass parent_class;
};

static AwSettings *singleton = NULL;

G_DEFINE_TYPE (AwSettings, aw_settings, G_TYPE_OBJECT);

static void
aw_settings_init (AwSettings *settings)
{
  char *config_dir;

  config_dir = g_build_filename (g_get_user_config_dir (), PACKAGE, NULL);
  settings->filename = g_build_filename (config_dir, "settings", NULL);
  g_mkdir_with_parents (config_dir, 0700);
  g_free (config_dir);

  settings->keyfile = g_key_file_new ();
  g_key_file_load_from_file (settings->keyfile, settings->filename, 0, NULL);
}

static GObject *
aw_settings_constructor (GType                  type,
                         unsigned               n_params,
                         GObjectConstructParam *params)
{
  GObjectClass *object_parent_class;
  GObject      *object;

  if (G_UNLIKELY (!singleton))
    {
      object_parent_class = G_OBJECT_CLASS (aw_settings_parent_class);
      object = object_parent_class->constructor (type, n_params, params);
      g_object_add_weak_pointer (object, (gpointer) &singleton);
      singleton = AW_SETTINGS (object);
    }
  else
    object = g_object_ref (singleton);

  return object;
}

static gboolean
aw_settings_save_cb (gpointer user_data)
{
  AwSettings *settings = AW_SETTINGS (user_data);
  char       *data = NULL;
  gsize       length = 0;

  g_print ("%s: %s (%p)\n", G_STRFUNC, settings->filename, settings->keyfile);

  if (settings->keyfile)
    data = g_key_file_to_data (settings->keyfile, &length, NULL);
  if (data)
    g_file_set_contents (settings->filename, data, length, NULL);

  settings->save_id = 0;
  g_free (data);

  return FALSE;
}

static void
aw_settings_finalize (GObject *object)
{
  AwSettings *settings = AW_SETTINGS (object);

  if (settings->save_id)
    {
      g_source_remove (settings->save_id);
      aw_settings_save_cb (settings);
    }

  g_key_file_free (settings->keyfile);
  g_free (settings->filename);

  G_OBJECT_CLASS (aw_settings_parent_class)->finalize (object);
}

static void
aw_settings_save (AwSettings *settings)
{
  if (!settings->save_id)
    settings->save_id = g_idle_add (aw_settings_save_cb, settings);
}

static void
aw_settings_set_string (AwSettings   *settings,
                        const char   *group,
                        const char   *key,
                        const GValue *value)
{
  g_key_file_set_string (settings->keyfile, group, key, g_value_get_string (value));
  aw_settings_save (settings);
}

static void
aw_settings_set_boolean (AwSettings   *settings,
                         const char   *group,
                         const char   *key,
                         const GValue *value)
{
  g_key_file_set_boolean (settings->keyfile, group, key, g_value_get_boolean (value));
  aw_settings_save (settings);
}

static void
aw_settings_set_integer (AwSettings   *settings,
                         const char   *group,
                         const char   *key,
                         const GValue *value)
{
  g_key_file_set_integer (settings->keyfile, group, key, g_value_get_int (value));
  aw_settings_save (settings);
}

static void
aw_settings_set_property (GObject      *object,
                          unsigned      prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  AwSettings *settings = AW_SETTINGS (object);

  switch (prop_id) {
  case PROP_USERNAME:
    aw_settings_set_string (settings, "profile", "username", value);
    break;
  case PROP_PASSWORD:
    aw_settings_set_string (settings, "profile", "password", value);
    break;
  case PROP_STORE_CREDENTIALS:
    aw_settings_set_boolean (settings, "profile", "store-credentials", value);
    break;
  case PROP_PLAYER_LEVEL:
    aw_settings_set_integer (settings, "profile", "player-level", value);
    break;
  case PROP_ALLIANCE_TAG:
    aw_settings_set_string (settings, "profile", "alliance", value);
    break;
  case PROP_SESSION_COOKIES:
    aw_settings_set_string (settings, "session", "cookies", value);
    break;
  case PROP_ORIGIN:
    aw_settings_set_integer (settings, "profile", "origin", value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
aw_settings_get_string (AwSettings *settings,
                        const char *group,
                        const char *key,
                        GValue     *target,
                        GParamSpec *pspec)
{
  GError *error = NULL;
  char   *value;

  value = g_key_file_get_string (settings->keyfile, group, key, &error);

  if (error)
    {
      g_param_value_set_default (pspec, target);
      g_error_free (error);
    }
  else
    g_value_set_string (target, value);

  g_free (value);
}

static void
aw_settings_get_boolean (AwSettings *settings,
                         const char *group,
                         const char *key,
                         GValue     *target,
                         GParamSpec *pspec)
{
  GError   *error = NULL;
  gboolean  value;

  value = g_key_file_get_boolean (settings->keyfile, group, key, &error);

  if (error)
    {
      g_param_value_set_default (pspec, target);
      g_error_free (error);
    }
  else
    g_value_set_boolean (target, value);
}

static void
aw_settings_get_integer (AwSettings *settings,
                         const char *group,
                         const char *key,
                         GValue     *target,
                         GParamSpec *pspec)
{
  GError *error = NULL;
  int     value;

  value = g_key_file_get_integer (settings->keyfile, group, key, &error);

  if (error)
    {
      g_param_value_set_default (pspec, target);
      g_error_free (error);
    }
  else
    g_value_set_int (target, value);
}

static void
aw_settings_get_property (GObject    *object,
                          unsigned    prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  AwSettings *settings = AW_SETTINGS (object);

  switch (prop_id) {
  case PROP_USERNAME:
    aw_settings_get_string (settings, "profile", "username", value, pspec);
    break;
  case PROP_PASSWORD:
    aw_settings_get_string (settings, "profile", "password", value, pspec);
    break;
  case PROP_STORE_CREDENTIALS:
    aw_settings_get_boolean (settings, "profile", "store-credentials", value, pspec);
    break;
  case PROP_PLAYER_LEVEL:
    aw_settings_get_integer (settings, "profile", "player-level", value, pspec);
    break;
  case PROP_ALLIANCE_TAG:
    aw_settings_get_string (settings, "profile", "alliance", value, pspec);
    break;
  case PROP_SESSION_COOKIES:
    aw_settings_get_string (settings, "session", "cookies", value, pspec);
    break;
  case PROP_ORIGIN:
    aw_settings_get_integer (settings, "profile", "origin", value, pspec);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
aw_settings_class_init (AwSettingsClass *class)
{
  GObjectClass *object_class;

  object_class               = G_OBJECT_CLASS (class);
  object_class->constructor  = aw_settings_constructor;
  object_class->finalize     = aw_settings_finalize;
  object_class->set_property = aw_settings_set_property;
  object_class->get_property = aw_settings_get_property;

  g_object_class_install_property
    (object_class, PROP_USERNAME,
     g_param_spec_string ("username",
                          "Username",
                          "Username for playing the game",
                          g_get_user_name (),
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class, PROP_PASSWORD,
     g_param_spec_string ("password",
                          "Password",
                          "Password for playing the game",
                          "",
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class, PROP_STORE_CREDENTIALS,
     g_param_spec_boolean ("store-credentials",
                           "Store Credentials",
                           "Weither to store login credentials",
                           TRUE,
                           G_PARAM_READWRITE |
                           G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class, PROP_PLAYER_LEVEL,
     g_param_spec_int ("player-level",
                       "Player Level",
                       "Combat experience of the player",
                       0, G_MAXINT, 0,
                       G_PARAM_READWRITE |
                       G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class, PROP_ALLIANCE_TAG,
     g_param_spec_string ("alliance-tag",
                          "Alliance Tag",
                          "Identifier of the player's alliance",
                          NULL,
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class, PROP_SESSION_COOKIES,
     g_param_spec_string ("session-cookies",
                          "Session Cookies",
                          "Cookies of the HTTP session.",
                          NULL,
                          G_PARAM_READWRITE |
                          G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class, PROP_ORIGIN,
     g_param_spec_int ("origin",
                       "Origin",
                       "Origin of the player",
                       0, G_MAXINT, 0,
                       G_PARAM_READWRITE |
                       G_PARAM_STATIC_STRINGS));
}

AwSettings *
aw_settings_new (void)
{
  return g_object_new (AW_TYPE_SETTINGS, NULL);
}

AwSettings *
aw_settings_get_singleton (void)
{
  g_warn_if_fail (AW_IS_SETTINGS (singleton));
  return singleton;
}
