#ifndef UI_H
#define UI_H

#include "audio.h"
#include "util.h"
#include <stdbool.h>
#include <string.h>

typedef struct ui{
	float radius;
	float space;

	bool show_devices;
	char device_name[100];

	char style_name[10];
	int style;

	bool show_ui;
} UserInterface;

void draw(AudioData *data, UserInterface *ui);

#endif
