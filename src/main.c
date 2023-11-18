#include "ui.h"

int main(int argc, char **argv) {
    AudioData data;

    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.dav", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), &data);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    close_stream(&data);
    return status;
}
