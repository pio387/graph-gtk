#ifndef __GRAPH_GTK_VIEW_H__
#define __GRAPH_GTK_VIEW_H__

#include <gtk/gtk.h>

/**
   GraphGtkView:

   GraphGtkView::nodes-connected
   GraphGtkView::nodes-disconnected
   GraphGtkView::node-selected
   GraphGtkView::node-deselected
 */

G_BEGIN_DECLS

#define GRAPH_TYPE_GTK_VIEW                                             \
   (graph_gtk_view_get_type())
#define GRAPH_GTK_VIEW(obj)                                             \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                GRAPH_TYPE_GTK_VIEW,                    \
                                GraphGtkView))
#define GRAPH_GTK_VIEW_CLASS(klass)                                     \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             GRAPH_TYPE_GTK_VIEW,                       \
                             GraphGtkViewClass))
#define IS_GRAPH_GTK_VIEW(obj)                                          \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                GRAPH_TYPE_GTK_VIEW))
#define IS_GRAPH_GTK_VIEW_CLASS(klass)                                  \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             GRAPH_TYPE_GTK_VIEW))
#define GRAPH_GTK_VIEW_GET_CLASS(obj)                                   \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               GRAPH_TYPE_GTK_VIEW,                     \
                               GraphGtkViewClass))

struct _GraphGtkPad;
typedef struct _GraphGtkPad GraphGtkPad;

struct _GraphGtkNode;
typedef struct _GraphGtkNode GraphGtkNode;

typedef struct _GraphGtkView      GraphGtkView;
typedef struct _GraphGtkViewClass GraphGtkViewClass;

struct _GraphGtkViewClass
{
  GtkDrawingAreaClass parent_class;
};

struct _GraphGtkView
{
  GtkDrawingArea parent;

  GList* nodes;

  /* private state management */
  GList* selected_nodes; //list of currently selected nodes

  gboolean is_mouse_connecting;
  GraphGtkPad* pad_connecting_from;

  gdouble mouse_x, mouse_y;

  gboolean is_mouse_dragging; //mouse was pressed on a node and has not yet been released
  gint drag_begin_x, drag_begin_y; //mouse coordinates in screen space when dragging began

  gboolean is_mouse_panning; //mouse was pressed on the canvas and has not yet been released
  gint pan_x, pan_y; //current pan offset
  gint pan_begin_x, pan_begin_y;

  cairo_surface_t *bg;
};

GType graph_gtk_view_get_type (void) G_GNUC_CONST;

GtkWidget*	graph_gtk_view_new(void);
void		graph_gtk_view_add_node(GraphGtkView* self, GraphGtkNode* node);
void		graph_gtk_view_remove_node(GraphGtkView* self, GraphGtkNode* node);
void		graph_gtk_view_remove_selected_nodes(GraphGtkView* self);
void		graph_gtk_view_clear(GraphGtkView* self);
GList*		graph_gtk_view_get_nodes(GraphGtkView* self);
void graph_gtk_view_set_bg(GraphGtkView* self, cairo_surface_t* bg); //NULL means no background should be rendered

G_END_DECLS

#endif /* __GRAPH_GTK_VIEW_H__ */
