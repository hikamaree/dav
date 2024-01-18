#include "visualizer.h"
#include "ui.h"

int device = -1;

gboolean draw_visualizer(GtkWidget *widget, cairo_t *cr, gpointer d) {
	AppData* data = (AppData*)d;

	int width, height;
	gtk_window_get_size(GTK_WINDOW(widget), &width, &height);
	float delta = 2 * G_PI / data->stream->channel_cnt;

	cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.5);
	for (int i = 0; i < data->stream->channel_cnt; i += 2) {
		float x = data->space * cos(i * delta / 2);
		float y = data->space * sin(i * delta / 2);

		cairo_arc(cr, (float)width / 2 - x, (float)height / 2 - y, data->radius * data->stream->channels[i], 0, 2 * G_PI);
		cairo_arc(cr, (float)width / 2 + x, (float)height / 2 + y, data->radius * data->stream->channels[i + 1], 0, 2 * G_PI);
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
			g_print("select device\n");
			return;
		}
		data->stream->device = device;
		start_stream(data->stream);
		gtk_button_set_label(GTK_BUTTON(data->start_stop), "Stop");
		data->visualizer = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_type_hint(GTK_WINDOW(data->visualizer), GDK_WINDOW_TYPE_HINT_TOOLTIP);
		gtk_window_set_skip_taskbar_hint(GTK_WINDOW(data->visualizer), TRUE);
		gtk_window_set_skip_pager_hint(GTK_WINDOW(data->visualizer), TRUE);
		gtk_widget_set_app_paintable(data->visualizer, TRUE);
		gtk_window_set_decorated(GTK_WINDOW(data->visualizer), FALSE);
		gtk_window_set_position(GTK_WINDOW(data->visualizer), GTK_WIN_POS_CENTER);
		gtk_window_set_default_size(GTK_WINDOW(data->visualizer), 1280, 720);
		gtk_window_set_keep_above(GTK_WINDOW(data->visualizer), TRUE);

		g_signal_connect(G_OBJECT(data->visualizer), "draw", G_CALLBACK(draw_visualizer), data);
		g_signal_connect(G_OBJECT(data->visualizer), "delete-event", G_CALLBACK(on_visualizer_close), NULL);

		gtk_widget_show_all(data->visualizer);
	} else {
		gtk_button_set_label(GTK_BUTTON(data->start_stop), "Start");
		gtk_widget_destroy(data->visualizer);
		data->visualizer = NULL;
		close_stream(data->stream);
	}
}


