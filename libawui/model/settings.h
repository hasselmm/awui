#ifndef __AW_MODEL_SETTINGS_H__
#define __AW_MODEL_SETTINGS_H__ 1

#include <glib-object.h>

#define AW_TYPE_SETTINGS                (aw_settings_get_type ())
#define AW_SETTINGS(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_SETTINGS, AwSettings))
#define AW_SETTINGS_CLASS(cls)          (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_SETTINGS, AwSettingsClass))
#define AW_IS_SETTINGS(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_SETTINGS))
#define AW_IS_SETTINGS_CLASS(obj)       (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_SETTINGS))
#define AW_SETTINGS_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_SETTINGS, AwSettingsClass))

typedef struct _AwSettings               AwSettings;
typedef struct _AwSettingsClass          AwSettingsClass;

GType
aw_settings_get_type                    (void) G_GNUC_CONST;

AwSettings *
aw_settings_new                         (void);

AwSettings *
aw_settings_get_singleton               (void);

#endif /* __AW_MODEL_SETTINGS_H__ */
