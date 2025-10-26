#include "wayland.h"
#include "visualizer.h"
#include <gtk-layer-shell.h>

void open_wayland_overlay(AppData* data) {
	data->overlay = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(data->overlay, TRUE);
	g_signal_connect(G_OBJECT(data->overlay), "draw", G_CALLBACK(draw_overlay), data);

	gtk_layer_init_for_window(GTK_WINDOW(data->overlay));
	gtk_widget_input_shape_combine_region (data->overlay, data->input_region);

	for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
		gtk_layer_set_anchor(GTK_WINDOW(data->overlay), i, TRUE);
	}
	gtk_layer_set_layer(GTK_WINDOW(data->overlay), GTK_LAYER_SHELL_LAYER_OVERLAY);
	gtk_layer_set_keyboard_mode(GTK_WINDOW(data->overlay), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
	gtk_layer_set_namespace (GTK_WINDOW(data->overlay), "dav");

	gtk_widget_show_all(data->overlay);
}

void close_wayland_overlay(AppData* data) {
	gtk_widget_destroy(data->overlay);
	data->overlay = NULL;
}
