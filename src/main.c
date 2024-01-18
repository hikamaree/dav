#include "ui.h"

int main(int argc, char *argv[]) {
	AppData* data = malloc (sizeof(AppData));
	data->stream = malloc(sizeof(StreamData));
	gtk_init(&argc, &argv);
	create_window(data);
	gtk_main();
	Pa_Terminate();
	free(data->stream);
	free(data);
	return 0;
}
