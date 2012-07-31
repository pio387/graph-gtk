#include "graph-gtk-view.h"
#include "graph-gtk-node.h"
#include "graph-gtk-pad.h"
#include "graph-gtk-connection.h"

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

  //Signals
  /* Nodes connected:
   * void nodes_connected(GraphGtkNode *from, const gchar* output, GraphGtkNode *to, const gchar* input);
   */
  g_signal_new("nodes-connected",
	       GRAPH_TYPE_GTK_VIEW,
	       G_SIGNAL_RUN_FIRST,
	       0, //no class method
	       NULL, //no accumulator,
	       NULL,
	       NULL,
	       G_TYPE_NONE,
	       4,
	       GRAPH_TYPE_GTK_NODE,
	       G_TYPE_STRING,
	       GRAPH_TYPE_GTK_NODE,
	       G_TYPE_STRING);

  g_signal_new("nodes-disconnected",
	       GRAPH_TYPE_GTK_VIEW,
	       G_SIGNAL_RUN_FIRST,
	       0, //no class method
	       NULL, //no accumulator,
	       NULL,
	       NULL,
	       G_TYPE_NONE,
	       4,
	       GRAPH_TYPE_GTK_NODE,
	       G_TYPE_STRING,
	       GRAPH_TYPE_GTK_NODE,
	       G_TYPE_STRING);

  g_signal_new("node-selected",
	       GRAPH_TYPE_GTK_VIEW,
	       G_SIGNAL_RUN_FIRST,
	       0, //no class method
	       NULL, //no accumulator,
	       NULL,
	       NULL,
	       G_TYPE_NONE,
	       1,
	       GRAPH_TYPE_GTK_NODE);

  g_signal_new("node-deselected",
	       GRAPH_TYPE_GTK_VIEW,
	       G_SIGNAL_RUN_FIRST,
	       0, //no class method
	       NULL, //no accumulator,
	       NULL,
	       NULL,
	       G_TYPE_NONE,
	       1,
	       GRAPH_TYPE_GTK_NODE);

  g_signal_new("node-doubleclicked",
	       GRAPH_TYPE_GTK_VIEW,
	       G_SIGNAL_RUN_FIRST,
	       0, //no class method
	       NULL, //no accumulator,
	       NULL,
	       NULL,
	       G_TYPE_NONE,
	       1,
	       GRAPH_TYPE_GTK_NODE);
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
  self->pan_x = 0;
  self->pan_y = 0;
  self->bg = NULL;
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

  cairo_set_source_rgb(cr, 124.0/256.0, 124.0/256.0, 124.0/256.0);
  cairo_paint(cr);

  if(view->bg)
    {
      gdouble bg_w = cairo_image_surface_get_width(view->bg);
      gdouble bg_h = cairo_image_surface_get_height(view->bg);


      gint width = gdk_window_get_width(widget->window);
      gint height = gdk_window_get_height(widget->window);

      cairo_pattern_t *pattern = cairo_pattern_create_for_surface(view->bg);
      cairo_matrix_t transform;
      cairo_matrix_init_translate(&transform, -(width/2-bg_w/2), -(height/2-bg_h/2));
      cairo_pattern_set_matrix(pattern, &transform);

      cairo_set_source(cr, pattern);
      cairo_rectangle(cr, width/2-bg_w/2, height/2-bg_h/2, bg_w, bg_h);
      cairo_fill(cr);

      cairo_pattern_destroy(pattern);
    }

  cairo_translate(cr, -view->pan_x, -view->pan_y);
  if(view->is_mouse_panning)
    {
      cairo_translate(cr, -(view->pan_begin_x-view->mouse_x), -(view->pan_begin_y-view->mouse_y));
    }

  //render the graph_gtk_view
  GList* nodes;
  for(nodes = view->nodes; nodes != NULL; nodes = nodes->next)
    {
      GraphGtkNode *node = (GraphGtkNode*)nodes->data;

      GList *pads;
      for(pads = graph_gtk_node_get_input_pads(node); pads != NULL; pads = pads->next)
	{
	  GraphGtkPad *pad = (GraphGtkPad*)pads->data;
	  GList *connections;
	  for(connections = pad->connections; connections != NULL; connections = connections->next)
	    {
	      GraphGtkConnection *connection = (GraphGtkConnection*)connections->data;
	      graph_gtk_connection_render(connection, cr);
	    }
	}

      for(pads = graph_gtk_node_get_output_pads(node); pads != NULL; pads = pads->next)
	{
	  GraphGtkPad *pad = (GraphGtkPad*)pads->data;
	  GList *connections;
	  for(connections = pad->connections; connections != NULL; connections = connections->next)
	    {
	      GraphGtkConnection *connection = (GraphGtkConnection*)connections->data;
	      graph_gtk_connection_render(connection, cr);
	    }
	}
    }

  GList* list;
  for(list = view->nodes; list != NULL; list = list->next)
    {
      GraphGtkNode* node = (GraphGtkNode*)list->data;
      graph_gtk_node_render(node, cr);
    }

  if(view->is_mouse_connecting)
    {
      int x, y;
      graph_gtk_pad_get_position(view->pad_connecting_from, &x, &y);

      cairo_move_to(cr, x, y);
      cairo_line_to(cr, view->mouse_x, view->mouse_y);

      cairo_set_source_rgb(cr, 0.0, 1, 0.0);
      cairo_set_line_width(cr, 0.5);
      cairo_stroke(cr);
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
      GList *deselect = g_list_copy(self->selected_nodes);
      GList* nodes;
      for(nodes = self->selected_nodes; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = nodes->data;
	  //Todo: don't emit signal if if is just going to be selected again
	  node->is_selected = FALSE;
	}

      g_list_free(self->selected_nodes);
      self->selected_nodes = NULL;

      GList *select = NULL;
      for(nodes = self->nodes; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = (GraphGtkNode*)nodes->data;
	  GraphGtkPad *pad;

	  if(pad = graph_gtk_node_is_on_pad(node, event->x, event->y))
	    {
	      self->is_mouse_connecting = TRUE;
	      self->pad_connecting_from = pad;
	      if(!pad->is_output)
		graph_gtk_pad_disconnect(pad);
	    }
	  else if(graph_gtk_node_is_within(node, event->x, event->y))
	    {
	      node->is_selected = TRUE;
	      deselect = g_list_remove(deselect, node);
	      select = g_list_append(select, node);
	      self->selected_nodes = g_list_append(self->selected_nodes, node);

	      self->is_mouse_dragging = TRUE;
	      self->drag_begin_x = event->x;
	      self->drag_begin_y = event->y;

	      break;
	    }
	}

      //This is a pretty slow way to do it, though it shouldn't matter given how few nodes most graphs will have
      for(nodes = deselect; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = nodes->data;
	  g_signal_emit_by_name(widget, "node-deselected", GRAPH_GTK_NODE(node));
	}

      for(nodes = select; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = nodes->data;
	  g_signal_emit_by_name(widget, "node-selected", GRAPH_GTK_NODE(node));


	  if(!nodes->next && event->type == GDK_2BUTTON_PRESS)
	    {
	      g_signal_emit_by_name(widget, "node-doubleclicked", GRAPH_GTK_NODE(node));
	    }
	}
      
      g_list_free(deselect);
      g_list_free(select);
    }
  else if(event->button == 3)
    {
      self->is_mouse_panning = TRUE;
      self->pan_begin_x = event->x;
      self->pan_begin_y = event->y;
    }

  return FALSE;
}

static gboolean
graph_gtk_view_button_released(GtkWidget* widget, GdkEventButton* event)
{
  GraphGtkView *self = GRAPH_GTK_VIEW(widget);

  if(event->button == 1)
    {
      if(self->is_mouse_dragging)
	{
	  REDRAW();

	  self->is_mouse_dragging = FALSE;

	  GList* nodes;
	  for(nodes = self->selected_nodes; nodes != NULL; nodes = nodes->next)
	    {
	      GraphGtkNode *node = nodes->data;
	      if(node->is_selected)
		{
		  node->x += event->x-self->drag_begin_x;
		  node->y += event->y-self->drag_begin_y;
		  node->offset_x = 0;
		  node->offset_y = 0;
		}
	    }
	}
      else if(self->is_mouse_connecting)
	{
	  REDRAW();

	  self->is_mouse_connecting = FALSE;

	  GList *nodes;
	  for(nodes = self->nodes; nodes != NULL; nodes = nodes->next)
	    {
	      GraphGtkNode *node = (GraphGtkNode*)nodes->data;
	      GraphGtkPad *pad;

	      if(pad = graph_gtk_node_is_on_pad(node, event->x, event->y))
		{
		  REDRAW();
		  self->is_mouse_connecting = FALSE;
		  if(self->pad_connecting_from->is_output)
		    {
		      graph_gtk_pad_connect_to(self->pad_connecting_from, pad);
		      g_signal_emit_by_name(self, "nodes-connected", 
					    self->pad_connecting_from->node, self->pad_connecting_from->name,
					    pad->node, pad->name);
		    }
		  else
		    {
		      graph_gtk_pad_connect_to(pad, self->pad_connecting_from);
		      g_signal_emit_by_name(self, "nodes-connected", 
					    pad->node, pad->name,
					    self->pad_connecting_from->node, self->pad_connecting_from->name);
		    }
		}
	    }
	}
    }
  else if(event->button == 3)
    {
      if(self->is_mouse_panning)
	{
	  self->is_mouse_panning = FALSE;
	  self->pan_x += self->pan_begin_x-self->mouse_x;
	  self->pan_y += self->pan_begin_y-self->mouse_y;
	}
    }

  return FALSE;
}

static gboolean
graph_gtk_view_mouse_moved(GtkWidget* widget, GdkEventMotion* event)
{
  GraphGtkView *self = GRAPH_GTK_VIEW(widget);
 
  self->mouse_x = event->x;
  self->mouse_y = event->y;

  if(self->is_mouse_connecting || self->is_mouse_panning)
    {
      REDRAW();
    }

  if(self->is_mouse_dragging)
    {
      GList* nodes;
      for(nodes = self->selected_nodes; nodes != NULL; nodes = nodes->next)
	{
	  GraphGtkNode *node = nodes->data;
	  node->offset_x = event->x-self->drag_begin_x;
	  node->offset_y = event->y-self->drag_begin_y;
	}

      REDRAW();
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
  if(!g_list_find(self->nodes, node))
    {
      g_object_ref_sink(G_OBJECT(node)); //is sink the right thing to do here?
      self->nodes = g_list_append(self->nodes, node);
      node->view = self;

      REDRAW();
    }
}

void
graph_gtk_view_remove_selected_nodes(GraphGtkView* self)
{
  while(self->selected_nodes)
    {
       graph_gtk_view_remove_node(self, (GraphGtkNode*)self->selected_nodes->data);
       self->selected_nodes = g_list_remove(self->selected_nodes, (GraphGtkNode*)self->selected_nodes->data);
    }
}

void
graph_gtk_view_remove_node(GraphGtkView* self, GraphGtkNode* node)
{
  if(g_list_find(self->nodes, node))
    {
      self->nodes = g_list_remove(self->nodes, node);

      GList *pad;
      for(pad = graph_gtk_node_get_input_pads(node); pad != NULL; pad = pad->next)
	{
	  graph_gtk_pad_disconnect((GraphGtkPad*)(pad->data));
	}

      for(pad = graph_gtk_node_get_output_pads(node); pad != NULL; pad = pad->next)
	{
	  graph_gtk_pad_disconnect((GraphGtkPad*)(pad->data));
	}
      
      g_object_unref(G_OBJECT(node));

      REDRAW();
    }
}

void
graph_gtk_view_clear(GraphGtkView* self)
{
  GList* list;
  for(list = self->nodes; list != NULL; list = list->next)
    {
      g_object_unref(G_OBJECT(list->data));
    }

  g_list_free(self->nodes);
  self->nodes = NULL;

  REDRAW();
}

GList*
graph_gtk_view_get_nodes(GraphGtkView* self)
{
  return g_list_copy(self->nodes); 
}

void
graph_gtk_view_set_bg(GraphGtkView* self, cairo_surface_t* bg)
{
  self->bg = bg;
  REDRAW();
}
