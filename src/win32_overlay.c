#include "win32_overlay.h"
#include <gtk/gtk.h>

#ifdef _WIN32
#include <gdk/gdkwin32.h>
#include <windows.h>
#endif

void open_win32_overlay(AppData* data) {
    data->overlay = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_app_paintable(data->overlay, TRUE);
    gtk_widget_set_visual(data->overlay, gdk_screen_get_rgba_visual(gtk_widget_get_screen(data->overlay)));
    g_signal_connect(G_OBJECT(data->overlay), "draw", G_CALLBACK(draw_overlay), data);
    gtk_window_set_decorated(GTK_WINDOW(data->overlay), FALSE);
    
    GdkRectangle geometry;
    GdkMonitor* monitor = gdk_display_get_primary_monitor(gdk_display_get_default());
    gdk_monitor_get_geometry(monitor, &geometry);
    gtk_window_set_default_size(GTK_WINDOW(data->overlay), geometry.width, geometry.height);
    gtk_widget_show_all(data->overlay);
    
#ifdef _WIN32
    HWND hwnd = GDK_WINDOW_HWND(gtk_widget_get_window(data->overlay));
    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
#endif
}

void close_win32_overlay(AppData* data) {
    gtk_widget_destroy(data->overlay);
    data->overlay = NULL;
}