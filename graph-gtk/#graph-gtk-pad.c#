#include "graph-gtk-pad.h"


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

