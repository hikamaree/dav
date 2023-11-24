#include "ui.h"

int main() {
	AudioData data;
	UserInterface ui;

	Pa_Initialize();

	draw(&data, &ui);

	close_stream(&data);
	Pa_Terminate();
	return 0;
}
