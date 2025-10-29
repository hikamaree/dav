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
#endif

int main(int argc, char* argv[]) {
	AppData* data = calloc(1, sizeof(AppData));
	data->stream = calloc(1, sizeof(StreamData));
	data->settings = read_config();
	data->visualizer = false;
	data->gifs = NULL;

	gtk_init(&argc, &argv);
#if defined(WIN32) || defined(_WIN32)
	setup_bundled_schemas();
#endif
	create_window(data);
	open_overlay(data);
	gtk_main();

	return 0;
}
