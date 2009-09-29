#include "main.h"

#include <glib/gi18n.h>

static void
send_login_data_cb (GObject      *source,
                    GAsyncResult *result,
                    gpointer      dialog)
{
  GError   *error = NULL;
  gboolean  success;

  success = aw_session_login_finish (AW_SESSION (source), result, &error);

  if (error)
    {
      hildon_banner_show_information (dialog, NULL, error->message);
      g_error_free (error);
    }
  else if (success)
    {
      gtk_dialog_response (dialog, GTK_RESPONSE_CLOSE);
    }
  else
    {
      hildon_banner_show_information (dialog, NULL, _("Incorrect password"));
      hildon_gtk_window_set_progress_indicator (dialog, 0);
      gtk_widget_set_sensitive (dialog, TRUE);
      gtk_window_present (dialog);
    }
}

static void
send_login_data (AwLoginDialog *dialog,
                 AwSession     *session)
{
  const char *username, *password;
  gboolean    store;

  username = aw_login_dialog_get_username          (dialog);
  password = aw_login_dialog_get_password          (dialog);
  store    = aw_login_dialog_get_store_credentials (dialog);

  g_object_set (aw_settings_get_singleton (),
                "username", username, "store-credentials", store, NULL);

  if (store)
    g_object_set (aw_settings_get_singleton (), "password", password, NULL);

  hildon_gtk_window_set_progress_indicator (GTK_WINDOW (dialog), 1);
  gtk_widget_set_sensitive (GTK_WIDGET (dialog), FALSE);

  aw_session_login_async (session, username, password,
                          send_login_data_cb, dialog);
}

static void
response_cb (AwLoginDialog *dialog,
             int            response,
             AwSession     *session)
{
  if (GTK_RESPONSE_OK == response)
    send_login_data (dialog, session);
  else
    gtk_widget_destroy (GTK_WIDGET (dialog));
}

void
aw_main_login (GtkWidget *window,
               AwSession *session)
{
  static GtkWidget *dialog = NULL;

  if (!dialog)
    {
      dialog = aw_login_dialog_new (GTK_WINDOW (window));
      g_object_add_weak_pointer (G_OBJECT (dialog), (gpointer) &dialog);

      g_signal_connect (dialog, "response",
                        G_CALLBACK (response_cb), session);

      gtk_widget_show (dialog);
    }

  hildon_gtk_window_set_progress_indicator (GTK_WINDOW (dialog), 0);
  gtk_widget_set_sensitive (dialog, TRUE);

  gtk_window_present (GTK_WINDOW (dialog));
}

