#ifndef UI_H
#define UI_H

#include "audio.h"
#include <stdbool.h>
#include <string.h>

typedef struct ui{
	float radius;
	float space;

	bool show_devices;
	char device_name[100];

	bool show_style;
	char style[100];
} UserInterface;

void draw(AudioData *data, UserInterface *ui);
void setup_ui(UserInterface *ui);

#endif
