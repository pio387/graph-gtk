#ifndef __GRAPH_GTK_CONNECTION_H__
#define __GRAPH_GTK_CONNECTION_H__

#include <glib-object.h>
#include "graph-gtk-pad.h"

G_BEGIN_DECLS

#define GRAPH_TYPE_GTK_CONNECTION                                       \
   (graph_gtk_connection_get_type())
#define GRAPH_GTK_CONNECTION(obj)                                       \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                GRAPH_TYPE_GTK_CONNECTION,              \
                                GraphGtkConnection))
#define GRAPH_GTK_CONNECTION_CLASS(klass)                               \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             GRAPH_TYPE_GTK_CONNECTION,                 \
                             GraphGtkConnectionClass))
#define IS_GRAPH_GTK_CONNECTION(obj)                                    \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                GRAPH_TYPE_GTK_CONNECTION))
#define IS_GRAPH_GTK_CONNECTION_CLASS(klass)                            \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             GRAPH_TYPE_GTK_CONNECTION))
#define GRAPH_GTK_CONNECTION_GET_CLASS(obj)                             \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               GRAPH_TYPE_GTK_CONNECTION,               \
                               GraphGtkConnectionClass))

struct _GraphGtkPad;
typedef struct _GraphGtkPad GraphGtkPad;

typedef struct _GraphGtkConnection      GraphGtkConnection;
typedef struct _GraphGtkConnectionClass GraphGtkConnectionClass;

struct _GraphGtkConnectionClass
{
  GObjectClass parent_class;
  void (*render_connection) (GraphGtkConnection* self, cairo_t* cairo);
};

struct _GraphGtkConnection
{
  GObject parent;

  GraphGtkPad *source, *sink;
};

GType graph_gtk_connection_get_type (void) G_GNUC_CONST;

GraphGtkConnection* graph_gtk_connection_new(GraphGtkPad* source, GraphGtkPad* sink);
void graph_gtk_connection_render(GraphGtkConnection* self, cairo_t* cairo);

G_END_DECLS

#endif /* __GRAPH_GTK_CONNECTION_H__ */
