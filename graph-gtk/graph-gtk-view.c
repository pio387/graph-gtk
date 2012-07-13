#include "graph-gtk-view.h"


static void graph_gtk_view_dispose (GObject *object);
static void graph_gtk_view_finalize (GObject *object);

G_DEFINE_TYPE (GraphGtkView, graph_gtk_view, GTK_TYPE_DRAWING_AREA);

static void
graph_gtk_view_class_init (GraphGtkViewClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *)klass;

    gobject_class->dispose = graph_gtk_view_dispose;
    gobject_class->finalize = graph_gtk_view_finalize;
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

