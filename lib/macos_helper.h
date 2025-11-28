#ifndef MACOS_HELPER_H
#define MACOS_HELPER_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#ifdef __APPLE__
void macos_set_window_on_all_spaces(GdkWindow *gdk_window);
void* macos_create_settings_window(void* app_data);
void macos_show_settings_window(void* window);
void macos_destroy_settings_window(void* window);
#endif

#endif
