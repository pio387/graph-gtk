#ifndef __GRAPH_GTK_PAD_H__
#define __GRAPH_GTK_PAD_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GRAPH_TYPE_GTK_PAD                                              \
   (graph_gtk_pad_get_type())
#define GRAPH_GTK_PAD(obj)                                              \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                GRAPH_TYPE_GTK_PAD,                     \
                                GraphGtkPad))
#define GRAPH_GTK_PAD_CLASS(klass)                                      \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             GRAPH_TYPE_GTK_PAD,                        \
                             GraphGtkPadClass))
#define IS_GRAPH_GTK_PAD(obj)                                           \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                GRAPH_TYPE_GTK_PAD))
#define IS_GRAPH_GTK_PAD_CLASS(klass)                                   \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             GRAPH_TYPE_GTK_PAD))
#define GRAPH_GTK_PAD_GET_CLASS(obj)                                    \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               GRAPH_TYPE_GTK_PAD,                      \
                               GraphGtkPadClass))

struct _GraphGtkNode;
typedef struct _GraphGtkNode GraphGtkNode;

typedef struct _GraphGtkPad      GraphGtkPad;
typedef struct _GraphGtkPadClass GraphGtkPadClass;

struct _GraphGtkPadClass
{
  GObjectClass parent_class;
  void (*render_pad) (GraphGtkPad* self, cairo_t* t);
};

struct _GraphGtkPad
{
  GObject parent;

  const gchar* name;

  GraphGtkNode* node; //node to which this pad is attached

  gint rel_x, rel_y; //coordinates of the center this pad relative to the containing node's coordinates. These values are set by the containing node when the node is constructed and if/when the node is resized. It is up to the containing node to made sure these coordinates are up to date
  gboolean is_output; //input pads can only have one connection. New connections replace old connections
  GSList* connections; //each GraphGtkConnection* is referenced twice, once each by the GraphGtkPads at either end
};

GType graph_gtk_pad_get_type (void) G_GNUC_CONST;

GraphGtkPad*		graph_gtk_pad_new(const gchar* name, gboolean is_output);
void		graph_gtk_pad_get_position(GraphGtkPad* self, int *x, int *y);
void		graph_gtk_pad_render(GraphGtkPad* self, cairo_t* cairo);
gboolean	graph_gtk_pad_is_connected_to(GraphGtkPad* self, GraphGtkPad* other);
void		graph_gtk_pad_connect_to(GraphGtkPad* source, GraphGtkPad* sink);
void		graph_gtk_pad_disconnect(GraphGtkPad* self);

G_END_DECLS

#endif /* __GRAPH_GTK_PAD_H__ */
