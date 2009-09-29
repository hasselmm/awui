#include "main.h"

#include <glib/gi18n.h>

static void
solve_captcha_cb (GObject      *source,
                  GAsyncResult *result,
                  gpointer      dialog)
{
  GError   *error = NULL;
  gboolean  success;

  success = aw_session_solve_captcha_finish (AW_SESSION (source), result, &error);

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
      hildon_banner_show_information (dialog, NULL, _("Incorrect solution"));
      hildon_gtk_window_set_progress_indicator (dialog, 0);
      gtk_widget_set_sensitive (dialog, TRUE);
      gtk_window_present (dialog);
    }
}

static void
send_response (AwCaptchaDialog *dialog,
               AwSession       *session)
{
  hildon_gtk_window_set_progress_indicator (GTK_WINDOW (dialog), 1);
  gtk_widget_set_sensitive (GTK_WIDGET (dialog), FALSE);

  aw_session_solve_captcha_async (session,
                                  aw_captcha_dialog_get_response (dialog),
                                  solve_captcha_cb, dialog);
}

static void
response_cb (AwCaptchaDialog *dialog,
             int              response,
             AwSession       *session)
{
  if (GTK_RESPONSE_OK == response)
    send_response (dialog, session);
  else
    gtk_widget_destroy (GTK_WIDGET (dialog));
}

static void
fetch_captcha_cb (GObject      *source,
                  GAsyncResult *result,
                  gpointer      dialog)
{
  GError    *error = NULL;
  char      *solution, *s;
  GdkPixbuf *challenge;

  challenge = aw_session_fetch_captcha_finish (AW_SESSION (source), result, NULL);

  if (error)
    {
      hildon_banner_show_information (dialog, NULL, error->message);
      g_error_free (error);
    }
  else if (challenge)
    {
      solution = aw_captcha_solve (challenge);

      for (s = solution; *s; ++s)
        *s = g_ascii_toupper (*s);

      aw_captcha_dialog_set_challenge (dialog, challenge);
      aw_captcha_dialog_set_response (dialog, solution);
      g_free (solution);
    }
}

void
aw_main_solve_captcha (GtkWidget *window,
                       AwSession *session)
{
  static GtkWidget *dialog = NULL;

  if (!dialog)
    {
      dialog = aw_captcha_dialog_new (GTK_WINDOW (window));
      g_object_add_weak_pointer (G_OBJECT (dialog), (gpointer) &dialog);

      g_signal_connect (dialog, "response",
                        G_CALLBACK (response_cb), session);

      gtk_widget_show (dialog);
    }

  aw_session_fetch_captcha_async (session, fetch_captcha_cb, dialog);
  hildon_gtk_window_set_progress_indicator (GTK_WINDOW (dialog), 0);
  gtk_widget_set_sensitive (dialog, TRUE);

  gtk_window_present (GTK_WINDOW (dialog));
}

