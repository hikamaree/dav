#include "win32_overlay.h"
#include <gtk/gtk.h>

void open_win32_overlay(AppData* data) {
    data->overlay = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_app_paintable(data->overlay, TRUE);
    
    GdkScreen *screen = gtk_widget_get_screen(data->overlay);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(data->overlay, visual);
    }
    
    g_signal_connect(G_OBJECT(data->overlay), "draw", G_CALLBACK(draw_overlay), data);
    
    gtk_window_set_decorated(GTK_WINDOW(data->overlay), FALSE);
    gtk_window_set_accept_focus(GTK_WINDOW(data->overlay), FALSE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(data->overlay), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(data->overlay), TRUE);
    gtk_window_set_keep_above(GTK_WINDOW(data->overlay), TRUE);
    
    GdkRectangle geometry;
    GdkMonitor* monitor = gdk_display_get_primary_monitor(gdk_display_get_default());
    gdk_monitor_get_geometry(monitor, &geometry);
    gtk_window_move(GTK_WINDOW(data->overlay), geometry.x, geometry.y);
    gtk_window_set_default_size(GTK_WINDOW(data->overlay), geometry.width, geometry.height);
    
    gtk_widget_show_all(data->overlay);
}

void close_win32_overlay(AppData* data) {
    if (data->overlay) {
        gtk_widget_destroy(data->overlay);
        data->overlay = NULL;
    }
}
