#ifndef __GRAPH_GTK_NODE_H__
#define __GRAPH_GTK_NODE_H__

#include <glib-object.h>


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

typedef struct _GraphGtkNode      GraphGtkNode;
typedef struct _GraphGtkNodeClass GraphGtkNodeClass;

struct _GraphGtkNodeClass
{
    GObjectClass parent_class;
};

struct _GraphGtkNode
{
    GObject parent;

  GSList* input_pads;
  GSList* output_pads;

  const gchar* name;

  gboolean resizable;

  void (*render_node) (GraphGtkNode* self, cairo_t* cairo) = NULL;

  /* Rendering state */
  gint x, y; //coordinate position
  gint width, height;
};

GType graph_gtk_node_get_type (void) G_GNUC_CONST;

void graph_gtk_node_render(GraphGtkNode* self, cairo_t* cairo);
GSList* graph_gtk_node_get_pads(GraphGtkNode* self);
GSList* graph_gtk_node_get_input_pads(GraphGtkNode* self);
GSList* graph_gtk_node_get_output_pads(GraphGtkNode* self);
void graph_gtk_node_connect_to(GraphGtkNode* source, const gchar* output_pad, GraphGtkNode* sink, const gchar* input_pad);

G_END_DECLS

#endif /* __GRAPH_GTK_NODE_H__ */
