#include "config.h"
#include "spend-all-dialog.h"

#include <glib/gi18n-lib.h>
#include <hildon/hildon.h>
#include <libawui/model.h>

enum {
  COL_TITLE,
  COL_ITEM,
  COL_COUNT,
};

typedef enum {
  PROP_PRODUCTION_POINTS = 1,
} AwSpendAllDialogPropId;

struct _AwSpendAllDialogPrivate {
  int production_points;
};

G_DEFINE_TYPE (AwSpendAllDialog, aw_spend_all_dialog, HILDON_TYPE_PICKER_DIALOG);

static void
aw_spend_all_dialog_init (AwSpendAllDialog *dialog)
{
  HildonTouchSelector *selector;
  GtkListStore        *store;
  AwProfile           *profile;
  GtkCellRenderer     *cell;
  AwItemType           item;
  const char          *title;

  dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog, AW_TYPE_SPEND_ALL_DIALOG,
                                              AwSpendAllDialogPrivate);

  profile = aw_profile_get_self ();
  store = gtk_list_store_new (COL_COUNT, G_TYPE_STRING, G_TYPE_UINT);

  for (item = 0; item < AW_ITEM_LAST; ++item)
    {
      if (!aw_item_can_spend_all (item) ||
          !aw_item_is_available (item, profile))
        continue;

      title = aw_item_get_display_name (item, 0);
      gtk_list_store_insert_with_values (store, NULL, -1,
                                         COL_TITLE, title,
                                         COL_ITEM, item, -1);
    }

  selector = HILDON_TOUCH_SELECTOR (hildon_touch_selector_new ());
  cell = g_object_new (GTK_TYPE_CELL_RENDERER_TEXT, "xalign", 0.5, NULL);
  hildon_touch_selector_append_column (selector, GTK_TREE_MODEL (store),
                                       cell, "text", 0, NULL);

  g_object_unref (store);
  g_object_unref (profile);

  hildon_picker_dialog_set_selector (HILDON_PICKER_DIALOG (dialog), selector);
}

static void
aw_spend_all_dialog_update_title (AwSpendAllDialog *dialog)
{
  char *title;

  if (dialog->priv->production_points > 0)
    title = g_strdup_printf (_("Spend %d Production Points"),
                             dialog->priv->production_points);
  else
    title = g_strdup (_("Spend All Production Points"));

  gtk_window_set_title (GTK_WINDOW (dialog), title);
  g_free (title);
}

static void
aw_spend_all_dialog_set_property (GObject      *object,
                                  unsigned      prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  AwSpendAllDialog *dialog = AW_SPEND_ALL_DIALOG (object);

  switch ((AwSpendAllDialogPropId) prop_id) {
  case PROP_PRODUCTION_POINTS:
    dialog->priv->production_points = g_value_get_int (value);
    aw_spend_all_dialog_update_title (dialog);
    return;
  }

  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
aw_spend_all_dialog_get_property (GObject    *object,
                                  unsigned    prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  AwSpendAllDialog *dialog = AW_SPEND_ALL_DIALOG (object);

  switch ((AwSpendAllDialogPropId) prop_id) {
  case PROP_PRODUCTION_POINTS:
    g_value_set_int (value, dialog->priv->production_points);
    return;
  }

  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
aw_spend_all_dialog_class_init (AwSpendAllDialogClass *class)
{
  GObjectClass *object_class;

  object_class               = G_OBJECT_CLASS (class);
  object_class->set_property = aw_spend_all_dialog_set_property;
  object_class->get_property = aw_spend_all_dialog_get_property;

  g_object_class_install_property
    (object_class, PROP_PRODUCTION_POINTS,
     g_param_spec_int ("production-points",
                       "Production Points",
                       "Number of production points the player currently has",
                       0, G_MAXINT, 0,
                       G_PARAM_CONSTRUCT |
                       G_PARAM_READWRITE |
                       G_PARAM_STATIC_STRINGS));

  g_type_class_add_private (class, sizeof (AwSpendAllDialogPrivate));
}

GtkWidget *
aw_spend_all_dialog_new (GtkWindow *window)
{
  g_return_val_if_fail (GTK_IS_WINDOW (window) || !window, NULL);

  return gtk_widget_new (AW_TYPE_SPEND_ALL_DIALOG,
                         "transient-for", window,
                         "destroy-with-parent", TRUE,
                         "modal", window ? TRUE : FALSE, NULL);
}

void
aw_spend_all_dialog_set_production_points (AwSpendAllDialog *dialog,
                                           int               points)
{
  g_return_if_fail (AW_IS_SPEND_ALL_DIALOG (dialog));
  g_object_set (dialog, "production-points", points, NULL);
}

int
aw_spend_all_dialog_get_production_points (AwSpendAllDialog *dialog)
{
  g_return_val_if_fail (AW_IS_SPEND_ALL_DIALOG (dialog), -1);
  return dialog->priv->production_points;
}

AwItemType
aw_spend_all_dialog_get_selected_item (AwSpendAllDialog *dialog)
{
  AwItemType           item = AW_ITEM_INVALID;
  HildonTouchSelector *selector;
  GtkTreeModel        *model;
  GtkTreeIter          iter;

  g_return_val_if_fail (AW_IS_SPEND_ALL_DIALOG (dialog), AW_ITEM_INVALID);

  selector = hildon_picker_dialog_get_selector (HILDON_PICKER_DIALOG (dialog));
  model = hildon_touch_selector_get_model (selector, 0);

  if (hildon_touch_selector_get_selected (selector, 0, &iter))
    gtk_tree_model_get (model, &iter, COL_ITEM, &item, -1);

  return item;
}

