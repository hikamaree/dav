#include "visualizer.h"
#include "ui.h"
#include <gtk-layer-shell.h>

int device = -1;

gboolean draw_visualizer(GtkWidget *widget, cairo_t *cr, gpointer d) {
	AppData* data = (AppData*)d;

	int width, height;
	gtk_window_get_size(GTK_WINDOW(widget), &width, &height);
	float delta = 2 * G_PI / data->stream->channel_cnt;

	cairo_set_source_rgba(cr, data->settings->red, data->settings->green, data->settings->blue, data->settings->alpha);
	for (int i = 0; i < data->stream->channel_cnt; i += 2) {
		float x = data->settings->space * cos(i * delta / 2);
		float y = data->settings->space * sin(i * delta / 2);
		cairo_arc(cr, (float)width / 2 - x, (float)height / 2 - y, data->settings->radius * data->stream->channels[i], 0, 2 * G_PI);
		cairo_arc(cr, (float)width / 2 + x, (float)height / 2 + y, data->settings->radius * data->stream->channels[i + 1], 0, 2 * G_PI);
		cairo_fill(cr);
	}
	gtk_widget_queue_draw((GtkWidget*)widget);
	return FALSE;
}

gboolean on_visualizer_close(GtkWidget *widget, GdkEvent *event, gpointer data) {
	return TRUE;
}

void start_stop(GtkWidget *widget, gpointer d) {
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
		data->visualizer = gtk_window_new(GTK_WINDOW_TOPLEVEL);

////////////////////////////
		gtk_layer_init_for_window (GTK_WINDOW(data->visualizer));
		gtk_layer_set_keyboard_mode(GTK_WINDOW(data->visualizer), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
		gtk_layer_set_layer(GTK_WINDOW(data->visualizer), GTK_LAYER_SHELL_LAYER_OVERLAY);
		gtk_widget_set_app_paintable(data->visualizer, TRUE);

		static const gboolean anchors[] = {TRUE, TRUE, TRUE, TRUE};
		for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
			gtk_layer_set_anchor (GTK_WINDOW(data->visualizer), i, anchors[i]);
		}
////////////////////////////

		/// Transparency for x11
		GdkScreen* screen = gdk_screen_get_default();
		GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
		if (visual != NULL && gdk_screen_is_composited(screen)) {
			gtk_widget_set_visual(data->visualizer, visual);
		}

		g_signal_connect(G_OBJECT(data->visualizer), "draw", G_CALLBACK(draw_visualizer), data);
		gtk_widget_show_all(data->visualizer);
	} else {
		gtk_button_set_label(GTK_BUTTON(data->start_stop), "Start");
		gtk_widget_destroy(data->visualizer);
		data->visualizer = NULL;
		close_stream(data->stream);
	}
}
