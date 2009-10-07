#ifndef __AW_TREE_VIEW_H__
#define __AW_TREE_VIEW_H__ 1

#include <hildon/hildon.h>

#define AW_TYPE_TREE_VIEW                 (aw_tree_view_get_type ())
#define AW_TREE_VIEW(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), AW_TYPE_TREE_VIEW, AwTreeView))
#define AW_TREE_VIEW_CLASS(cls)           (G_TYPE_CHECK_CLASS_CAST ((cls), AW_TYPE_TREE_VIEW, AwTreeViewClass))
#define AW_IS_TREE_VIEW(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AW_TYPE_TREE_VIEW))
#define AW_IS_TREE_VIEW_CLASS(obj)        (G_TYPE_CHECK_CLASS_TYPE ((obj), AW_TYPE_TREE_VIEW))
#define AW_TREE_VIEW_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), AW_TYPE_TREE_VIEW, AwTreeViewClass))

typedef enum {
  AW_TREE_VIEW_COLOR_PRIMARY,
  AW_TREE_VIEW_COLOR_SECONDARY,
  AW_TREE_VIEW_COLOR_ATTENTION,
  AW_TREE_VIEW_COLOR_LAST,
} AwTreeViewColorName;

typedef enum {
  AW_TREE_VIEW_FONT_PRIMARY,
  AW_TREE_VIEW_FONT_SECONDARY,
  AW_TREE_VIEW_FONT_ITALIC,
  AW_TREE_VIEW_FONT_BOLD,
  AW_TREE_VIEW_FONT_LAST,
} AwTreeViewFontName;

typedef struct _AwTreeView                AwTreeView;
typedef struct _AwTreeViewPrivate         AwTreeViewPrivate;
typedef struct _AwTreeViewClass           AwTreeViewClass;

struct _AwTreeView {
  GtkTreeView        parent;
  AwTreeViewPrivate *priv;
};

struct _AwTreeViewClass {
  GtkTreeViewClass        parent_class;

  /* vtable */
  GType *      (* get_core_model) (AwTreeView *view);
  const char * (* get_empty_text) (AwTreeView *view);
  void         (* create_columns) (AwTreeView *view);
  GSList *     (* get_filters)    (AwTreeView *view);
  GSList *     (* get_menu_items) (AwTreeView *view);
};

GType
aw_tree_view_get_type                (void) G_GNUC_CONST;

GtkWidget *
aw_tree_view_new                     (HildonUIMode           ui_mode);

GtkWidget *
aw_tree_view_new_with_model          (HildonUIMode           ui_mode,
                                      GtkTreeModel          *model);

void
aw_tree_view_set_empty_text          (AwTreeView            *view,
                                      const char            *text);

G_CONST_RETURN char *
aw_tree_view_get_empty_text          (AwTreeView            *view);

GSList *
aw_tree_view_get_filters             (AwTreeView            *view);

GSList *
aw_tree_view_get_menu_items          (AwTreeView            *view);

void
aw_tree_view_set_sort_func           (AwTreeView            *view,
                                      GtkTreeIterCompareFunc sort_func);

void
aw_tree_view_append_items            (AwTreeView            *view,
                                      GList                 *items);

int
aw_tree_view_accumulate              (AwTreeView            *view,
                                      int                  (*callback)(),
                                      gpointer               user_data);

GtkTreeViewColumn *
aw_tree_view_append_primary_column   (AwTreeView            *view,
                                      GtkCellRenderer       *cell);

GtkTreeViewColumn *
aw_tree_view_append_secondary_column (AwTreeView            *view,
                                      GtkCellRenderer       *cell);

PangoAttrList *
aw_tree_view_insert_color            (AwTreeView            *view,
                                      PangoAttrList         *list,
                                      AwTreeViewColorName    name,
                                      int                    start,
                                      int                    end);

PangoAttrList *
aw_tree_view_insert_font             (AwTreeView            *view,
                                      PangoAttrList         *list,
                                      AwTreeViewFontName     name,
                                      int                    start,
                                      int                    end);

#endif /* __AW_TREE_VIEW_H__ */


