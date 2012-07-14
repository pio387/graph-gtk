#include <gtk/gtk.h>
#include <graph-gtk-view.h>

gint
main(gint argc, 
     gchar** argv)
{
  gtk_init(&argc, &argv);
    
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
  g_signal_connect (window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
  GtkWidget *graphView = graph_gtk_view_new();

  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  //Build the menu
  GtkWidget *menubar;
  GtkWidget *graph_menu;
  GtkWidget *graph;
  GtkWidget *add_node;
  GtkWidget *delete;
  GtkWidget *auto_arrange;

  menubar = gtk_menu_bar_new();
  graph_menu = gtk_menu_new();
  graph = gtk_menu_item_new_with_label("Graph");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(graph), graph_menu);

  add_node = gtk_menu_item_new_with_label("Add Node");
  //g_signal_connect
  gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), add_node);

  delete = gtk_menu_item_new_with_label("Delete");
  //g_signal_connect
  gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), delete);

  auto_arrange = gtk_menu_item_new_with_label("Auto-arrange");
  //g_signal_connect
  gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), auto_arrange);

  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), graphView, TRUE, TRUE, 0);

  //Show all widgets
  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}
