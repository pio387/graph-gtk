#include "graph-gtk-node.h"
#include "graph-gtk-pad.h"

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
  //Draw the node with cairo
}

void
graph_gtk_node_render(GraphGtkNode* self, cairo_t* cairo)
{
  g_return_if_fail(IS_GRAPH_GTK_NODE(self));

  GRAPH_GTK_NODE_GET_CLASS(self)->render_node(self, cairo);
}

GSList*
graph_gtk_node_get_pads(GraphGtkNode* self)
{
  return g_slist_concat(self->input_pads, self->output_pads);
}

GSList* 
graph_gtk_node_get_input_pads(GraphGtkNode* self)
{
  return g_slist_copy(self->input_pads);
}

GSList* 
graph_gtk_node_get_output_pads(GraphGtkNode* self)
{
  return g_slist_copy(self->output_pads);
}

void
graph_gtk_node_connect_to(GraphGtkNode* source, const gchar* output_pad, GraphGtkNode* sink, const gchar* input_pad)
{
  GraphGtkPad *source_pad = NULL, *sink_pad = NULL;

  GSList* list;
  for(list = graph_gtk_node_get_pads(source); list != NULL; list++)
    {
      GraphGtkPad *pad = (GraphGtkPad*)list->data;
      if(g_strcmp0(pad->name, output_pad))
	{
	  source_pad = pad;
	  break;
	}
    }

  for(list = graph_gtk_node_get_pads(sink); list != NULL; list++)
    {
      GraphGtkPad *pad = (GraphGtkPad*)list->data;
      if(g_strcmp0(pad->name, input_pad))
	{
	  sink_pad = pad;
	  break;
	}
    }

  if(!source_pad || !sink_pad)
    return;

  graph_gtk_pad_connect_to(source_pad, sink_pad);
}

void
graph_gtk_node_recalculate_size(GraphGtkNode* source)
{
  //Recalculate size of the node based on number of pads and pad-name lengths
  //...
}
