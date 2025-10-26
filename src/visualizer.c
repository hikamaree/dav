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
	radius = sqrtf(radius);
    radius *= data->settings->radius;

    float angle = (data->stream->angle + 180) * (G_PI / 180.0f);

    float sx = cx + cosf(angle) * r;
    float sy = cy + sinf(angle) * r;

    cairo_arc(cr, sx, sy, radius, 0, 2 * G_PI);
    cairo_fill(cr);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

	if (data->gif_iter) {
		GdkPixbuf* frame = gdk_pixbuf_animation_iter_get_pixbuf(data->gif_iter);

		GdkPixbuf* scaled_frame = NULL;
		if (data->settings->gif_width > 0 && data->settings->gif_height > 0) {
			scaled_frame = gdk_pixbuf_scale_simple(frame,
												   data->settings->gif_width,
												   data->settings->gif_height,
												   GDK_INTERP_BILINEAR);
		}

		int gif_x = data->settings->gif_x;
		int gif_y = data->settings->gif_y;

		if (scaled_frame) {
			gdk_cairo_set_source_pixbuf(cr, scaled_frame, gif_x, gif_y);
			g_object_unref(scaled_frame);
		} else {
			gdk_cairo_set_source_pixbuf(cr, frame, gif_x, gif_y);
		}

		cairo_paint(cr);

		if (gdk_pixbuf_animation_iter_advance(data->gif_iter, NULL))
			gtk_widget_queue_draw(widget);
	}

#pragma GCC diagnostic pop

	gtk_widget_queue_draw((GtkWidget*)widget);
	return FALSE;
}

void open_overlay(AppData* data) {
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
