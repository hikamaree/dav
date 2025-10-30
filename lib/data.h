#ifndef DATA_H
#define DATA_H

#include <gtk/gtk.h>
#include "audio.h"
#include "config.h"
#include <stdbool.h>

typedef struct {
	GtkWidget* overlay;
	cairo_region_t* input_region;
	GtkWidget* window;
	GtkWidget* header;
	GtkWidget* grid;
	GtkWidget* devices;
	GtkWidget* start_stop;
	GtkWidget* device_name;
	StreamData* stream;
	Config* settings;
	bool visualizer;
	GList* gifs;
	GtkWidget* notebook;
} AppData;

#endif
