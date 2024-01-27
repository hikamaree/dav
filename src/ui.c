#include "ui.h"

void show_notification(AppData* data, const char* message) {
    GtkWidget* dialog = gtk_dialog_new_with_buttons("Custom Popup", GTK_WINDOW(data->window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_OK, NULL);
    GtkWidget* label = gtk_label_new(message);
    gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label, TRUE, TRUE, 0);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void select_device(GtkWidget* widget, gpointer d) {
	int data = GPOINTER_TO_INT(d);
	device = data;
}

void start_stop(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	if (data->visualizer == NULL) {
		if(device < 0 || device > Pa_GetDeviceCount()) {
			show_notification(data, "\tSelect valid source\t");
			return;
		}
		data->stream->device = device;
		start_stream(data->stream);
		gtk_label_set_text(GTK_LABEL(data->device_name), Pa_GetDeviceInfo(device)->name);
		gtk_button_set_label(GTK_BUTTON(data->start_stop), "Stop");
		open_overlay(data);
	}
	else {
		gtk_button_set_label(GTK_BUTTON(data->start_stop), "Start");
		close_overlay(data);
		close_stream(data->stream);
	}
}

void refresh(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	if(data->visualizer != NULL) {
		start_stop(NULL, data);
	}

	refresh_stream(data->stream);

	GList* children = gtk_container_get_children(GTK_CONTAINER(data->devices));
	for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) {
		GtkWidget* menuItem = GTK_WIDGET(iter->data);
		gtk_container_remove(GTK_CONTAINER(data->devices), menuItem);
	}
	g_list_free(children);

	int cnt = Pa_GetDeviceCount();
	const PaDeviceInfo* device_info;
	for (int device = 0; device < cnt; device++) {
		device_info = Pa_GetDeviceInfo(device);
		if(device_info->maxInputChannels > 0 && device_info->maxInputChannels < 8) {
			GtkWidget* menuItem = gtk_menu_item_new_with_label(device_info->name);
			g_signal_connect(G_OBJECT(menuItem), "activate", G_CALLBACK(select_device), GINT_TO_POINTER(device));
			gtk_menu_shell_append(GTK_MENU_SHELL(data->devices), menuItem);
			gtk_widget_show(menuItem);
		}
	}
	device = -1;
}

void set_radius(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->radius = gtk_range_get_value(GTK_RANGE(widget));
	write_config(data->settings);
}

void set_space(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->space = gtk_range_get_value(GTK_RANGE(widget));
	write_config(data->settings);
}

void set_speed(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->stream->speed = gtk_range_get_value(GTK_RANGE(widget));
	data->settings->speed = gtk_range_get_value(GTK_RANGE(widget));
	write_config(data->settings);
}

void set_red(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->red = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	write_config(data->settings);
}

void set_green(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->green = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	write_config(data->settings);
}

void set_blue(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->blue = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	write_config(data->settings);
}

void set_alpha(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->alpha = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	write_config(data->settings);
}

void close_dav(GtkWidget* window, gpointer d) {
	AppData* data = (AppData*)d;
	if(data->visualizer != NULL) {
		gtk_widget_destroy(data->visualizer);
	}
	close_stream(data->stream);

	free(data->input_region);
	free(data->settings->path);
	free(data->settings);
	free(data->stream);
	free(data);
	gtk_main_quit();
}

void create_window(AppData* data) {
	data->visualizer = NULL;
	data->input_region = cairo_region_create();
	data->stream->speed = data->settings->speed;

	data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(data->window), 600, 400);
	g_signal_connect(G_OBJECT(data->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	data->header = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(data->header), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(data->header), "Directional Audio Visualizer");
	gtk_window_set_titlebar(GTK_WINDOW(data->window), data->header);

	GtkWidget* menubar = gtk_menu_bar_new();
	GtkWidget* file_menu_item = gtk_menu_item_new_with_label("Device");

	data->devices = gtk_menu_new();
	refresh(NULL, data);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), data->devices);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_menu_item);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(data->header), menubar);

	data->start_stop = gtk_button_new_with_label("Start");
	GtkWidget* button2 = gtk_button_new_with_label("Refresh");
	g_signal_connect(G_OBJECT(data->start_stop), "clicked", G_CALLBACK(start_stop), data);
	g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(refresh), data);

	gtk_header_bar_pack_start(GTK_HEADER_BAR(data->header), data->start_stop);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(data->header), button2);

	data->grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(data->grid), TRUE);

	data->device_name = gtk_label_new("");

	GtkWidget* radius = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(radius), "value-changed", G_CALLBACK(set_radius), data);
	gtk_range_set_value(GTK_RANGE(radius), data->settings->radius);

	GtkWidget* space = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(space), "value-changed", G_CALLBACK(set_space), data);
	gtk_range_set_value(GTK_RANGE(space), data->settings->space);

	GtkWidget* speed = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1000, 1);
	g_signal_connect(G_OBJECT(speed), "value-changed", G_CALLBACK(set_speed), data);
	gtk_range_set_value(GTK_RANGE(speed), data->stream->speed);

	GtkWidget* red = gtk_spin_button_new_with_range(0, 1.0, 0.1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(red), data->settings->red);
	g_signal_connect(red, "value-changed", G_CALLBACK(set_red), data);

	GtkWidget* green = gtk_spin_button_new_with_range(0, 1.0, 0.1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(green), data->settings->green);
	g_signal_connect(green, "value-changed", G_CALLBACK(set_green), data);

	GtkWidget* blue = gtk_spin_button_new_with_range(0, 1.0, 0.1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(blue), data->settings->blue);
	g_signal_connect(blue, "value-changed", G_CALLBACK(set_blue), data);

	GtkWidget* alpha = gtk_spin_button_new_with_range(0, 1.0, 0.1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(alpha), data->settings->alpha);
	g_signal_connect(alpha, "value-changed", G_CALLBACK(set_alpha), data);

	gtk_grid_attach(GTK_GRID(data->grid), data->device_name, 0, 0, 6, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Radius"), 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), radius, 1, 1, 5, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Distance"), 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), space, 1, 2, 5, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Speed"), 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), speed, 1, 3, 5, 1);

	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("\nColor\n"), 2, 5, 2, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Red"), 1, 7, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), red, 2, 7, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Green"), 3, 7, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), green, 4, 7, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Blue"), 1, 8, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), blue, 2, 8, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), gtk_label_new("Alpha"), 3, 8, 1, 1);
	gtk_grid_attach(GTK_GRID(data->grid), alpha, 4, 8, 1, 1);

	gtk_container_add (GTK_CONTAINER (data->window), data->grid);
	g_signal_connect(data->window, "destroy", G_CALLBACK(close_dav), data);
	gtk_widget_show_all(data->window);
}
