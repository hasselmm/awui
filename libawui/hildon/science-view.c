#include "config.h"
#include "science-view.h"

#include <libawui/model.h>
#include <glib/gi18n-lib.h>

enum {
  PROP_0,
  PROP_SCIENCE_ID,
  PROP_SCIENCE,
};

struct _AwScienceView {
  GtkAlignment    parent;

  AwScience      *sciences[AW_SCIENCE_LAST];
  GtkWidget      *buttons[AW_SCIENCE_LAST];
  GtkWidget      *labels[AW_SCIENCE_LAST];
  GtkWidget      *science_stats;
  GtkWidget      *culture_stats;

  AwScienceId     science_id;
  AwScienceStats  stats;
};

struct _AwScienceViewClass {
  GtkAlignmentClass parent_class;
};

static GObjectClass *object_parent_class;

G_DEFINE_TYPE (AwScienceView, aw_science_view, GTK_TYPE_ALIGNMENT);

static gboolean
aw_science_view_box_expose_cb (GtkWidget      *widget,
                               GdkEventExpose *event,
                               AwScience      *science)
{
// FIXME: cleanup
  double   p, x;
  cairo_t *cr;

  cr = gdk_cairo_create (event->window);
  gdk_cairo_region (cr, event->region);
  cairo_clip (cr);

  x = aw_science_get_research_cost (aw_science_get_id (science),
                                    aw_science_get_level (science));
  p = 1 - aw_science_get_remaining (science) / x;

GdkColor *c; // FIXME
  cairo_rectangle (cr, widget->allocation.x, widget->allocation.y,
                   widget->allocation.width, widget->allocation.height);
  c = &widget->style->fg[widget->state];
  cairo_set_source_rgba (cr, c->red/65535., c->green/65535., c->blue/65535., 0.6);

  cairo_fill (cr);

  cairo_rectangle (cr, widget->allocation.x + 1, widget->allocation.y + 1,
                   widget->allocation.width - 2, widget->allocation.height - 2);
  gdk_cairo_set_source_color (cr, &widget->style->bg[GTK_STATE_NORMAL]);

  cairo_fill (cr);

  cairo_rectangle (cr, widget->allocation.x + 1, widget->allocation.y + 1,
                   p * (widget->allocation.width - 2), widget->allocation.height - 2);
// FIXME: use proper color
  c = &widget->style->bg[GTK_STATE_SELECTED];
  cairo_set_source_rgba (cr, c->red/65535., c->green/65535., c->blue/65535., 1);

  cairo_fill (cr);

  cairo_destroy (cr);

  return FALSE;
}

static void
aw_science_view_update_science_label (AwScienceView   *view,
                                      const AwScience *science)
{
  int        remaining, output, bonus, minutes;
  GString   *text = g_string_new (NULL);
  GtkWidget *label;

  remaining = aw_science_get_remaining (science);

  if (aw_science_get_id (science) < AW_SCIENCE_CULTURE)
    {
      output = view->stats.science_output;
      bonus = view->stats.science_bonus;
    }
  else
    {
      output = view->stats.culture_output;
      bonus = view->stats.culture_bonus;
    }

  g_string_printf (text, "%s: %d, %s: %d",
                   _("level"), aw_science_get_level (science),
                   _("remaining"), remaining);

  if (output > 0)
    {
      output  = (bonus + 100) * output;
      minutes = (remaining * 6000 + 5999) / output;

      g_string_append_printf (text, " (%d:%02d)",
                              minutes / 60, minutes % 60);
    }

  label = view->labels[aw_science_get_id (science)];
  gtk_label_set_text (GTK_LABEL (label), text->str);
  g_string_free (text, TRUE);
}

static GtkWidget *
aw_science_view_box_create (AwScienceView *view,
                            AwScience     *science,
                            const char    *title,
                            GtkWidget    **stats_label)
{
  GtkWidget      *alignment, *box, *label;
  PangoAttrList  *attr_list;
  PangoAttribute *attribute;

  if (!title && science)
    title = aw_science_get_name (science);

  alignment = gtk_alignment_new (0.5, 0.5, 1.0, 0.0);

  box = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (alignment), box);

  label = gtk_label_new (title);
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);

  if (stats_label)
    {
      attr_list = pango_attr_list_new ();
      attribute = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
      pango_attr_list_insert (attr_list, attribute);
      gtk_label_set_attributes (GTK_LABEL (label), attr_list);
      pango_attr_list_unref (attr_list);
    }

  if (science)
    {
      label = gtk_label_new (NULL);
      gtk_box_pack_start (GTK_BOX (box), label, TRUE, TRUE, 0);

      hildon_helper_set_logical_font (label, "SmallSystemFont");

      g_signal_connect (label, "expose-event",
                        G_CALLBACK (aw_science_view_box_expose_cb),
                        science);

      view->labels[aw_science_get_id (science)] = label;
      aw_science_view_update_science_label (view, science);
    }

  if (stats_label)
    {
      label = gtk_label_new (NULL);
      gtk_box_pack_end (GTK_BOX (box), label, FALSE, FALSE, 0);

      hildon_helper_set_logical_font (label, "SmallSystemFont");
      hildon_helper_set_logical_color (label, GTK_RC_FG, GTK_STATE_NORMAL,
                                       "SecondaryTextColor");
      hildon_helper_set_logical_color (label, GTK_RC_FG, GTK_STATE_PRELIGHT,
                                       "SecondaryTextColor");

      *stats_label = label;
    }

  return alignment;
}

static void
aw_science_view_button_toggled_cb (GtkWidget *button,
                                   gpointer   user_data)
{
  GtkWidget *view;

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
    {
      view = gtk_widget_get_ancestor (button, AW_TYPE_SCIENCE_VIEW);
      AW_SCIENCE_VIEW (view)->science_id = GPOINTER_TO_UINT (user_data);
      g_object_notify (G_OBJECT (view), "science-id");
    }
}

static void
aw_science_view_init (AwScienceView *view)
{
  GtkWidget    *table, *widget;
  AwScienceId   id;

  for (id = 0; id < AW_SCIENCE_LAST; ++id)
    view->sciences[id] = aw_science_new (id, 0, 0);

  gtk_alignment_set_padding (GTK_ALIGNMENT (view),
                             0, 0, HILDON_MARGIN_DOUBLE,
                             HILDON_MARGIN_DOUBLE);
  gtk_alignment_set (GTK_ALIGNMENT (view), 0.5, 0.0, 1.0, 0.0);

  table = gtk_table_new (5, 2, TRUE);
  gtk_container_add (GTK_CONTAINER (view), table);
  gtk_table_set_row_spacing (GTK_TABLE (table), 3, HILDON_MARGIN_TRIPLE);

  widget = aw_science_view_box_create (view, NULL, _("Science"),
                                       &view->science_stats);

  gtk_table_attach (GTK_TABLE (table), widget, 0, 2, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 0, 0);

  for (id = 0; id < AW_SCIENCE_CULTURE; ++id)
    {
      int col = id / 3;
      int row = id % 3 + 1;

      widget = hildon_gtk_radio_button_new_from_widget
        (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT,
         id ? GTK_RADIO_BUTTON (view->buttons[id - 1]) : NULL);

      gtk_table_attach (GTK_TABLE (table), widget,
                        col, col + 1, row, row + 1,
                        GTK_FILL | GTK_EXPAND, 0, 0, 0);

      gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (widget), FALSE);

      if (id == view->science_id)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);

      g_signal_connect (widget, "toggled",
                        G_CALLBACK (aw_science_view_button_toggled_cb),
                        GUINT_TO_POINTER (id));

      view->buttons[id] = widget;

      widget = aw_science_view_box_create (view, view->sciences[id], NULL, NULL);
      gtk_container_add (GTK_CONTAINER (view->buttons[id]), widget);
    }

  widget = aw_science_view_box_create (view, view->sciences[id],
                                       NULL, &view->culture_stats);
  gtk_table_attach (GTK_TABLE (table), widget, 0, 2, 4, 5,
                    GTK_FILL | GTK_EXPAND, 0, 0, 0);

  aw_science_view_set_statistics (view, NULL);

  gtk_widget_show_all (table);
}

static void
aw_science_view_finalize (GObject *object)
{
  AwScienceView *view;
  int            i;

  view = AW_SCIENCE_VIEW (object);

  for (i = 0; i < G_N_ELEMENTS (view->sciences); ++i)
    aw_science_unref (view->sciences[i]);

  object_parent_class->finalize (object);
}

static void
aw_science_view_set_property (GObject      *object,
                              unsigned      prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  AwScienceView *view = AW_SCIENCE_VIEW (object);

  switch (prop_id) {
  case PROP_SCIENCE_ID:
    aw_science_view_set_science_id (view, g_value_get_enum (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}
static void
aw_science_view_get_property (GObject    *object,
                              unsigned    prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  AwScienceView *view = AW_SCIENCE_VIEW (object);

  switch (prop_id) {
  case PROP_SCIENCE_ID:
    g_value_set_enum (value, view->science_id);
    break;
  case PROP_SCIENCE:
    g_value_set_boxed (value, view->sciences[view->science_id]);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
aw_science_view_class_init (AwScienceViewClass *class)
{
  GObjectClass *object_class;

  object_class               = G_OBJECT_CLASS (class);
  object_class->finalize     = aw_science_view_finalize;
  object_class->set_property = aw_science_view_set_property;
  object_class->get_property = aw_science_view_get_property;

  g_object_class_install_property
    (object_class, PROP_SCIENCE_ID,
     g_param_spec_enum ("science-id",
                        "Science Id",
                        "Id of the currently selected science",
                        AW_TYPE_SCIENCE_ID,
                        AW_SCIENCE_INVALID,
                        G_PARAM_READWRITE |
                        G_PARAM_STATIC_STRINGS));

  g_object_class_install_property
    (object_class, PROP_SCIENCE,
     g_param_spec_boxed ("science",
                         "Science",
                         "The currently selected science",
                         AW_TYPE_SCIENCE,
                         G_PARAM_READABLE |
                         G_PARAM_STATIC_STRINGS));

  object_parent_class = G_OBJECT_CLASS (aw_science_view_parent_class);
}

GtkWidget *
aw_science_view_new (void)
{
  return gtk_widget_new (AW_TYPE_SCIENCE_VIEW, NULL);
}

void
aw_science_view_set_sciences (AwScienceView *view,
                              GList         *sciences)
{
  g_return_if_fail (AW_IS_SCIENCE_VIEW (view));

  while (sciences)
    {
      AwScience   *science = sciences->data;
      AwScienceId  id = AW_SCIENCE_INVALID;

      if (science)
        id = aw_science_get_id (science);

      if (id >= 0 && id < G_N_ELEMENTS (view->sciences))
        {
          aw_science_set (view->sciences[id],
                          aw_science_get_level (science),
                          aw_science_get_remaining (science));

          aw_science_view_update_science_label (view,
                                                view->sciences[id]);
        }

      sciences = sciences->next;
    }
}

static void
aw_science_view_update_stats_label (GtkLabel *label,
                                    int       output,
                                    int       bonus)
{
  GString *text = g_string_new (NULL);

  g_string_append_printf (text, _("%+d per hour"), output);

  if (bonus)
    g_string_append_printf (text, " (%+d%%)", bonus);

  gtk_label_set_text (label, text->str);
  g_string_free (text, TRUE);
}

void
aw_science_view_set_statistics (AwScienceView        *view,
                                const AwScienceStats *stats)
{
  AwScienceId id;

  g_return_if_fail (AW_IS_SCIENCE_VIEW (view));

  if (stats)
    memcpy (&view->stats, stats, sizeof view->stats);
  else
    memset (&view->stats, 0, sizeof view->stats);

  aw_science_view_update_stats_label (GTK_LABEL (view->science_stats),
                                      view->stats.science_output,
                                      view->stats.science_bonus);
  aw_science_view_update_stats_label (GTK_LABEL (view->culture_stats),
                                      view->stats.culture_output,
                                      view->stats.culture_bonus);

  for (id = 0; id < G_N_ELEMENTS (view->sciences); ++id)
    aw_science_view_update_science_label (view, view->sciences[id]);
}

void
aw_science_view_set_science_id (AwScienceView *view,
                                AwScienceId    id)
{
  g_return_if_fail (AW_IS_SCIENCE_VIEW (view));

  g_return_if_fail (id >= 0);
  g_return_if_fail (id < G_N_ELEMENTS (view->sciences));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (view->buttons[id]), TRUE);
}

AwScienceId
aw_science_view_get_science_id (AwScienceView *view)
{
  g_return_val_if_fail (AW_IS_SCIENCE_VIEW (view), AW_SCIENCE_INVALID);
  return view->science_id;
}

G_CONST_RETURN AwScience *
aw_science_view_get_science (AwScienceView *view)
{
  g_return_val_if_fail (AW_IS_SCIENCE_VIEW (view), NULL);
  return view->sciences[view->science_id];
}

