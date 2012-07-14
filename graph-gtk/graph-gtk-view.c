#include "graph-gtk-view.h"


static void graph_gtk_view_dispose (GObject *object);
static void graph_gtk_view_finalize (GObject *object);

static gboolean graph_gtk_view_draw(GtkWidget* self, cairo_t* cairo);

#if GTK_MAJOR_VERSION == (2)
static gboolean graph_gtk_view_expose(GtkWidget* self, GdkEventExpose* event);
#endif

G_DEFINE_TYPE (GraphGtkView, graph_gtk_view, GTK_TYPE_DRAWING_AREA);

static void
graph_gtk_view_class_init (GraphGtkViewClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass *)klass;

  gobject_class->dispose = graph_gtk_view_dispose;
  gobject_class->finalize = graph_gtk_view_finalize;

  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

#if GTK_MAJOR_VERSION == (3)
  widget_class->draw = graph_gtk_view_draw;
#else
  widget_class->expose_event = graph_gtk_view_expose;
#endif

}

static void
graph_gtk_view_init (GraphGtkView *self)
{
}

static void
graph_gtk_view_dispose (GObject *object)
{
  GraphGtkView *self = (GraphGtkView *)object;

  G_OBJECT_CLASS (graph_gtk_view_parent_class)->dispose (object);
}

static void
graph_gtk_view_finalize (GObject *object)
{
  GraphGtkView *self = (GraphGtkView *)object;

  g_signal_handlers_destroy (object);
  G_OBJECT_CLASS (graph_gtk_view_parent_class)->finalize (object);
}

#if GTK_MAJOR_VERSION == (2)
static gboolean
graph_gtk_view_expose (GtkWidget *widget, GdkEventExpose *event)
{
  cairo_t *cr = gdk_cairo_create(widget->window);

  graph_gtk_view_draw(widget, cr);

  cairo_destroy(cr);

  return FALSE;
}
#endif

static gboolean
graph_gtk_view_draw(GtkWidget *widget, cairo_t* cr)
{
  GraphGtkView *view = GRAPH_GTK_VIEW(widget);

  /*
    Need to figure out a clean way of letting the user render a custom background, such as the 
    result of the combined graph operations. This should either be a signal or simply let the
    user set a cairo surface property on the GeglGtkView. For now just render a solid grey background
  */

  cairo_set_source_rgb(cr, 0.82, 0.82, 0.82);
  cairo_paint(cr);

  //render the graph_gtk_view
  GSList* list;
  for(list = view->nodes; list != NULL; list = list->next)
    {
      GraphGtkNode* node = (GraphGtkNode*)list->data;
      graph_gtk_node_render(node, cr);
    }

  return FALSE;
}

GtkWidget*
graph_gtk_view_new()
{
  return g_object_new(GRAPH_TYPE_GTK_VIEW, NULL);
}

void
graph_gtk_view_add_node(GraphGtkView* self, GraphGtkNode* node)
{
  if(!g_slist_find(self->nodes, node))
    {
      g_object_ref_sink(G_OBJECT(node)); //is sink the right thing to do here?
      self->nodes = g_slist_append(self->nodes, node);
    }
}

void
graph_gtk_view_remove_node(GraphGtkView* self, GraphGtkNode* node)
{
  if(g_slist_find(self->nodes, node))
    {
      self->nodes = g_slist_remove(self->nodes, node);
      g_object_unref(G_OBJECT(node));
    }
}

void
graph_gtk_view_clear(GraphGtkView* self)
{
  GSList* list;
  for(list = self->nodes; list != NULL; list = list->next)
    {
      g_object_unref(G_OBJECT(list->data));
    }

  g_slist_free(self->nodes);
  self->nodes = NULL;
}

GSList*
graph_gtk_view_get_nodes(GraphGtkView* self)
{
  return g_slist_copy(self->nodes); 
}
