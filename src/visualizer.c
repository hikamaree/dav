#include "visualizer.h"

#if defined(__linux__)
  #include "wayland.h"
  #include "x11.h"
#elif defined(WIN32) || defined(_WIN32)
  #include "win32_overlay.h"
#endif

int device = -1;

gboolean draw_overlay(GtkWidget* widget, cairo_t* cr, gpointer d) {
    AppData* data = (AppData*)d;

    int width, height;
    gtk_window_get_size(GTK_WINDOW(widget), &width, &height);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    cairo_set_source_rgba(cr, data->settings->red, data->settings->green, data->settings->blue, data->settings->alpha);

    float cx = (float)width / 2;
    float cy = (float)height / 2;
    float r = data->settings->space;

    float radius = 0.0f;
    for (int i = 0; i < data->stream->channel_cnt; i++) {
        radius += data->stream->channels[i];
    }
    radius /= data->stream->channel_cnt;

	if(radius > 0.01 && radius <= 0.2) {
		radius *= 4;
	}
	else if(radius > 0.2 && radius < 0.5) {
		radius *= 1.75;
	}
	else if(radius > 0.5) {
		radius *= 1;
	}

    radius *= data->settings->radius;

    float angle = (data->stream->angle + 180) * (G_PI / 180.0f);

    float sx = cx + cosf(angle) * r;
    float sy = cy + sinf(angle) * r;

    cairo_arc(cr, sx, sy, radius, 0, 2 * G_PI);
    cairo_fill(cr);

    gtk_widget_queue_draw((GtkWidget*)widget);
    return FALSE;
}

void open_overlay(AppData* data) {
#if defined(WIN32) || defined(_WIN32)
	open_win32_overlay(data);
#elif defined(__linux__)
	if (getenv("WAYLAND_DISPLAY")) {
		open_wayland_overlay(data);
	} else {
		open_x11_overlay(data);
	}
#endif
}

void close_overlay(AppData* data) {
#if defined(WIN32) || defined(_WIN32)
	close_win32_overlay(data);
#elif defined(__linux__)
	if (getenv("WAYLAND_DISPLAY")) {
		close_wayland_overlay(data);
	} else {
		close_x11_overlay(data);
	}
#endif
}
