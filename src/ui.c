#include "ui.h"

void select_device(GtkWidget* widget, gpointer d) {
	int data = GPOINTER_TO_INT(d);
	device = data;
}

void refresh(GtkWidget *widget, gpointer d) {
	AppData* data = (AppData*)d;
	if(data->visualizer != NULL) {
		start_stop(NULL, data);
	}

	refresh_devices(data->stream);

	GList *children = gtk_container_get_children(GTK_CONTAINER(data->devices));
	for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
		GtkWidget *menuItem = GTK_WIDGET(iter->data);
		gtk_container_remove(GTK_CONTAINER(data->devices), menuItem);
		g_object_unref(menuItem);
	}
	g_list_free(children);

	int cnt = Pa_GetDeviceCount();
	const PaDeviceInfo* device_info;
	for (int device = 0; device < cnt; device++) {
		device_info = Pa_GetDeviceInfo(device);
		if(device_info->maxInputChannels > 0 && device_info->maxInputChannels < 8) {
			GtkWidget *menuItem = gtk_menu_item_new_with_label(device_info->name);
			g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(select_device), GINT_TO_POINTER(device));
			gtk_menu_shell_append(GTK_MENU_SHELL(data->devices), menuItem);
			gtk_widget_show(menuItem);
		}
	}
	device = -1;
}

void set_radius(GtkWidget *widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->radius = gtk_range_get_value(GTK_RANGE(widget));
}

void set_space(GtkWidget *widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->space = gtk_range_get_value(GTK_RANGE(widget));
}

void set_speed(GtkWidget *widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->stream->speed = gtk_range_get_value(GTK_RANGE(widget));
}

void create_window(AppData* data) {
	data->visualizer = NULL;
	data->devices = gtk_menu_new();
	data->radius = 100;
	data->space = 200;
	data->stream->speed = 400;

	GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	GtkWidget *header = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Directional Audio Visualizer");
	gtk_window_set_titlebar(GTK_WINDOW(window), header);

	GtkWidget *menubar = gtk_menu_bar_new();
	GtkWidget *file_menu_item = gtk_menu_item_new_with_label("Device");
	refresh(NULL, data);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), data->devices);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_menu_item);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), menubar);

	data->start_stop = gtk_button_new_with_label("Start");
	GtkWidget *button2 = gtk_button_new_with_label("Refresh");
	g_signal_connect(G_OBJECT(data->start_stop), "clicked", G_CALLBACK(start_stop), data);
	g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(refresh), data);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), data->start_stop);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button2);

	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

	GtkWidget *label1 = gtk_label_new("redius");
	GtkWidget *radius = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(radius), "value-changed", G_CALLBACK(set_radius), data);
	gtk_range_set_value(GTK_RANGE(radius), data->radius);

	GtkWidget *space = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(space), "value-changed", G_CALLBACK(set_space), data);
	gtk_range_set_value(GTK_RANGE(space), data->space);

	GtkWidget *speed = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(speed), "value-changed", G_CALLBACK(set_speed), data);
	gtk_range_set_value(GTK_RANGE(speed), data->stream->speed);

	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Radius"), 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), radius, 1, 0, 10, 1);
	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Distance"), 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), space, 1, 1, 10, 1);
	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Speed"), 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), speed, 1, 2, 10, 1);

	gtk_container_add (GTK_CONTAINER (window), grid);

	gtk_widget_show_all(window);
}
