#include "graph-gtk-node.h"
#include "graph-gtk-pad.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

void cairo_image_surface_blur( cairo_surface_t* surface, double radius )
{
  // Steve Hanov, 2009
  // Released into the public domain.
    
  // get width, height
  int width = cairo_image_surface_get_width( surface );
  int height = cairo_image_surface_get_height( surface );
  unsigned char* dst = (unsigned char*)malloc(width*height*4);
  unsigned* precalc = 
    (unsigned*)malloc(width*height*sizeof(unsigned));
  unsigned char* src = cairo_image_surface_get_data( surface );
  double mul=1.f/((radius*2)*(radius*2));
  int channel;
    
  // The number of times to perform the averaging. According to wikipedia,
  // three iterations is good enough to pass for a gaussian.
  const MAX_ITERATIONS = 3; 
  int iteration;

  memcpy( dst, src, width*height*4 );

  for ( iteration = 0; iteration < MAX_ITERATIONS; iteration++ ) {
    for( channel = 0; channel < 4; channel++ ) {
      int x,y;

      // precomputation step.
      unsigned char* pix = src;
      unsigned* pre = precalc;

      pix += channel;
      for (y=0;y<height;y++) {
	for (x=0;x<width;x++) {
	  int tot=pix[0];
	  if (x>0) tot+=pre[-1];
	  if (y>0) tot+=pre[-width];
	  if (x>0 && y>0) tot-=pre[-width-1];
	  *pre++=tot;
	  pix += 4;
	}
      }

      // blur step.
      pix = dst + (int)radius * width * 4 + (int)radius * 4 + channel;
      for (y=radius;y<height-radius;y++) {
	for (x=radius;x<width-radius;x++) {
	  int l = x < radius ? 0 : x - radius;
	  int t = y < radius ? 0 : y - radius;
	  int r = x + radius >= width ? width - 1 : x + radius;
	  int b = y + radius >= height ? height - 1 : y + radius;
	  int tot = precalc[r+b*width] + precalc[l+t*width] - 
	    precalc[l+b*width] - precalc[r+t*width];
	  *pix=(unsigned char)(tot*mul);
	  pix += 4;
	}
	pix += (int)radius * 2 * 4;
      }
    }
    memcpy( src, dst, width*height*4 );
  }

  free( dst );
  free( precalc );
}

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
  self->x = 10;
  self->y = 10;
  self->width = 118;
  self->height = 80;
  self->view = NULL;
  self->input_pads = NULL;
  self->output_pads = NULL;
  self->image = NULL;
  self->img_width = -1;
  self->img_height = -1;
  self->show_image = FALSE;
  self->failed_size_calculation = TRUE;
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
graph_gtk_node_render_default(GraphGtkNode* self, cairo_t* cr)
{
  double M_PI = 3.14159;
  //Draw the node with cairo
  int corner_radius = 5;

  if(!self->is_selected)
    {
      //shadow
      cairo_surface_t *shadow = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, self->width+50, self->height+50);
      cairo_t *shadow_context = cairo_create(shadow);

      cairo_save (shadow_context);
      cairo_set_source_rgba (shadow_context, 0, 0, 0, 0);
      cairo_set_operator (shadow_context, CAIRO_OPERATOR_SOURCE);
      cairo_paint (shadow_context);
      cairo_restore (shadow_context);

      cairo_line_to(shadow_context, 25+self->width-corner_radius, 25);
      cairo_arc(shadow_context, 25+self->width-corner_radius, 25+corner_radius, corner_radius, -M_PI/2.0, 0.0);
      cairo_line_to(shadow_context, 25+self->width, 25+self->height-corner_radius);
      cairo_arc(shadow_context, 25+self->width-corner_radius, 25+self->height-corner_radius, corner_radius, 0.0, M_PI/2.0);
      cairo_line_to(shadow_context, 25+corner_radius, 25+self->height);
      cairo_arc(shadow_context, 25+corner_radius, 25+self->height-corner_radius, corner_radius, M_PI/2.0, M_PI);
      cairo_line_to(shadow_context, 25, 25+corner_radius);
      cairo_arc(shadow_context, 25+corner_radius, 25+corner_radius, corner_radius, M_PI, -M_PI/2.0);
      cairo_close_path(shadow_context); //probably unecessary
      cairo_set_source_rgba(shadow_context, 0, 0, 0, 0.7);
      cairo_fill(shadow_context);

      cairo_surface_flush(shadow);
      cairo_image_surface_blur(shadow, 2);

      int offset = 3.5;
      cairo_set_source_surface(cr, shadow, (self->x+self->offset_x)-25+offset, (self->y+self->offset_y)-25+offset);
      //cairo_rectangle(cr, (self->x+self->offset_x)-10, (self->y+self->offset_y)-10, (self->x+self->offset_x)+20, (self->y+self->offset_y)+20);
      cairo_paint(cr);

      cairo_surface_finish(shadow);
      cairo_destroy(shadow_context);
    }

  cairo_move_to(cr, (self->x+self->offset_x)+corner_radius, (self->y+self->offset_y));
  cairo_line_to(cr, (self->x+self->offset_x)+self->width-corner_radius, (self->y+self->offset_y));
  cairo_arc(cr, (self->x+self->offset_x)+self->width-corner_radius, (self->y+self->offset_y)+corner_radius,
	    corner_radius,
	    -M_PI/2.0, 0.0);
  cairo_line_to(cr, (self->x+self->offset_x)+self->width, (self->y+self->offset_y)+15);
  cairo_line_to(cr, (self->x+self->offset_x), (self->y+self->offset_y)+15);
  cairo_line_to(cr, (self->x+self->offset_x), (self->y+self->offset_y)+corner_radius);
  cairo_arc(cr, (self->x+self->offset_x)+corner_radius, (self->y+self->offset_y)+corner_radius, 
	    corner_radius,
	    M_PI, -M_PI/2.0);
  cairo_set_source_rgb(cr, 80.0/256.0, 80.0/256.0, 80.0/256.0);
  cairo_fill(cr);

  cairo_move_to(cr, (self->x+self->offset_x)+self->width, (self->y+self->offset_y)+15);
  cairo_line_to(cr, (self->x+self->offset_x)+self->width, (self->y+self->offset_y)+self->height-corner_radius);
  cairo_arc(cr, (self->x+self->offset_x)+self->width-corner_radius, (self->y+self->offset_y)+self->height-corner_radius, corner_radius, 0.0, M_PI/2.0);
  cairo_line_to(cr, (self->x+self->offset_x)+corner_radius, (self->y+self->offset_y)+self->height);
  cairo_arc(cr, (self->x+self->offset_x)+corner_radius, (self->y+self->offset_y)+self->height-corner_radius,
	    corner_radius,
	    M_PI/2.0, M_PI);
  cairo_line_to(cr, (self->x+self->offset_x), (self->y+self->offset_y)+15);
  cairo_close_path(cr);
  cairo_set_source_rgb(cr, 44.0/256.0, 44.0/256.0, 44.0/256.0);
  cairo_fill(cr);

  cairo_move_to(cr, (self->x+self->offset_x)+corner_radius, (self->y+self->offset_y));

  //top
  cairo_line_to(cr, (self->x+self->offset_x)+self->width-corner_radius, (self->y+self->offset_y));

  //top-right
  cairo_arc(cr, (self->x+self->offset_x)+self->width-corner_radius, (self->y+self->offset_y)+corner_radius,
	    corner_radius,
	    -M_PI/2.0, 0.0);

  //right
  cairo_line_to(cr, (self->x+self->offset_x)+self->width, (self->y+self->offset_y)+self->height-corner_radius);

  //right-bottom
  cairo_arc(cr, (self->x+self->offset_x)+self->width-corner_radius, (self->y+self->offset_y)+self->height-corner_radius, corner_radius, 0.0, M_PI/2.0);

  //bottom
  cairo_line_to(cr, (self->x+self->offset_x)+corner_radius, (self->y+self->offset_y)+self->height);

  //bottom-left
  cairo_arc(cr, (self->x+self->offset_x)+corner_radius, (self->y+self->offset_y)+self->height-corner_radius,
	    corner_radius,
	    M_PI/2.0, M_PI);

  //left
  cairo_line_to(cr, (self->x+self->offset_x), (self->y+self->offset_y)+corner_radius);
  //top-left
  cairo_arc(cr, (self->x+self->offset_x)+corner_radius, (self->y+self->offset_y)+corner_radius, 
	    corner_radius,
	    M_PI, -M_PI/2.0);
  cairo_close_path(cr); //probably unecessary

  cairo_pattern_t *gradient = cairo_pattern_create_linear(0, -15, 0, 25);

  if(!self->is_selected)
    {
      cairo_pattern_add_color_stop_rgb(gradient, 0, 210.0/256.0, 210.0/256.0, 210.0/256.0);
      cairo_pattern_add_color_stop_rgb(gradient, 1.0, 132.0/256.0, 132.0/256.0, 132.0/256.0);
    }
  else
    {
      cairo_pattern_add_color_stop_rgb(gradient, 0, 165.0/256.0, 255.0/256.0, 184.0/256.0);
      cairo_pattern_add_color_stop_rgb(gradient, 1.0, 9.0/256.0, 255.0/256.0, 48.0/256.0);
    }

  cairo_set_source(cr, gradient);
  cairo_set_line_width(cr, 0.5);
  cairo_stroke(cr);

  cairo_select_font_face (cr, "FreeSerif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 14);
  cairo_set_source_rgb(cr, 200.0/256.0, 200.0/256.0, 200.0/256.0);
  cairo_move_to(cr, (self->x+self->offset_x)+4, (self->y+self->offset_y)+13);
  cairo_show_text(cr, self->name);

  GList *pad;
  for(pad = graph_gtk_node_get_input_pads(self); pad != NULL; pad = pad->next)
    {
      graph_gtk_pad_render((GraphGtkPad*)pad->data, cr);
    }

  for(pad = graph_gtk_node_get_output_pads(self); pad != NULL; pad = pad->next)
    {
      graph_gtk_pad_render((GraphGtkPad*)pad->data, cr);
    }

  if(self->show_image && self->image)
    {
      //cairo_set_source_surface(cr, self->image, 0, 0);
      gdouble surface_w = cairo_image_surface_get_width(self->image);
      gdouble surface_h = cairo_image_surface_get_height(self->image);
      if(surface_w > 0 && surface_h > 0)
	{
	  gdouble image_w = self->img_width, image_h = self->img_height;
	  if(self->img_width == -1 && self->img_height != -1)
	    {
	      image_h = self->img_height;
	      image_w = (image_h/surface_h)*surface_w;
	    }
	  else if(self->img_width != -1 && self->img_height == -1)
	    {
	      image_w = self->img_width;
	      image_h = (image_w/surface_w)*surface_h;
	    }
	  else if(self->img_width == -1 && self->img_height == -1)
	    {
	      image_w = surface_w;
	      image_h = surface_h;
	    }

	  cairo_pattern_t *pattern = cairo_pattern_create_for_surface(self->image);

	  gdouble x = self->x+self->offset_x+4;
	  gdouble y = self->y+self->offset_y+20;

	  cairo_matrix_t transform;
	  cairo_matrix_init_scale(&transform, surface_w/image_w, surface_h/image_h);
	  cairo_matrix_translate(&transform, -x, -y);
	  cairo_pattern_set_matrix(pattern, &transform);

	  cairo_set_source(cr, pattern);
	  cairo_rectangle(cr, x, y, image_w, image_h);
	  cairo_fill(cr);

	  cairo_pattern_destroy(pattern);
	}
    }
}

GraphGtkNode*
graph_gtk_node_new()
{
  return GRAPH_GTK_NODE(g_object_new(GRAPH_TYPE_GTK_NODE, NULL));
}

void
graph_gtk_node_render(GraphGtkNode* self, cairo_t* cairo)
{
  g_return_if_fail(IS_GRAPH_GTK_NODE(self));

  if(self->failed_size_calculation)
    self->failed_size_calculation = !graph_gtk_node_recalculate_size(self);

  GRAPH_GTK_NODE_GET_CLASS(self)->render_node(self, cairo);
}

GList*
graph_gtk_node_get_pads(GraphGtkNode* self)
{
  return g_list_concat(g_list_copy(self->input_pads), g_list_copy(self->output_pads));
}

GList* 
graph_gtk_node_get_input_pads(GraphGtkNode* self)
{
  return g_list_copy(self->input_pads);
}

GList* 
graph_gtk_node_get_output_pads(GraphGtkNode* self)
{
  return g_list_copy(self->output_pads);
}

void
graph_gtk_node_connect_to(GraphGtkNode* source, const gchar* output_pad, GraphGtkNode* sink, const gchar* input_pad)
{
  GraphGtkPad *source_pad = NULL, *sink_pad = NULL;

  GList* list;
  for(list = graph_gtk_node_get_pads(source); list != NULL; list = list->next)
    {
      GraphGtkPad *pad = (GraphGtkPad*)list->data;
      if(0 == g_strcmp0(pad->name, output_pad))
	{
	  source_pad = pad;
	  break;
	}
    }

  for(list = graph_gtk_node_get_pads(sink); list != NULL; list = list->next)
    {
      GraphGtkPad *pad = (GraphGtkPad*)list->data;
      if(0 == g_strcmp0(pad->name, input_pad))
	{
	  sink_pad = pad;
	  break;
	}
    }

  if(!source_pad || !sink_pad)
    return;

  graph_gtk_pad_connect_to(source_pad, sink_pad);
}

gboolean
graph_gtk_node_recalculate_size(GraphGtkNode* self)
{
  GraphGtkView *view = self->view;
  if(!view)
    return FALSE;

  gtk_widget_queue_draw(GTK_WIDGET(view));

  self->height = 30;

  //Calculate width
  gdouble longest_in = 0.0, longest_out = 0.0;

  GList* list;
  for(list = self->output_pads; list != NULL; list = list->next) {
    GraphGtkPad *pad = (GraphGtkPad*)list->data;
    longest_in = MAX(longest_in, graph_gtk_pad_get_width(pad));
  }
  for(list = self->input_pads; list != NULL; list = list->next) {
    GraphGtkPad *pad = (GraphGtkPad*)list->data;
    longest_out = MAX(longest_out, graph_gtk_pad_get_width(pad));
  }

  cairo_text_extents_t extents;
  GtkWidget *widget = GTK_WIDGET(view);
  cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));

  cairo_select_font_face (cr, "FreeSerif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 13);
      
  cairo_text_extents(cr, self->name, &extents);

  cairo_destroy(cr);

  self->width = MAX(extents.width+20, longest_in+longest_out+45);

  gdouble image_w = 0, image_h = 0;

  if(self->show_image && self->image)
    {
      gdouble surface_w = cairo_image_surface_get_width(self->image);
      gdouble surface_h = cairo_image_surface_get_height(self->image);

      if(surface_w > 0 && surface_h > 0)
	{
	  image_w = self->img_width;
	  image_h = self->img_height;

	  if(self->img_width == -1 && self->img_height != -1)
	    {
	      image_h = self->img_height;
	      image_w = image_h/surface_h*surface_w;
	    }
	  else if(self->img_width != -1 && self->img_height == -1)
	    {
	      image_w = self->img_width;
	      image_h = image_w/surface_w*surface_h;
	    }
	  else if(self->img_width == -1 && self->img_height == -1)
	    {
	      image_w = surface_w;
	      image_h = surface_h;
	    }

	  self->width = MAX(self->width, image_w+8);
	}
    }
  
  //Set pad positions and calculate height
  int count;
  int m_count;
  for(list = self->output_pads, count = 0; list != NULL; list = list->next, count++) {
    GraphGtkPad *pad = (GraphGtkPad*)list->data;

    pad->rel_x = self->width-4.5;
    pad->rel_y = 30+count*25;
    if(self->show_image && image_h > 0)
      pad->rel_y += image_h+6;
  }

  int mcount = count;

  for(list = self->input_pads, count = 0; list != NULL; list = list->next, count++) {
    GraphGtkPad *pad = (GraphGtkPad*)list->data;

    pad->rel_x = 5.5;
    pad->rel_y = 30+count*25;
    if(self->show_image && image_h > 0)
      pad->rel_y += image_h+6;
  }

  mcount = MAX(count, mcount);
  mcount--;

  int imgh = (self->show_image ? image_h : 0) ;

  if(50+mcount*25 + imgh > self->height)
    self->height = 50+mcount*25 + imgh;

  return TRUE;
}

void
graph_gtk_node_set_name(GraphGtkNode* self, const gchar* name)
{
  self->name = g_strdup(name);
}

const gchar*
graph_gtk_node_get_name(GraphGtkNode* self)
{
  return self->name;
}

void
graph_gtk_node_add_pad(GraphGtkNode* self, const gchar* pad_name, gboolean output)
{
  if(output)
    {
      GList* list;
      for(list = self->output_pads; list != NULL && g_strcmp0(((GraphGtkPad*)list->data)->name, pad_name); list = list->next);
      if(!list) {
	GraphGtkPad* pad = graph_gtk_pad_new(pad_name, TRUE);
	pad->node = self;
	self->output_pads = g_list_append(self->output_pads, pad);
      }
    }
  else
    {
      GList* list;
      for(list = self->input_pads; list != NULL && g_strcmp0(((GraphGtkPad*)list->data)->name, pad_name); list = list->next);
      if(!list) {
	GraphGtkPad* pad = graph_gtk_pad_new(pad_name, FALSE);
	pad->node = self;
	self->input_pads = g_list_append(self->input_pads, pad);
      }
    }

  graph_gtk_node_recalculate_size(self);
}

gboolean
graph_gtk_node_is_within(GraphGtkNode* self, int x, int y)
{
  if(x > self->x && x < self->x+self->width &&
     y > self->y && y < self->y+self->height)
    {
      return TRUE;
    }

  return FALSE;
}

GraphGtkPad*
graph_gtk_node_is_on_pad(GraphGtkNode* self, int x, int y)
{
  GList *list;
  int px, py;

  for(list = graph_gtk_node_get_input_pads(self); list != NULL; list = list->next)
    {
      GraphGtkPad *pad = (GraphGtkPad*)list->data;
      graph_gtk_pad_get_position(pad, &px, &py);
      if(x > px-5 && x < px+5 && y > py-5 && y < py+5)
	return pad;
    }

  for(list = graph_gtk_node_get_output_pads(self); list != NULL; list = list->next)
    {
      GraphGtkPad *pad = (GraphGtkPad*)list->data;
      graph_gtk_pad_get_position(pad, &px, &py);
      if(x > px-5 && x < px+5 && y > py-5 && y < py+5)
	return pad;
    }

  return NULL;
}

void
graph_gtk_node_set_image(GraphGtkNode* self, cairo_surface_t* image)
{
  self->image = image;
  graph_gtk_node_recalculate_size(self);
}

void
graph_gtk_node_set_image_size(GraphGtkNode* self, gint width, gint height)
{
  self->img_width = width;
  self->img_height = height;
  graph_gtk_node_recalculate_size(self);
}

void
graph_gtk_node_show_image(GraphGtkNode* self, gboolean show_image)
{
  self->show_image = show_image;
  graph_gtk_node_recalculate_size(self);
}
