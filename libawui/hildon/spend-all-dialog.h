#ifndef __AW_SPEND_ALL_DIALOG_H__
#define __AW_SPEND_ALL_DIALOG_H__ 1

#include <hildon/hildon.h>
#include <libawui/model.h>

#define AW_TYPE_SPEND_ALL_DIALOG            (aw_spend_all_dialog_get_type ())
#define AW_SPEND_ALL_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_SPEND_ALL_DIALOG, AwSpendAllDialog))
#define AW_SPEND_ALL_DIALOG_CLASS(cls)      (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_SPEND_ALL_DIALOG, AwSpendAllDialogClass))
#define AW_IS_SPEND_ALL_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_SPEND_ALL_DIALOG))
#define AW_IS_SPEND_ALL_DIALOG_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_SPEND_ALL_DIALOG))
#define AW_SPEND_ALL_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_SPEND_ALL_DIALOG, AwSpendAllDialogClass))

typedef struct _AwSpendAllDialog           AwSpendAllDialog;
typedef struct _AwSpendAllDialogPrivate    AwSpendAllDialogPrivate;
typedef struct _AwSpendAllDialogClass      AwSpendAllDialogClass;

struct _AwSpendAllDialog {
  HildonPickerDialog       parent;
  AwSpendAllDialogPrivate *priv;
};

struct _AwSpendAllDialogClass {
  HildonPickerDialogClass parent_class;
};

GType
aw_spend_all_dialog_get_type              (void) G_GNUC_CONST;

GtkWidget *
aw_spend_all_dialog_new                   (GtkWindow        *parent);

void
aw_spend_all_dialog_set_production_points (AwSpendAllDialog *dialog,
                                           int               points);

int
aw_spend_all_dialog_get_production_points (AwSpendAllDialog *dialog);

AwItemType
aw_spend_all_dialog_get_selected_item     (AwSpendAllDialog *dialog);

#endif /* __AW_SPEND_ALL_DIALOG_H__ */


