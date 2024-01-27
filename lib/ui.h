#ifndef UI_H
#define UI_H

#include "visualizer.h"
#include "data.h"

extern int device;

void create_window(AppData*);
void show_notification(AppData*, const char*);

#endif
