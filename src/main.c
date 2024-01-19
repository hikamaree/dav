#include "ui.h"

int main(int argc, char *argv[]) {
	AppData* data = malloc (sizeof(AppData));
	data->stream = malloc(sizeof(StreamData));
	data->settings = malloc(sizeof(Config));

	data->settings->path = get_config_path();
	g_print("%s", data->settings->path);
	read_config(data->settings);

	gtk_init(&argc, &argv);
	create_window(data);
	gtk_main();
	Pa_Terminate();
	free(data->stream);
	free(data);
	return 0;
}
