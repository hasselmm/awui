#ifndef __AW_POPUP_DIALOG_H__
#define __AW_POPUP_DIALOG_H__ 1

#include <hildon/hildon.h>

#define AW_TYPE_POPUP_DIALOG                 (aw_popup_dialog_get_type ())
#define AW_POPUP_DIALOG(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_POPUP_DIALOG, AwPopupDialog))
#define AW_POPUP_DIALOG_CLASS(cls)           (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_POPUP_DIALOG, AwPopupDialogClass))
#define AW_IS_POPUP_DIALOG(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_POPUP_DIALOG))
#define AW_IS_POPUP_DIALOG_CLASS(obj)        (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_POPUP_DIALOG))
#define AW_POPUP_DIALOG_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_POPUP_DIALOG, AwPopupDialogClass))

typedef struct _AwPopupDialog                AwPopupDialog;
typedef struct _AwPopupDialogPrivate         AwPopupDialogPrivate;
typedef struct _AwPopupDialogClass           AwPopupDialogClass;

typedef gboolean (* AwPopupDialogCallback) (AwPopupDialog *dialog,
                                            gpointer       user_data);


struct _AwPopupDialog {
  GtkDialog             parent;
  AwPopupDialogPrivate *priv;
};

struct _AwPopupDialogClass {
  GtkDialogClass parent_class;
};

GType
aw_popup_dialog_get_type (void) G_GNUC_CONST;

GtkWidget *
aw_popup_dialog_new      (GtkWindow             *parent,
                          const char            *title);

GtkWidget *
aw_popup_dialog_append   (AwPopupDialog         *dialog,
                          const char            *title,
                          const char            *details,
                          AwPopupDialogCallback  callback,
                          gpointer               user_data);

#endif /* __AW_POPUP_DIALOG_H__ */
