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
	if (GTK_IS_COMBO_BOX(widget)) {
		const char* id = gtk_combo_box_get_active_id(GTK_COMBO_BOX(widget));
		if (id) {
			device = atoi(id);
			AppData* data = (AppData*)d;
			const PaDeviceInfo* di = Pa_GetDeviceInfo(device);
			if (di) {
				gtk_label_set_text(GTK_LABEL(data->device_name), di->name);
			}
		}
		return;
	}

	int data_int = GPOINTER_TO_INT(d);
	device = data_int;
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

	if (GTK_IS_COMBO_BOX_TEXT(data->devices)) {
		gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(data->devices));
		int cnt = Pa_GetDeviceCount();
		const PaDeviceInfo* device_info;
		for (int dev = 0; dev < cnt; dev++) {
			device_info = Pa_GetDeviceInfo(dev);
			if (device_info->maxInputChannels > 0 && device_info->maxInputChannels < 8) {
				char idbuf[32];
				snprintf(idbuf, sizeof(idbuf), "%d", dev);
				gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(data->devices), idbuf, device_info->name);
			}
		}
		gtk_combo_box_set_active(GTK_COMBO_BOX(data->devices), -1);
		device = -1;
		return;
	}

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

void set_gif_path(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;

	GtkWidget *dialog = gtk_file_chooser_dialog_new("Select GIF File",
													GTK_WINDOW(data->window),
													GTK_FILE_CHOOSER_ACTION_OPEN,
													"_Cancel", GTK_RESPONSE_CANCEL,
													"_Open", GTK_RESPONSE_ACCEPT,
													NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, "GIF Images");
	gtk_file_filter_add_pattern(filter, "*.gif");
	gtk_file_filter_add_pattern(filter, "*.GIF");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	GtkFileFilter *all_filter = gtk_file_filter_new();
	gtk_file_filter_set_name(all_filter, "All Files");
	gtk_file_filter_add_pattern(all_filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), all_filter);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
		if (filename) {
			if (data->settings->gif_path) {
				free(data->settings->gif_path);
			}
			data->settings->gif_path = strdup(filename);
			write_config(data->settings);
			g_free(filename);

			GtkWidget* gif_path_label = gtk_bin_get_child(GTK_BIN(widget));
			if (GTK_IS_LABEL(gif_path_label)) {
				char* basename = g_path_get_basename(data->settings->gif_path);
				gtk_label_set_text(GTK_LABEL(gif_path_label), basename);
				g_free(basename);
			}
		}
	}

	gtk_widget_destroy(dialog);
}

void set_gif_x(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->gif_x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	write_config(data->settings);
}

void set_gif_y(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->gif_y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	write_config(data->settings);
}

void set_gif_width(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->gif_width = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	write_config(data->settings);
}

void set_gif_height(GtkWidget* widget, gpointer d) {
	AppData* data = (AppData*)d;
	data->settings->gif_height = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
	write_config(data->settings);
}

void create_window(AppData* data) {
	data->visualizer = NULL;
	data->input_region = cairo_region_create();
	data->stream->speed = data->settings->speed;

	data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(data->window), 600, 500);
	g_signal_connect(G_OBJECT(data->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	data->header = gtk_header_bar_new();
	gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(data->header), TRUE);
	gtk_header_bar_set_title(GTK_HEADER_BAR(data->header), "Directional Audio Visualizer");
	gtk_window_set_titlebar(GTK_WINDOW(data->window), data->header);

	GtkWidget* outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
	gtk_container_set_border_width(GTK_CONTAINER(outer_box), 10);

	data->device_name = gtk_label_new("");
	gtk_widget_set_halign(data->device_name, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(outer_box), data->device_name, FALSE, FALSE, 0);

	GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
	data->grid = NULL;
	gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);

	GtkWidget* settings_frame = gtk_frame_new("Settings");
	gtk_widget_set_hexpand(settings_frame, TRUE);

	GtkWidget* settings_grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(settings_grid), 8);
	gtk_grid_set_row_spacing(GTK_GRID(settings_grid), 6);
	gtk_widget_set_margin_start(settings_grid, 12);
	gtk_widget_set_margin_end(settings_grid, 12);
	gtk_widget_set_margin_top(settings_grid, 12);
	gtk_widget_set_margin_bottom(settings_grid, 12);
	gtk_widget_set_hexpand(settings_grid, TRUE);
	gtk_container_add(GTK_CONTAINER(settings_frame), settings_grid);

	GtkWidget* controls_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_widget_set_halign(controls_box, GTK_ALIGN_START);
	gtk_widget_set_valign(controls_box, GTK_ALIGN_START);
	gtk_widget_set_hexpand(controls_box, FALSE);

	data->devices = gtk_combo_box_text_new();
	gtk_widget_set_hexpand(data->devices, FALSE);
	GtkCellRenderer *cell = gtk_cell_renderer_text_new();
	gtk_cell_renderer_set_fixed_size(GTK_CELL_RENDERER(cell), 100, -1);
	g_object_set(G_OBJECT(cell), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
	gtk_cell_layout_clear(GTK_CELL_LAYOUT(data->devices));
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(data->devices), cell, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(data->devices), cell, "text", 0, NULL);
	g_signal_connect(G_OBJECT(data->devices), "changed", G_CALLBACK(select_device), data);
	refresh(NULL, data);
	gtk_widget_set_sensitive(data->devices, TRUE);

	data->start_stop = gtk_button_new_with_label("Start");
	GtkWidget* button2 = gtk_button_new_with_label("Refresh");
	g_signal_connect(G_OBJECT(data->start_stop), "clicked", G_CALLBACK(start_stop), data);
	g_signal_connect(G_OBJECT(button2), "clicked", G_CALLBACK(refresh), data);

	gtk_widget_set_size_request(data->devices, 100, -1);
	gtk_widget_set_size_request(controls_box, 100, -1);
	g_object_set(G_OBJECT(data->devices), "width-chars", 10, NULL);
	gtk_box_pack_start(GTK_BOX(controls_box), data->devices, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(controls_box), data->start_stop, TRUE, FALSE, 6);
	gtk_box_pack_start(GTK_BOX(controls_box), button2, TRUE, FALSE, 0);

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

	GtkWidget* gif_path_button = gtk_button_new_with_label("Select GIF");
	g_signal_connect(G_OBJECT(gif_path_button), "clicked", G_CALLBACK(set_gif_path), data);

	char* current_gif_basename = g_path_get_basename(data->settings->gif_path);
	g_free(current_gif_basename);

	GtkWidget* gif_x = gtk_spin_button_new_with_range(0, 5000, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gif_x), data->settings->gif_x);
	g_signal_connect(gif_x, "value-changed", G_CALLBACK(set_gif_x), data);

	GtkWidget* gif_y = gtk_spin_button_new_with_range(0, 5000, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gif_y), data->settings->gif_y);
	g_signal_connect(gif_y, "value-changed", G_CALLBACK(set_gif_y), data);

	GtkWidget* gif_width = gtk_spin_button_new_with_range(1, 1000, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gif_width), data->settings->gif_width);
	g_signal_connect(gif_width, "value-changed", G_CALLBACK(set_gif_width), data);

	GtkWidget* gif_height = gtk_spin_button_new_with_range(1, 1000, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gif_height), data->settings->gif_height);
	g_signal_connect(gif_height, "value-changed", G_CALLBACK(set_gif_height), data);

	int row = 0;
	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("Radius"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), radius, 1, row, 1, 1);
	gtk_widget_set_hexpand(radius, TRUE);
	row++;

	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("Distance"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), space, 1, row, 1, 1);
	gtk_widget_set_hexpand(space, TRUE);
	row++;

	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("Speed"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), speed, 1, row, 1, 1);
	gtk_widget_set_hexpand(speed, TRUE);
	row++;

	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("Red"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), red, 1, row, 1, 1);
	row++;
	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("Green"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), green, 1, row, 1, 1);
	row++;
	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("Blue"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), blue, 1, row, 1, 1);
	row++;
	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("Alpha"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), alpha, 1, row, 1, 1);
	row++;

	GtkWidget* separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_grid_attach(GTK_GRID(settings_grid), separator, 0, row, 2, 1);
	row++;

	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("GIF File"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), gif_path_button, 1, row, 1, 1);
	row++;

	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("GIF X Position"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), gif_x, 1, row, 1, 1);
	row++;
	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("GIF Y Position"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), gif_y, 1, row, 1, 1);
	row++;
	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("GIF Width"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), gif_width, 1, row, 1, 1);
	row++;
	gtk_grid_attach(GTK_GRID(settings_grid), gtk_label_new("GIF Height"), 0, row, 1, 1);
	gtk_grid_attach(GTK_GRID(settings_grid), gif_height, 1, row, 1, 1);

	gtk_box_pack_start(GTK_BOX(main_box), settings_frame, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(main_box), controls_box, FALSE, FALSE, 6);

	gtk_box_pack_start(GTK_BOX(outer_box), main_box, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(data->window), outer_box);
	g_signal_connect(data->window, "destroy", G_CALLBACK(close_dav), data);
	gtk_widget_show_all(data->window);
}
