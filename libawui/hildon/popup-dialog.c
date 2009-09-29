#include "config.h"
#include "popup-dialog.h"
#include "marshal.h"

G_DEFINE_TYPE (AwPopupDialog, aw_popup_dialog, GTK_TYPE_DIALOG);

static void
aw_popup_dialog_init (AwPopupDialog *dialog)
{
  gtk_widget_hide (gtk_dialog_get_action_area (GTK_DIALOG (dialog)));
}

static void
aw_popup_dialog_class_init (AwPopupDialogClass *class)
{
}

GtkWidget *
aw_popup_dialog_new (GtkWindow  *parent,
                     const char *title)
{
  return gtk_widget_new (AW_TYPE_POPUP_DIALOG,
                         "transient-for", parent, "title", title,
                         "modal", TRUE, "destroy-with-parent", TRUE,
                         "has-separator", FALSE, NULL);
}

static void
aw_popup_dialog_clicked_cb (GtkWidget *button,
                            GClosure  *closure)
{
  GValue     return_value   =  { 0, };
  GValue     param_values[] = {{ 0, }};
  GtkWidget *dialog;
 
  dialog = gtk_widget_get_ancestor (button, AW_TYPE_POPUP_DIALOG);

  g_value_init        (&return_value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&return_value, FALSE);

  g_value_init        (&param_values[0], AW_TYPE_POPUP_DIALOG);
  g_value_set_object  (&param_values[0], dialog);

  g_closure_invoke (closure, &return_value, 1, param_values, NULL);

  if (!g_value_get_boolean (&return_value))
    gtk_widget_destroy (dialog);

  g_value_unset (&param_values[0]);
  g_value_unset (&return_value);
}

GtkWidget *
aw_popup_dialog_append (AwPopupDialog         *dialog,
                        const char            *title,
                        const char            *details,
                        AwPopupDialogCallback  callback,
                        gpointer               user_data)
{
  GtkWidget *button, *content_area;
  GClosure  *closure;

  g_return_val_if_fail (AW_IS_POPUP_DIALOG (dialog), NULL);

  button = hildon_button_new_with_text (HILDON_SIZE_AUTO_WIDTH |
                                        HILDON_SIZE_FINGER_HEIGHT,
                                        HILDON_BUTTON_ARRANGEMENT_VERTICAL,
                                        title, details);

  closure = g_cclosure_new (G_CALLBACK (callback), user_data, NULL);
  g_closure_set_marshal (closure, aw_cclosure_marshal_BOOLEAN__VOID);

  g_signal_connect_data (button, "clicked",
                         G_CALLBACK (aw_popup_dialog_clicked_cb),
                         closure, (GClosureNotify) g_closure_unref, 0);

  content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_container_add (GTK_CONTAINER (content_area), button);
  gtk_widget_show (button);

  return button;
}

