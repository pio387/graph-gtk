#include "graph-gtk-connection.h"


static void graph_gtk_connection_dispose (GObject *object);
static void graph_gtk_connection_finalize (GObject *object);
static void graph_gtk_connection_default_render(GraphGtkConnection* self, cairo_t* cairo);

G_DEFINE_TYPE (GraphGtkConnection, graph_gtk_connection, G_TYPE_OBJECT);

static void
graph_gtk_connection_class_init (GraphGtkConnectionClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;

  gobject_class->dispose = graph_gtk_connection_dispose;
  gobject_class->finalize = graph_gtk_connection_finalize;

  klass->render_connection = graph_gtk_connection_default_render;
}

static void
graph_gtk_connection_init (GraphGtkConnection *self)
{
}

static void
graph_gtk_connection_dispose (GObject *object)
{
  GraphGtkConnection *self = (GraphGtkConnection *)object;

  G_OBJECT_CLASS (graph_gtk_connection_parent_class)->dispose (object);
}

static void
graph_gtk_connection_finalize (GObject *object)
{
  GraphGtkConnection *self = (GraphGtkConnection *)object;

  g_signal_handlers_destroy (object);
  G_OBJECT_CLASS (graph_gtk_connection_parent_class)->finalize (object);
}

static void 
graph_gtk_connection_default_render(GraphGtkConnection* self, cairo_t* cr)
{
  int from_x, from_y, to_x, to_y;
  graph_gtk_pad_get_position(self->source, &from_x, &from_y);
  graph_gtk_pad_get_position(self->sink, &to_x, &to_y);


  cairo_set_line_width(cr, 1);
  cairo_set_source_rgb(cr, 0, 0, 0);

  cairo_move_to(cr, from_x, from_y);

  if(to_x - from_x > 200)
    cairo_curve_to(cr, (from_x+to_x)/2, from_y,
		   (from_x+to_x)/2, to_y,
		   to_x, to_y);
  else
    cairo_curve_to(cr, from_x+100, from_y,
		   to_x-100, to_y,
		   to_x, to_y);

  //cairo_line_to(cr, to_x, to_y);

  cairo_stroke(cr);
}

void 
graph_gtk_connection_render(GraphGtkConnection* self, cairo_t* cairo)
{
  g_return_if_fail(IS_GRAPH_GTK_CONNECTION(self));

  GRAPH_GTK_CONNECTION_GET_CLASS(self)->render_connection(self, cairo);
}

GraphGtkConnection*
graph_gtk_connection_new(GraphGtkPad* source, GraphGtkPad* sink)
{
  GraphGtkConnection* connection = GRAPH_GTK_CONNECTION(g_object_new(GRAPH_TYPE_GTK_CONNECTION, NULL));
  connection->source = source;
  connection->sink = sink;

  return connection;
}
