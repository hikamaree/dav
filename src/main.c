#include "ui.h"
#include "visualizer.h"
#include <gio/gio.h>

#if defined(WIN32) || defined(_WIN32)
extern GResource *resources_get_resource(void);

static void setup_bundled_schemas(void) {
	GResource *resource = resources_get_resource();
	g_resources_register(resource);

	GBytes *schema_bytes = g_resource_lookup_data(resource, "/org/dav/schemas/schemas/gschemas.compiled", G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);

	if (schema_bytes) {
		const char *temp_dir = g_get_tmp_dir();
		char *schema_dir = g_build_filename(temp_dir, "dav-schemas", NULL);
		g_mkdir_with_parents(schema_dir, 0700);

		char *schema_file = g_build_filename(schema_dir, "gschemas.compiled", NULL);
		g_file_set_contents(schema_file, g_bytes_get_data(schema_bytes, NULL), g_bytes_get_size(schema_bytes), NULL);

		g_setenv("GSETTINGS_SCHEMA_DIR", schema_dir, TRUE);

		g_free(schema_dir);
		g_free(schema_file);
		g_bytes_unref(schema_bytes);
	}
}
#else

static void on_app_activate(GtkApplication *app, gpointer user_data) {
	AppData *data = user_data;

	if (!data->window) {
		data->window = gtk_application_window_new(app);
		create_window(data);
		open_overlay(data);
	} else {
		gtk_window_present(GTK_WINDOW(data->window));
	}
}
#endif

int main(int argc, char *argv[]) {
	AppData *data = calloc(1, sizeof(AppData));
	data->stream = calloc(1, sizeof(StreamData));
	data->settings = read_config();
	data->visualizer = false;
	data->gifs = NULL;

#if defined(WIN32) || defined(_WIN32)
	gtk_init(&argc, &argv);
	setup_bundled_schemas();
	create_window(data);
	open_overlay(data);
	gtk_main();
#else
	GtkApplication *app = gtk_application_new("org.dav.visualizer", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), data);
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	return status;
#endif

	return 0;
}

