#include "config.h"
#include "captcha-dialog.h"

#include <hildon/hildon.h>
#include <glib/gi18n-lib.h>

#define AW_CAPTCHA_WIDTH  240
#define AW_CAPTCHA_HEIGHT  60

struct _AwCaptchaDialogPrivate {
  GtkWidget *selector;
  GtkWidget *challenge;
  GtkWidget *response;

  unsigned   changing : 1;
};

G_DEFINE_TYPE (AwCaptchaDialog, aw_captcha_dialog, GTK_TYPE_DIALOG);

static GtkTreeModel *
aw_captcha_dialog_create_model (void)
{
  GtkListStore *store;
  char          text[] = "0";
  GtkTreeIter   iter;
  int           i;

  store = gtk_list_store_new (1, G_TYPE_STRING);

  for (i = 0; i < 10; ++i)
    {
      text[0] = '0' + i;
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, text, -1);
    }

  for (i = 0; i < 6; ++i)
    {
      text[0] = 'A' + i;
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter, 0, text, -1);
    }

  return GTK_TREE_MODEL (store);
}

static void
aw_captcha_dialog_entry_changed_cb (AwCaptchaDialog *dialog)
{
  HildonTouchSelector *selector;
  GtkTreeModel        *model;
  GtkTreeIter          iter;
  const char          *text;
  int                  i;

  if (dialog->priv->changing)
    return;

  selector = HILDON_TOUCH_SELECTOR (dialog->priv->selector);
  model = hildon_touch_selector_get_model (selector, 1);
  dialog->priv->changing = TRUE;

  text = aw_captcha_dialog_get_response (dialog);

  for (i = 1; i <= 5 && *text; ++i, ++text)
    {
      int value;

      if (*text >= '0' && *text <= '9')
        value = *text - '0';
      else if (*text >= 'A' && *text <= 'F')
        value = *text - 'A' + 10;
      else if (*text >= 'a' && *text <= 'f')
        value = *text - 'a' + 10;
      else
        value = -1;

      if (value >= 0 && gtk_tree_model_iter_nth_child (model, &iter, NULL, value))
        hildon_touch_selector_select_iter (selector, i, &iter, TRUE);
    }

  dialog->priv->changing = FALSE;
}

static void
aw_captcha_dialog_selector_changed_cb (AwCaptchaDialog *dialog)
{
  HildonTouchSelector *selector;
  GtkTreeModel        *model;
  GtkTreeIter          iter;
  GString             *text;
  char                *number;
  int                  i;

  if (dialog->priv->changing)
    return;

  selector = HILDON_TOUCH_SELECTOR (dialog->priv->selector);
  model = hildon_touch_selector_get_model (selector, 1);
  text = g_string_new (NULL);
  dialog->priv->changing = TRUE;

  for (i = 1; i <= 5; ++i)
    {
      if (hildon_touch_selector_get_selected (selector, i, &iter))
        {
          gtk_tree_model_get (model, &iter, 0, &number, -1);
          g_string_append (text, number);
          g_free (number);
        }
      else
        g_string_append (text, "0");
    }

  aw_captcha_dialog_set_response (dialog, text->str);
  dialog->priv->changing = FALSE;
  g_string_free (text, TRUE);
}

static void
aw_captcha_dialog_init (AwCaptchaDialog *dialog)
{
  GtkWidget    *table, *widget, *content;
  GtkTreeModel *model;

  dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog, AW_TYPE_CAPTCHA_DIALOG,
                                              AwCaptchaDialogPrivate);

  content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

  gtk_dialog_add_button (GTK_DIALOG (dialog), _("Submit"), GTK_RESPONSE_OK);

  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), HILDON_MARGIN_DOUBLE);
  gtk_container_add (GTK_CONTAINER (content), table);

  widget = gtk_image_new ();
  gtk_widget_set_size_request (widget, AW_CAPTCHA_WIDTH, AW_CAPTCHA_HEIGHT);
  gtk_table_attach (GTK_TABLE (table), widget, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);
  dialog->priv->challenge = widget;

  widget = hildon_entry_new (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT);
  gtk_entry_set_overwrite_mode (GTK_ENTRY (widget), TRUE);
  gtk_entry_set_max_length (GTK_ENTRY (widget), 5);
  gtk_table_attach (GTK_TABLE (table), widget, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, 0, 0, 0);
  dialog->priv->response = widget;

  g_signal_connect_swapped (widget, "changed",
                            G_CALLBACK (aw_captcha_dialog_entry_changed_cb),
                            dialog);

  widget = hildon_touch_selector_new_text ();
  gtk_widget_set_size_request (widget, -1, 4 * 70);
  gtk_table_attach_defaults (GTK_TABLE (table), widget, 0, 2, 1, 2);
  dialog->priv->selector = widget;

  model = aw_captcha_dialog_create_model ();

  hildon_touch_selector_append_text_column
    (HILDON_TOUCH_SELECTOR (widget), model, TRUE);
  hildon_touch_selector_append_text_column
    (HILDON_TOUCH_SELECTOR (widget), model, TRUE);
  hildon_touch_selector_append_text_column
    (HILDON_TOUCH_SELECTOR (widget), model, TRUE);
  hildon_touch_selector_append_text_column
    (HILDON_TOUCH_SELECTOR (widget), model, TRUE);
  hildon_touch_selector_append_text_column
    (HILDON_TOUCH_SELECTOR (widget), model, TRUE);

  g_signal_connect_swapped
    (widget, "changed",
     G_CALLBACK (aw_captcha_dialog_selector_changed_cb), dialog);

  g_object_unref (model);

  gtk_widget_show_all (table);
}

static void
aw_captcha_dialog_class_init (AwCaptchaDialogClass *class)
{
  g_type_class_add_private (class, sizeof (AwCaptchaDialogPrivate));
}

GtkWidget *
aw_captcha_dialog_new (GtkWindow *window)
{
  g_return_val_if_fail (GTK_IS_WINDOW (window) || !window, NULL);

  return gtk_widget_new (AW_TYPE_CAPTCHA_DIALOG,
                         "transient-for", window,
                         "destroy-with-parent", TRUE,
                         "title", _("Security Measure"),
                         "modal", window ? TRUE : FALSE, NULL);
}

void
aw_captcha_dialog_set_challenge (AwCaptchaDialog *dialog,
                                 GdkPixbuf       *challenge)
{
  GdkPixbuf *scaled = NULL;

  g_return_if_fail (AW_IS_CAPTCHA_DIALOG (dialog));
  g_return_if_fail (GDK_IS_PIXBUF (challenge) || !challenge);

  if (challenge)
    {
      int    cx, cy, dx, dy;
      double sx, sy;

      cx = gdk_pixbuf_get_width (challenge);
      cy = gdk_pixbuf_get_height (challenge);

      sy = (double) AW_CAPTCHA_HEIGHT / cy;
      sx = (double) AW_CAPTCHA_WIDTH / cx;

      dx = MIN (sx, sy) * cx;
      dy = MIN (sx, sy) * cy;

      if (cx != dx && cy != dx)
        {
          scaled = gdk_pixbuf_scale_simple (challenge, dx, dy, GDK_INTERP_TILES);
          challenge = scaled;
        }
    }

  gtk_image_set_from_pixbuf (GTK_IMAGE (dialog->priv->challenge), challenge);

  if (scaled)
    g_object_unref (scaled);
}

GdkPixbuf *
aw_captcha_dialog_get_challenge (AwCaptchaDialog *dialog)
{
  g_return_val_if_fail (AW_IS_CAPTCHA_DIALOG (dialog), NULL);
  return gtk_image_get_pixbuf (GTK_IMAGE (dialog->priv->challenge));
}

void
aw_captcha_dialog_set_response (AwCaptchaDialog *dialog,
                                const char      *text)
{
  g_return_if_fail (AW_IS_CAPTCHA_DIALOG (dialog));
  gtk_entry_set_text (GTK_ENTRY (dialog->priv->response), text);
  gtk_editable_set_position (GTK_EDITABLE (dialog->priv->response), -1);
}

G_CONST_RETURN char *
aw_captcha_dialog_get_response (AwCaptchaDialog *dialog)
{
  g_return_val_if_fail (AW_IS_CAPTCHA_DIALOG (dialog), NULL);
  return gtk_entry_get_text (GTK_ENTRY (dialog->priv->response));
}

