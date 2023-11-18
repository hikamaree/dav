// citanje narednog koda u cilju razumevanja istog nije preporucljivo

#include "ui.h"

typedef struct {
    AudioData *data;
    int device;
} ButtonData;

typedef struct {
    AudioData *data;
    GtkWidget *window;
} RefreshData;

static void draw_cb(GtkDrawingArea *drawing_area, cairo_t *cr, int width, int height, gpointer user_data) {
    AudioData *data = (AudioData*)user_data;

    float window_width = 900;
    float window_height = 500;
    float radius = 200;
    float space = 50;
    float delta = 2 * M_PI / data->chanel_cnt;
    float x, y;

    cairo_paint(cr);
    cairo_set_source_rgba(cr, 1, 0, 0, 1);

    for(int i = 0; i < data->chanel_cnt; i += 2) {
        x = (radius + space) * cos(i * delta / 2);
        y = (radius + space) * sin(i * delta / 2);

        cairo_arc(cr, window_width / 2 - x, window_height / 2 - y, data->chanels[i] * radius, 0, 2 * M_PI);
        cairo_arc(cr, window_width / 2 + x, window_height / 2 + y, data->chanels[i] * radius, 0, 2 * M_PI);
    }
    cairo_fill(cr);
    gtk_widget_queue_draw((GtkWidget*)drawing_area);
}

static void select_device(GtkWidget *widget, ButtonData *button) {
    button->data->device = button->device;
    start_stream(button->data);
}

static void refresh(RefreshData *d) {
    Pa_Terminate();
    Pa_Initialize();
    GtkWidget *button;
    int numDevices = Pa_GetDeviceCount();

    if (numDevices < 0) {
        printf("Error getting device count.\n");
        exit(EXIT_FAILURE);
    } else if (numDevices == 0) {
        printf("There are no available audio devices on this machine.\n");
        exit(EXIT_SUCCESS);
    }

    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        ButtonData *b = (ButtonData*) malloc(sizeof(ButtonData));
        b->device = i + 1;
        b->data = d->data;
        button = gtk_button_new_with_label(deviceInfo->name);
        g_signal_connect(button, "clicked", G_CALLBACK(select_device), b);
        gtk_grid_attach(GTK_GRID(d->window), button, 0, numDevices - i - 1, 1, 1);
    }

    gtk_widget_queue_draw(d->window);
}

void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *drawing_area;
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;

    AudioData *data = (AudioData*)user_data;

    // init window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Directional Audio Visualizer");

    // init grid
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 900, 500);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), draw_cb, data, NULL);
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 1, 2, 100, 100);

    //refresh devices
    RefreshData *d = (RefreshData*) malloc(sizeof(RefreshData));
    d->data = data;
    d->window = grid;
    refresh(d);

    // init button for refresh
    button = gtk_button_new_with_label("Refresh");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(refresh), d);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);

    // init button for quit
    button = gtk_button_new_with_label("Quit");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), window);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);

    // set window
    gtk_window_present(GTK_WINDOW(window));
}
