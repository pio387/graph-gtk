#ifndef __GRAPH_GTK_NODE_H__
#define __GRAPH_GTK_NODE_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include "graph-gtk-pad.h"

G_BEGIN_DECLS

#define GRAPH_TYPE_GTK_NODE                                             \
   (graph_gtk_node_get_type())
#define GRAPH_GTK_NODE(obj)                                             \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                GRAPH_TYPE_GTK_NODE,                    \
                                GraphGtkNode))
#define GRAPH_GTK_NODE_CLASS(klass)                                     \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             GRAPH_TYPE_GTK_NODE,                       \
                             GraphGtkNodeClass))
#define IS_GRAPH_GTK_NODE(obj)                                          \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                GRAPH_TYPE_GTK_NODE))
#define IS_GRAPH_GTK_NODE_CLASS(klass)                                  \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             GRAPH_TYPE_GTK_NODE))
#define GRAPH_GTK_NODE_GET_CLASS(obj)                                   \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               GRAPH_TYPE_GTK_NODE,                     \
                               GraphGtkNodeClass))

struct _GraphGtkView;
typedef struct _GraphGtkView GraphGtkView;

typedef struct _GraphGtkNode      GraphGtkNode;
typedef struct _GraphGtkNodeClass GraphGtkNodeClass;

struct _GraphGtkNodeClass
{
  GObjectClass parent_class;
  void (*render_node) (GraphGtkNode* self, cairo_t* cairo);
  gboolean (*recalculate_size) (GraphGtkNode *self);
};

struct _GraphGtkNode
{
  GObject parent;

  GraphGtkView *view;

  GList *input_pads;
  GList *output_pads;

  const gchar *name;

  gpointer user_data; //intended to be used by the user, e.g. for tying GraphGtkNodes to GeglNodes. Not used for anything internally

  gboolean resizable;

  /* Rendering state */
  gint x, y; //coordinate position
  gint offset_x, offset_y; //drag offset
  gint width, height;
  gboolean is_selected; //set and unset by the GraphGtkView
  cairo_surface_t *image;
  gint img_width, img_height; //-1 meant default or scale based on other
  gboolean show_image;
  gboolean failed_size_calculation;
};

GType graph_gtk_node_get_type (void) G_GNUC_CONST;

GraphGtkNode*	graph_gtk_node_new();
void		graph_gtk_node_set_name(GraphGtkNode* self, const gchar* name);
const gchar*	graph_gtk_node_get_name(GraphGtkNode* self);
void		graph_gtk_node_render(GraphGtkNode* self, cairo_t* cairo);
void		graph_gtk_node_add_pad(GraphGtkNode* self, const gchar* pad_name, gboolean output);
void graph_gtk_node_remove_pads(GraphGtkNode *self);
GList*		graph_gtk_node_get_pads(GraphGtkNode* self);
GList*		graph_gtk_node_get_input_pads(GraphGtkNode* self);
GList*		graph_gtk_node_get_output_pads(GraphGtkNode* self);
void		graph_gtk_node_connect_to(GraphGtkNode* source, const gchar* output_pad, GraphGtkNode* sink, const gchar* input_pad);
gboolean	graph_gtk_node_recalculate_size(GraphGtkNode* self);
gboolean	graph_gtk_node_is_within(GraphGtkNode* self, int x, int y);	//check whether this position is within the node
GraphGtkPad*	graph_gtk_node_is_on_pad(GraphGtkNode* self, int x, int y);	//check whether this position is within a pad on this node
void		graph_gtk_node_set_image(GraphGtkNode* self, cairo_surface_t* image);
void		graph_gtk_node_set_image_size(GraphGtkNode* self, gint width, gint height);
void		graph_gtk_node_show_image(GraphGtkNode* self, gboolean show_image);

G_END_DECLS

#endif /* __GRAPH_GTK_NODE_H__ */
