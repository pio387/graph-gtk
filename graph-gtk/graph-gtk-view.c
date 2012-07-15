#include "graph-gtk-view.h"
#include "graph-gtk-node.h"
#include "graph-gtk-pad.h"

#define REDRAW() gtk_widget_queue_draw(GTK_WIDGET(self))

static void graph_gtk_view_dispose (GObject *object);
static void graph_gtk_view_finalize (GObject *object);

static gboolean graph_gtk_view_draw(GtkWidget* self, cairo_t* cairo);

#if GTK_MAJOR_VERSION == (2)
static gboolean graph_gtk_view_expose(GtkWidget* self, GdkEventExpose* event);
#endif

static gboolean graph_gtk_view_button_pressed(GtkWidget* self, GdkEventButton* event);
static gboolean graph_gtk_view_button_released(GtkWidget* widget, GdkEventButton* event);
static gboolean graph_gtk_view_mouse_moved(GtkWidget* widget, GdkEventMotion* event);

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

  widget_class->button_press_event = graph_gtk_view_button_pressed;
  widget_class->button_release_event = graph_gtk_view_button_released;
  widget_class->motion_notify_event = graph_gtk_view_mouse_moved;
}

static void
graph_gtk_view_init (GraphGtkView *self)
{
  gtk_widget_add_events (GTK_WIDGET(self),
			 GDK_POINTER_MOTION_MASK |
			 GDK_BUTTON_PRESS_MASK   |
			 GDK_BUTTON_RELEASE_MASK |
			 GDK_KEY_PRESS_MASK |
			 GDK_KEY_RELEASE_MASK);

  gtk_widget_set_can_focus(GTK_WIDGET(self), TRUE);
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

  cairo_set_source_rgb(cr, 124.0/256.0, 124.0/256.0, 124.0/256.0);
  cairo_paint(cr);

  //render the graph_gtk_view
  GSList* list;
  for(list = view->nodes; list != NULL; list = list->next)
    {
      GraphGtkNode* node = (GraphGtkNode*)list->data;
      graph_gtk_node_render(node, cr, 0, 0);
    }

  return FALSE;
}

static gboolean
graph_gtk_view_button_pressed(GtkWidget* widget, GdkEventButton* event)
{
  GraphGtkView *self = GRAPH_GTK_VIEW(widget);

  if(event->button == 1)
    {
      REDRAW();

      //TODO: shift click to select multiple nodes
      GSList* nodes;
      for(nodes = self->selected_nodes; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = nodes->data;
	  node->is_selected = FALSE;
	}

      g_slist_free(self->selected_nodes);
      self->selected_nodes = NULL;

      for(nodes = self->nodes; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = (GraphGtkNode*)nodes->data;
	  GraphGtkPad *pad;

	  if(pad = graph_gtk_node_is_on_pad(node, event->x, event->y))
	    {
	      g_print("Connecting from pad: %s\n", pad->name);
	      self->is_mouse_connecting = TRUE;
	      self->pad_connecting_from = pad;
	    }
	  else if(graph_gtk_node_is_within(node, event->x, event->y))
	    {
	      node->is_selected = TRUE;
	      self->selected_nodes = g_slist_append(self->selected_nodes, node);

	      self->is_mouse_dragging = TRUE;
	      self->drag_begin_x = event->x;
	      self->drag_begin_y = event->y;

	      break;
	    }
	}
    }

  return FALSE;
}

static gboolean
graph_gtk_view_button_released(GtkWidget* widget, GdkEventButton* event)
{
  GraphGtkView *self = GRAPH_GTK_VIEW(widget);

  if(self->is_mouse_dragging)
    {
      REDRAW();

      self->is_mouse_dragging = FALSE;

      GSList* nodes;
      for(nodes = self->selected_nodes; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = nodes->data;
	  if(node->is_selected)
	    {
	      node->x += event->x-self->drag_begin_x;
	      node->y += event->y-self->drag_begin_y;
	    }
	}
    }
  else if(self->is_mouse_connecting)
    {
      GSList *nodes;
      for(nodes = self->nodes; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = (GraphGtkNode*)nodes->data;
	  GraphGtkPad *pad;

	  if(pad = graph_gtk_node_is_on_pad(node, event->x, event->y))
	    {
	      REDRAW();
	      g_print("Connecting to pad: %s\n", pad->name);
	      self->is_mouse_connecting = FALSE;
	      if(self->pad_connecting_from->is_output)
		graph_gtk_pad_connect_to(self->pad_connecting_from, pad);
	      else
		graph_gtk_pad_connect_to(pad, self->pad_connecting_from);
	    }
	}
    }

  return FALSE;
}

static gboolean
graph_gtk_view_mouse_moved(GtkWidget* widget, GdkEventMotion* event)
{
  GraphGtkView *self = GRAPH_GTK_VIEW(widget);

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

      REDRAW();
    }
}

void
graph_gtk_view_remove_node(GraphGtkView* self, GraphGtkNode* node)
{
  if(g_slist_find(self->nodes, node))
    {
      self->nodes = g_slist_remove(self->nodes, node);
      g_object_unref(G_OBJECT(node));

      REDRAW();
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

  REDRAW();
}

GSList*
graph_gtk_view_get_nodes(GraphGtkView* self)
{
  return g_slist_copy(self->nodes); 
}
