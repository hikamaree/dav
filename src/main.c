#include "ui.h"

int main(int argc, char *argv[]) {
	AppData* data = malloc (sizeof(AppData));
	data->stream = malloc(sizeof(StreamData));
	data->settings = read_config();

	gtk_init(&argc, &argv);
	create_window(data);
	gtk_main();

	return 0;
}
