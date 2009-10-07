#include "config.h"
#include "login-dialog.h"

#include <glib/gi18n-lib.h>
#include <hildon/hildon.h>
#include <libawui/model.h>

struct _AwLoginDialogPrivate {
  GtkWidget *username;
  GtkWidget *password;
  GtkWidget *store_credentials;
};

G_DEFINE_TYPE (AwLoginDialog, aw_login_dialog, GTK_TYPE_DIALOG);

static void
aw_login_dialog_init (AwLoginDialog *dialog)
{
  GtkWidget  *table, *widget, *content;
  char       *username, *password;
  gboolean    store_credentials;

  dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog, AW_TYPE_LOGIN_DIALOG,
                                              AwLoginDialogPrivate);

  g_object_get (aw_settings_get_singleton (),
                "username", &username, "password", &password,
                "store-credentials", &store_credentials, NULL);

  gtk_dialog_add_button (GTK_DIALOG (dialog), _("Login"), GTK_RESPONSE_OK);

  table = gtk_table_new (3, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), HILDON_MARGIN_DOUBLE);
  content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
  gtk_container_add (GTK_CONTAINER (content), table);

  widget = gtk_label_new (_("Username"));
  gtk_misc_set_alignment (GTK_MISC (widget), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), widget, 0, 1, 0, 1, GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  widget = hildon_entry_new (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT);
  gtk_entry_set_text (GTK_ENTRY (widget), username ? username : "");
  gtk_table_attach_defaults (GTK_TABLE (table), widget, 1, 2, 0, 1);
  dialog->priv->username = widget;

  widget = gtk_label_new (_("Password"));
  gtk_misc_set_alignment (GTK_MISC (widget), 0.0, 0.5);
  gtk_table_attach (GTK_TABLE (table), widget, 0, 1, 1, 2, GTK_FILL, GTK_FILL | GTK_EXPAND, 0, 0);

  widget = hildon_entry_new (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT);
  gtk_entry_set_text (GTK_ENTRY (widget), password ? password : "");
  gtk_entry_set_visibility (GTK_ENTRY (widget), FALSE);
  gtk_table_attach_defaults (GTK_TABLE (table), widget, 1, 2, 1, 2);
  dialog->priv->password = widget;

  widget = hildon_check_button_new (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT);
  hildon_check_button_set_active (HILDON_CHECK_BUTTON (widget), store_credentials);
  gtk_button_set_label (GTK_BUTTON (widget), _("Remember login credentials"));
  gtk_table_attach_defaults (GTK_TABLE (table), widget, 1, 2, 2, 3);
  dialog->priv->store_credentials = widget;

  gtk_widget_show_all (table);

  g_free (username);
  g_free (password);
}

static void
aw_login_dialog_class_init (AwLoginDialogClass *class)
{
  g_type_class_add_private (class, sizeof (AwLoginDialogPrivate));
}

GtkWidget *
aw_login_dialog_new (GtkWindow *window)
{
  g_return_val_if_fail (GTK_IS_WINDOW (window) || !window, NULL);

  return gtk_widget_new (AW_TYPE_LOGIN_DIALOG,
                         "transient-for", window,
                         "destroy-with-parent", TRUE,
                         "title", _("Login Required"),
                         "modal", window ? TRUE : FALSE, NULL);
}

G_CONST_RETURN char *
aw_login_dialog_get_username (AwLoginDialog *dialog)
{
  g_return_val_if_fail (AW_IS_LOGIN_DIALOG (dialog), NULL);
  return gtk_entry_get_text (GTK_ENTRY (dialog->priv->username));
}

G_CONST_RETURN char *
aw_login_dialog_get_password (AwLoginDialog *dialog)
{
  g_return_val_if_fail (AW_IS_LOGIN_DIALOG (dialog), NULL);
  return gtk_entry_get_text (GTK_ENTRY (dialog->priv->password));
}

gboolean
aw_login_dialog_get_store_credentials (AwLoginDialog *dialog)
{
  HildonCheckButton *button;

  g_return_val_if_fail (AW_IS_LOGIN_DIALOG (dialog), TRUE);

  button = HILDON_CHECK_BUTTON (dialog->priv->store_credentials);
  return hildon_check_button_get_active (button);
}

