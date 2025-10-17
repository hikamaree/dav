#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "data.h"

void open_overlay(AppData*);
void close_overlay(AppData*);
gboolean draw_overlay(GtkWidget*, cairo_t*, gpointer);

#endif
