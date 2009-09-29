#ifndef __AW_CAPTCHA_DIALOG_H__
#define __AW_CAPTCHA_DIALOG_H__ 1

#include <gtk/gtk.h>

#define AW_TYPE_CAPTCHA_DIALOG            (aw_captcha_dialog_get_type ())
#define AW_CAPTCHA_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_CAPTCHA_DIALOG, AwCaptchaDialog))
#define AW_CAPTCHA_DIALOG_CLASS(cls)      (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_CAPTCHA_DIALOG, AwCaptchaDialogClass))
#define AW_IS_CAPTCHA_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_CAPTCHA_DIALOG))
#define AW_IS_CAPTCHA_DIALOG_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_CAPTCHA_DIALOG))
#define AW_CAPTCHA_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_CAPTCHA_DIALOG, AwCaptchaDialogClass))

typedef struct _AwCaptchaDialog           AwCaptchaDialog;
typedef struct _AwCaptchaDialogPrivate    AwCaptchaDialogPrivate;
typedef struct _AwCaptchaDialogClass      AwCaptchaDialogClass;

struct _AwCaptchaDialog {
  GtkDialog               parent;
  AwCaptchaDialogPrivate *priv;
};

struct _AwCaptchaDialogClass {
  GtkDialogClass parent_class;
};

GType
aw_captcha_dialog_get_type      (void) G_GNUC_CONST;

GtkWidget *
aw_captcha_dialog_new           (GtkWindow       *parent);

void
aw_captcha_dialog_set_challenge (AwCaptchaDialog *dialog,
                                 GdkPixbuf       *challenge);

GdkPixbuf *
aw_captcha_dialog_get_challenge (AwCaptchaDialog *dialog);

void
aw_captcha_dialog_set_response  (AwCaptchaDialog *dialog,
                                 const char      *text);

G_CONST_RETURN char *
aw_captcha_dialog_get_response  (AwCaptchaDialog *dialog);

#endif /* __AW_CAPTCHA_DIALOG_H__ */

