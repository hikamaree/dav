#include "wayland.h"
#include <gtk-layer-shell.h>

gboolean draw(GtkWidget* widget, cairo_t* cr, gpointer d) {
    AppData* data = (AppData*)d;

    int width, height;
    gtk_window_get_size(GTK_WINDOW(widget), &width, &height);

    cairo_set_source_rgba(cr, data->settings->red, data->settings->green, data->settings->blue, data->settings->alpha);

    // float delta = 2 * G_PI / data->stream->channel_cnt;
    // for (int i = 0; i < data->stream->channel_cnt; i += 2) {
    //     float x = data->settings->space * cos(i * delta / 2);
    //     float y = data->settings->space * sin(i * delta / 2);
    //     cairo_arc(cr, (float)width / 2 - x, (float)height / 2 - y, data->settings->radius * data->stream->channels[i], 0, 2 * G_PI);
    //     cairo_arc(cr, (float)width / 2 + x, (float)height / 2 + y, data->settings->radius * data->stream->channels[i + 1], 0, 2 * G_PI);
    //     cairo_fill(cr);
    // }

    float cx = (float)width / 2;
    float cy = (float)height / 2;
    float r = data->settings->space;

    float radius = 0.0f;
    for (int i = 0; i < data->stream->channel_cnt; i++) {
        radius += data->stream->channels[i];
    }
    radius /= data->stream->channel_cnt;
    
    radius *= data->settings->radius;

    float angle = (data->stream->angle + 180) * (G_PI / 180.0f);

    float sx = cx + cosf(angle) * r;
    float sy = cy + sinf(angle) * r;

    cairo_arc(cr, sx, sy, radius, 0, 2 * G_PI);
    cairo_fill(cr);

    gtk_widget_queue_draw((GtkWidget*)widget);
    return FALSE;
}

void open_wayland_overlay(AppData* data) {
	data->visualizer = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_app_paintable(data->visualizer, TRUE);
	g_signal_connect(G_OBJECT(data->visualizer), "draw", G_CALLBACK(draw), data);

	gtk_layer_init_for_window(GTK_WINDOW(data->visualizer));
	gtk_widget_input_shape_combine_region (data->visualizer, data->input_region);

	for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
		gtk_layer_set_anchor(GTK_WINDOW(data->visualizer), i, TRUE);
	}
	gtk_layer_set_layer(GTK_WINDOW(data->visualizer), GTK_LAYER_SHELL_LAYER_OVERLAY);
	gtk_layer_set_keyboard_mode(GTK_WINDOW(data->visualizer), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
	gtk_layer_set_namespace (GTK_WINDOW(data->visualizer), "dav");

	gtk_widget_show_all(data->visualizer);
}

void close_wayland_overlay(AppData* data) {
	gtk_widget_destroy(data->visualizer);
	data->visualizer = NULL;
}
