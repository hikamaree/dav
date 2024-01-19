#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include "audio.h"
#include "visualizer.h"
#include "config.h"

typedef struct {
	GtkWidget* visualizer;
	GtkWidget* window;
	GtkWidget* header;
	GtkWidget* grid;
	GtkWidget* devices;
	GtkWidget* start_stop;
	GtkWidget* device_name;
	StreamData* stream;
	Config* settings;
} AppData;

extern int device;

void create_window(AppData*);
void show_notification(AppData*, const char*);

#endif
