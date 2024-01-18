#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include "audio.h"
#include "visualizer.h"

typedef struct {
	GtkWidget* visualizer;
	GtkWidget* devices;
	GtkWidget* start_stop;
	StreamData* stream;
	int radius;
	int space;
} AppData;

extern int device;

void create_window(AppData* data);

#endif
