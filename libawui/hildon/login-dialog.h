#ifndef __AW_LOGIN_DIALOG_H__
#define __AW_LOGIN_DIALOG_H__ 1

#include <gtk/gtk.h>

#define AW_TYPE_LOGIN_DIALOG            (aw_login_dialog_get_type ())
#define AW_LOGIN_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_LOGIN_DIALOG, AwLoginDialog))
#define AW_LOGIN_DIALOG_CLASS(cls)      (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_LOGIN_DIALOG, AwLoginDialogClass))
#define AW_IS_LOGIN_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_LOGIN_DIALOG))
#define AW_IS_LOGIN_DIALOG_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_LOGIN_DIALOG))
#define AW_LOGIN_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_LOGIN_DIALOG, AwLoginDialogClass))

typedef struct _AwLoginDialog           AwLoginDialog;
typedef struct _AwLoginDialogPrivate    AwLoginDialogPrivate;
typedef struct _AwLoginDialogClass      AwLoginDialogClass;

struct _AwLoginDialog {
  GtkDialog             parent;
  AwLoginDialogPrivate *priv;
};

struct _AwLoginDialogClass {
  GtkDialogClass parent_class;
};

GType
aw_login_dialog_get_type                (void) G_GNUC_CONST;

GtkWidget *
aw_login_dialog_new                     (GtkWindow *parent);

G_CONST_RETURN char *
aw_login_dialog_get_username            (AwLoginDialog *dialog);

G_CONST_RETURN char *
aw_login_dialog_get_password            (AwLoginDialog *dialog);

gboolean
aw_login_dialog_get_store_credentials   (AwLoginDialog *dialog);

#endif /* __AW_LOGIN_DIALOG_H__ */

