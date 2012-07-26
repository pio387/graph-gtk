#include <gtk/gtk.h>
#include <graph-gtk-view.h>

static void menu_item_activated (GtkMenuItem*, gpointer);
static void button_clicked(GtkButton*, gpointer);

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
  g_signal_connect(add_node, "activate", (GCallback)menu_item_activated, graphView);
  gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), add_node);

  delete = gtk_menu_item_new_with_label("Delete Node");
  g_signal_connect(delete, "activate", (GCallback)menu_item_activated, graphView);
  gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), delete);

  auto_arrange = gtk_menu_item_new_with_label("Auto-arrange");
  g_signal_connect(auto_arrange, "activate", (GCallback)menu_item_activated, graphView);
  gtk_menu_shell_append(GTK_MENU_SHELL(graph_menu), auto_arrange);

  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), graph);

  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), graphView, TRUE, TRUE, 0);

  //Show all widgets
  gtk_widget_show_all(window);

  gtk_main();

  return 0;
}

static void menu_item_activated (GtkMenuItem* menu_item, gpointer data)
{
  const gchar* label = gtk_menu_item_get_label(menu_item);

  if(0 == g_strcmp0(label, "Add Node"))
    {
      GtkWidget* dialog = gtk_dialog_new_with_buttons ("Add Node",
						       GTK_WINDOW(gtk_widget_get_ancestor (GTK_WIDGET(menu_item), GTK_TYPE_WINDOW)),
						       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
						       GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
						       NULL);

      gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 80);

      GtkWidget* vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

      GtkWidget* name_label = gtk_label_new("Name");
      GtkWidget* name_entry = gtk_entry_new();
      gtk_box_pack_start(GTK_BOX(vbox), name_label, FALSE, FALSE, 0);
      gtk_box_pack_start(GTK_BOX(vbox), name_entry, FALSE, FALSE, 0);

      GtkWidget* columns = gtk_hbox_new(TRUE, 0);
      gtk_box_pack_start(GTK_BOX(vbox), columns, TRUE, TRUE, 0);

      GtkWidget* left_column = gtk_vbox_new(FALSE, 0);
      gtk_box_pack_start_defaults(GTK_BOX(columns), left_column);
      gtk_box_pack_start(GTK_BOX(left_column), gtk_label_new("Inputs"), FALSE, FALSE, 0);

      GtkWidget* right_column = gtk_vbox_new(FALSE, 0);
      gtk_box_pack_start_defaults(GTK_BOX(columns), right_column);
      gtk_box_pack_start(GTK_BOX(right_column), gtk_label_new("Outputs"), FALSE, FALSE, 0);

      GtkWidget* inputs = gtk_vbox_new(FALSE, 0);
      gtk_box_pack_start_defaults(GTK_BOX(left_column), inputs);

      GtkWidget* outputs = gtk_vbox_new(FALSE, 0);
      gtk_box_pack_start_defaults(GTK_BOX(right_column), outputs);

      GtkWidget* add_input = gtk_button_new_with_label("Add");
      g_signal_connect(add_input, "clicked", (GCallback)button_clicked, inputs);
      gtk_box_pack_start(GTK_BOX(left_column), add_input, FALSE, FALSE, 0);

      GtkWidget* add_output = gtk_button_new_with_label("Add");
      g_signal_connect(add_output, "clicked", (GCallback)button_clicked, outputs);
      gtk_box_pack_start(GTK_BOX(right_column), add_output, FALSE, FALSE, 0);

      gtk_widget_show_all(vbox);

      gint result = gtk_dialog_run(GTK_DIALOG(dialog));

      if(result == GTK_RESPONSE_ACCEPT)
	{
	  GraphGtkNode *node = graph_gtk_node_new();
	  graph_gtk_node_set_name(node, gtk_entry_get_text(GTK_ENTRY(name_entry)));

	  GList* list;
	  for(list = gtk_container_get_children(GTK_CONTAINER(inputs)); list != NULL; list = list->next)
	    {
	      GtkWidget *entry = (GtkWidget*)list->data;
	      if(GTK_IS_ENTRY(entry))
		graph_gtk_node_add_pad(node, gtk_entry_get_text(GTK_ENTRY(entry)), FALSE);
	    }

	  for(list = gtk_container_get_children(GTK_CONTAINER(outputs)); list != NULL; list = list->next)
	    {
	      GtkWidget *entry = (GtkWidget*)list->data;
	      if(GTK_IS_ENTRY(entry))
		graph_gtk_node_add_pad(node, gtk_entry_get_text(GTK_ENTRY(entry)), TRUE);
	    }

	  GraphGtkView *graph = (GraphGtkView*)data;
	  graph_gtk_view_add_node(graph, node);

	  //unref node?
	}
      gtk_widget_destroy(dialog);
    }
  else if(0 == g_strcmp0(label, "Delete Node"))
    {

    }
  else if(0 == g_strcmp0(label, "Auto-arrange"))
    {

    }
}

static void button_clicked(GtkButton *button, gpointer data)
{
  GtkWidget *vbox = (GtkWidget*)data;
  GtkWidget *entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 2);
  gtk_widget_show(GTK_WIDGET(entry));
}
