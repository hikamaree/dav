#include "visualizer.h"
#include "data.h"
#include "gif.h"

#if defined(__linux__)
  #include "wayland.h"
  #include "x11.h"
#elif defined(WIN32) || defined(_WIN32)
  #include "win32_overlay.h"
#endif

int device = -1;

void draw_visualizer(GtkWidget* widget, cairo_t* cr, AppData* data) {
	if(!data->visualizer) {
		return;
	}

    int width, height;
    gtk_window_get_size(GTK_WINDOW(widget), &width, &height);

    cairo_set_source_rgba(cr, data->settings->red, data->settings->green, data->settings->blue, data->settings->alpha);

    float cx = (float)width / 2;
    float cy = (float)height / 2;
    float r = data->settings->space;

    float radius = 0.0f;
    for (int i = 0; i < data->stream->channel_cnt; i++) {
        radius += data->stream->channels[i];
    }
    radius /= data->stream->channel_cnt;
	radius = sqrtf(radius);
    radius *= data->settings->radius;

    float angle = (data->stream->angle + 180) * (G_PI / 180.0f);

    float sx = cx + cosf(angle) * r;
    float sy = cy + sinf(angle) * r;

    cairo_arc(cr, sx, sy, radius, 0, 2 * G_PI);
    cairo_fill(cr);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

void draw_gifs(GtkWidget* widget, cairo_t* cr, AppData* data) {
	for (GList* l = data->gifs; l != NULL; l = l->next) {
		Gif* gif = (Gif*)l->data;
		if (!gif || !gif->gif_iter)
			continue;

		GdkPixbuf* frame = gdk_pixbuf_animation_iter_get_pixbuf(gif->gif_iter);
		GdkPixbuf* scaled_frame = NULL;

		if (gif->gif_width > 0 && gif->gif_height > 0) {
			scaled_frame = gdk_pixbuf_scale_simple(frame,
												   gif->gif_width,
												   gif->gif_height,
												   GDK_INTERP_BILINEAR);
		}

		gdk_cairo_set_source_pixbuf(cr,
			scaled_frame ? scaled_frame : frame,
			gif->gif_x,
			gif->gif_y);

		cairo_paint(cr);

		if (scaled_frame)
			g_object_unref(scaled_frame);

		if (gdk_pixbuf_animation_iter_advance(gif->gif_iter, NULL))
			gtk_widget_queue_draw(widget);
	}
}

#pragma GCC diagnostic pop

gboolean draw_overlay(GtkWidget* widget, cairo_t* cr, gpointer d) {
    AppData* data = (AppData*)d;

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	draw_gifs(widget, cr, data);
	draw_visualizer(widget, cr, data);

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
