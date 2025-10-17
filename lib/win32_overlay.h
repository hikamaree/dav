#ifndef WIN32_OVERLAY_H
#define WIN32_OVERLAY_H

#include "data.h"
#include <gtk/gtk.h>

gboolean draw_overlay(GtkWidget*, cairo_t*, gpointer);

void open_win32_overlay(AppData*);
void close_win32_overlay(AppData*);

#endif
