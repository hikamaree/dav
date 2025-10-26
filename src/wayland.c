#include "wayland.h"
#include "visualizer.h"
#include <gtk-layer-shell.h>

void open_wayland_overlay(AppData* data) {
	data->visualizer = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(data->visualizer, TRUE);
	g_signal_connect(G_OBJECT(data->visualizer), "draw", G_CALLBACK(draw_overlay), data);

	gtk_layer_init_for_window(GTK_WINDOW(data->visualizer));
	gtk_widget_input_shape_combine_region (data->visualizer, data->input_region);

	for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
		gtk_layer_set_anchor(GTK_WINDOW(data->visualizer), i, TRUE);
	}
	gtk_layer_set_layer(GTK_WINDOW(data->visualizer), GTK_LAYER_SHELL_LAYER_OVERLAY);
	gtk_layer_set_keyboard_mode(GTK_WINDOW(data->visualizer), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
	gtk_layer_set_namespace (GTK_WINDOW(data->visualizer), "dav");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

	GError* err = NULL;
	data->gif_anim = gdk_pixbuf_animation_new_from_file(data->settings->gif_path, &err);
	if (!data->gif_anim) {
		g_warning("Failed to load %s: %s",data->settings->gif_path, err->message);
		g_clear_error(&err);
	} else {
		data->gif_iter = gdk_pixbuf_animation_get_iter(data->gif_anim, NULL);
	}

#pragma GCC diagnostic pop

	gtk_widget_show_all(data->visualizer);
}

void close_wayland_overlay(AppData* data) {
	gtk_widget_destroy(data->visualizer);
	data->visualizer = NULL;
}
