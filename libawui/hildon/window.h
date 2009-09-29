#ifndef __AW_WINDOW_H__
#define __AW_WINDOW_H__ 1

#include <hildon/hildon.h>

#define AW_TYPE_WINDOW             (aw_window_get_type ())
#define AW_WINDOW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_WINDOW, AwWindow))
#define AW_WINDOW_CLASS(cls)       (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_WINDOW, AwWindowClass))
#define AW_IS_WINDOW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_WINDOW))
#define AW_IS_WINDOW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_WINDOW))
#define AW_WINDOW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_WINDOW, AwWindowClass))

typedef struct _AwWindow            AwWindow;
typedef struct _AwWindowPrivate     AwWindowPrivate;
typedef struct _AwWindowClass       AwWindowClass;

struct _AwWindow {
  HildonStackableWindow  parent;
  AwWindowPrivate       *priv;
};

struct _AwWindowClass {
  HildonStackableWindowClass parent_class;
};

GType
aw_window_get_type (void) G_GNUC_CONST;

GtkWidget *
aw_window_new      (const char *title);

#endif /* __AW_WINDOW_H__ */


