#ifndef DATA_H
#define DATA_H

#include <gtk/gtk.h>
#include "audio.h"
#include "config.h"

typedef struct {
	GtkWidget* visualizer;
	cairo_region_t* input_region;
	GtkWidget* window;
	GtkWidget* header;
	GtkWidget* grid;
	GtkWidget* devices;
	GtkWidget* start_stop;
	GtkWidget* device_name;
	StreamData* stream;
	Config* settings;
} AppData;


#endif
