#ifndef UI_H
#define UI_H

#include "data.h"

extern int device;

void create_window(AppData*);
void show_notification(AppData*, const char*);
void close_dav(GtkWidget* window, gpointer d);
void open_gif_tab(AppData* data, void* gif);

#endif
