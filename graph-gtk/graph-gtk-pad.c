#include "graph-gtk-pad.h"
#include "graph-gtk-connection.h"

static void graph_gtk_pad_dispose (GObject *object);
static void graph_gtk_pad_finalize (GObject *object);
static void graph_gtk_pad_default_render(GraphGtkPad* self, cairo_t* cairo);

G_DEFINE_TYPE (GraphGtkPad, graph_gtk_pad, G_TYPE_OBJECT);

static void
graph_gtk_pad_class_init (GraphGtkPadClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;

  gobject_class->dispose = graph_gtk_pad_dispose;
  gobject_class->finalize = graph_gtk_pad_finalize;

  klass->render_pad = graph_gtk_pad_default_render;
}

static void
graph_gtk_pad_init (GraphGtkPad *self)
{
}

static void
graph_gtk_pad_dispose (GObject *object)
{
  GraphGtkPad *self = (GraphGtkPad *)object;

  G_OBJECT_CLASS (graph_gtk_pad_parent_class)->dispose (object);
}

static void
graph_gtk_pad_finalize (GObject *object)
{
  GraphGtkPad *self = (GraphGtkPad *)object;

  g_signal_handlers_destroy (object);
  G_OBJECT_CLASS (graph_gtk_pad_parent_class)->finalize (object);
}

static void
graph_gtk_pad_default_render(GraphGtkPad* self, cairo_t* cairo)
{
  
}

GraphGtkPad*
graph_gtk_pad_new(const gchar* name, gboolean is_output)
{
  GraphGtkPad *pad = GRAPH_GTK_PAD(g_object_new(GRAPH_TYPE_GTK_PAD, NULL));
  pad->name = name;
  pad->is_output = is_output;
  return pad;
}

void
graph_gtk_pad_render(GraphGtkPad* self, cairo_t* cairo)
{
  g_return_if_fail(IS_GRAPH_GTK_PAD(self));

  GRAPH_GTK_PAD_GET_CLASS(self)->render_pad(self, cairo);
}

void
graph_gtk_pad_get_position(GraphGtkPad* self, int *x, int *y)
{
  g_return_if_fail(IS_GRAPH_GTK_PAD(self));

  *x = self->rel_x+self->node->x;
  *y = self->rel_y+self->node->y;
}

gboolean
graph_gtk_pad_is_connected_to(GraphGtkPad* self, GraphGtkPad* other)
{
  if((self->is_output && other->is_output) || (!self->is_output && !other->is_output))
    return FALSE;

  GSList* list;
  for(list = self->connections; list != NULL; list++)
    {
      GraphGtkConnection *connection = (GraphGtkConnection*)list->data;
      GraphGtkPad *pad;
      
      if(self->is_output)
	pad = connection->sink;
      else
	pad = connection->source;

      if(pad == other)
	return TRUE;
    }

  return FALSE;
}

void
graph_gtk_pad_connect_to(GraphGtkPad* source, GraphGtkPad* sink)
{
  if(!source->is_output || sink->is_output)
    {
      g_warning("graph_gtk_pad_connect_to(): can only connect from an output pad to an input pad\n");
      return;
    }

  if(!graph_gtk_pad_is_connected_to(source, sink))
    {
      GraphGtkConnection *connection = graph_gtk_connection_new(source, sink);
      source->connections = g_slist_append(source->connections, connection);

      graph_gtk_pad_disconnect(sink);
      sink->connections = g_slist_append(sink->connections, connection);
    }
}

//Remove all connections from this pad
void
graph_gtk_pad_disconnect(GraphGtkPad* self)
{
  GSList* list;
  for(list = self->connections; list != NULL; list++)
    {
      GraphGtkConnection *connection = (GraphGtkConnection*)list->data;
      GraphGtkPad *other;
      
      if(self->is_output)
	other = connection->sink;
      else
	other = connection->source;

      other->connections = g_slist_remove(other->connections, connection);

      g_object_unref(connection);
    }

  g_slist_free(list);
  self->connections = NULL;
}
