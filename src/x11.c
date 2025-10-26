#include "x11.h"
#include "visualizer.h"
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xfixes.h>

static gboolean keep_above_timer(gpointer user_data) {
    GtkWidget* widget = GTK_WIDGET(user_data);

    if (!widget || !gtk_widget_get_realized(widget)) {
        return FALSE;
    }

    GdkWindow* gdk_window = gtk_widget_get_window(widget);
    if (!gdk_window) {
        return FALSE;
    }

    Display* display = GDK_WINDOW_XDISPLAY(gdk_window);
    Window window = GDK_WINDOW_XID(gdk_window);

    XRaiseWindow(display, window);
    XFlush(display);

    return TRUE;
}

void open_x11_overlay(AppData* data) {
    data->overlay = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_widget_set_app_paintable(data->overlay, TRUE);

    GdkScreen* screen = gtk_widget_get_screen(data->overlay);
    GdkVisual* visual = gdk_screen_get_rgba_visual(screen);
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

    gtk_widget_realize(data->overlay);

    GdkWindow* gdk_window = gtk_widget_get_window(data->overlay);
    Display* display = GDK_WINDOW_XDISPLAY(gdk_window);
    Window window = GDK_WINDOW_XID(gdk_window);

    XSetWindowAttributes attrs;
    attrs.override_redirect = True;
    XChangeWindowAttributes(display, window, CWOverrideRedirect, &attrs);

    Atom atom_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom atom_above = XInternAtom(display, "_NET_WM_STATE_ABOVE", False);
    Atom atom_sticky = XInternAtom(display, "_NET_WM_STATE_STICKY", False);
    Atom atom_skip_taskbar = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False);
    Atom atom_skip_pager = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", False);

    Atom states[] = {atom_above, atom_sticky, atom_skip_taskbar, atom_skip_pager};
    XChangeProperty(display, window, atom_state, XA_ATOM, 32, PropModeReplace, (unsigned char*)states, 4);

    Atom atom_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom atom_dock = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", False);
    XChangeProperty(display, window, atom_type, XA_ATOM, 32, PropModeReplace, (unsigned char*)&atom_dock, 1);

    Atom atom_desktop = XInternAtom(display, "_NET_WM_DESKTOP", False);
    long desktop = 0xFFFFFFFF;
    XChangeProperty(display, window, atom_desktop, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&desktop, 1);

    XserverRegion region = XFixesCreateRegion(display, NULL, 0);
    XFixesSetWindowShapeRegion(display, window, ShapeInput, 0, 0, region);
    XFixesDestroyRegion(display, region);

    gtk_widget_show_all(data->overlay);

    XRaiseWindow(display, window);
    XFlush(display);

    g_timeout_add(100, keep_above_timer, data->overlay);
}

void close_x11_overlay(AppData* data) {
    if (data->overlay) {
        gtk_widget_destroy(data->overlay);
        data->overlay = NULL;
    }
}
