#include "graph-gtk-node.h"

static void graph_gtk_node_dispose (GObject *object);
static void graph_gtk_node_finalize (GObject *object);
static void graph_gtk_node_render_default(GraphGtkNode* self, cairo_t* cairo);

G_DEFINE_TYPE (GraphGtkNode, graph_gtk_node, G_TYPE_OBJECT);

static void
graph_gtk_node_class_init (GraphGtkNodeClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;

  gobject_class->dispose = graph_gtk_node_dispose;
  gobject_class->finalize = graph_gtk_node_finalize;

  klass->render_node = graph_gtk_node_render_default;
}

static void
graph_gtk_node_init (GraphGtkNode *self)
{
}

static void
graph_gtk_node_dispose (GObject *object)
{
  GraphGtkNode *self = (GraphGtkNode *)object;

  G_OBJECT_CLASS (graph_gtk_node_parent_class)->dispose (object);
}

static void
graph_gtk_node_finalize (GObject *object)
{
  GraphGtkNode *self = (GraphGtkNode *)object;

  g_signal_handlers_destroy (object);
  G_OBJECT_CLASS (graph_gtk_node_parent_class)->finalize (object);
}

static void
graph_gtk_node_render_default(GraphGtkNode* self, cairo_t* cairo)
{
  
}

void
graph_gtk_node_render(GraphGtkNode* self, cairo_t* cairo)
{
  g_return_if_fail(IS_GRAPH_GTK_NODE(self));

  GRAPH_GTK_NODE_GET_CLASS(self)->render_node(self, cairo);
}
