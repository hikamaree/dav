#include "ui.h"

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

#include "../styles/ashes/style_ashes.h"
#include "../styles/bluish/style_bluish.h"
#include "../styles/candy/style_candy.h"
#include "../styles/cherry/style_cherry.h"
#include "../styles/cyber/style_cyber.h"
#include "../styles/dark/style_dark.h"
#include "../styles/enefete/style_enefete.h"
#include "../styles/jungle/style_jungle.h"
#include "../styles/lavanda/style_lavanda.h"
#include "../styles/sunny/style_sunny.h"
#include "../styles/terminal/style_terminal.h"

void setup_ui(UserInterface *ui) {
	ui->radius = 200;
	ui->space = 200;

	ui->show_devices = false;
	strcpy(ui->device_name, "N/A");

	strcpy(ui->style_name, "CYBER");
	ui->style = 4;

	ui->show_ui = true;
}

void print_devices(AudioData *data, UserInterface *ui) {
	if(GuiButton((Rectangle){20, 20, 300, 50}, ui->device_name)) {
		ui->show_devices = !ui->show_devices;
	}

	if(ui->show_devices){
		int numDevices = Pa_GetDeviceCount();
		const PaDeviceInfo* deviceInfo;
		int x = 1;

		char *name;
		for (int i = 0; i < numDevices; i++) {
			deviceInfo = Pa_GetDeviceInfo(i);
			if(deviceInfo->maxInputChannels > 0 && deviceInfo->maxInputChannels < 8) {
				data->device = i;
				name = get_name((char*)deviceInfo->name, 36, 3);
				if(GuiButton((Rectangle){20, 60 * (x++) + 20, 300, 50}, name)) {
					strcpy(ui->device_name, name);
					ui->show_devices = !ui->show_devices;
					start_stream(data);
				}
				free(name);
			}
		}
	}
}

void refresh_button(AudioData *data, UserInterface *ui) {
	if(GuiButton((Rectangle){20, GetRenderHeight() - 70, 200, 50}, "Refresh")) {
		strcpy(ui->device_name, "N/A");
		ui->show_devices = false;
		refresh_devices(data);
	}
}

void draw_widget(AudioData *data, UserInterface *ui) {
	float delta = 2 * PI / data->channel_cnt;
	float width = GetRenderWidth();;
	float height = GetRenderHeight();;
	float x, y;
	Color color = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
	for (int i = 0; i < data->channel_cnt; i += 2) {
		x = (ui->space) * cos(i * delta / 2);
		y = (ui->space) * sin(i * delta / 2);

		DrawCircle(width / 2 - x, height / 2 - y, data->channels[i] * ui->radius, color);
		DrawCircle(width / 2 + x, height / 2 + y, data->channels[i + 1] * ui->radius, color);
	}
}

void widget_settings(AudioData *data, UserInterface *ui) {
	GuiGroupBox((Rectangle){340, 20, 400, 190}, " VISUALIZER SETTINGS");
	GuiSliderBar((Rectangle){400, 40, 280, 30},  "SPACE ", TextFormat("%i", (int)ui->space), &ui->space, 0, 1000);
	GuiSliderBar((Rectangle){400, 100, 280, 30}, "RADIUS", TextFormat("%i", (int)ui->radius), &ui->radius, 0, 1000);
	GuiSliderBar((Rectangle){400, 160, 280, 30}, "SPEED ", TextFormat("%i", (int)data->speed), &data->speed, 0, 1000);
}

void set_style(UserInterface *ui) {
	int x = GetScreenWidth() - 220;
	if(GuiButton((Rectangle){x, 20, 200, 50}, ui->style_name)) {
		ui->style = (ui->style + 1) % 12;
		switch(ui->style) {
			case 0:
				strcpy(ui->style_name, "ASHES");
				GuiLoadStyleAshes();
				break;
			case 1:
				strcpy(ui->style_name, "BLUISH");
				GuiLoadStyleBluish();
				break;
			case 2:
				strcpy(ui->style_name, "CANDY");
				GuiLoadStyleCandy();
				break;
			case 3:
				strcpy(ui->style_name, "CHERRY");
				GuiLoadStyleCherry();
				break;
			case 4:
				strcpy(ui->style_name, "CYBER");
				GuiLoadStyleCyber();
				break;
			case 5:
				strcpy(ui->style_name, "DARK");
				GuiLoadStyleDark();
				break;
			case 6:
				strcpy(ui->style_name, "DEFAULT");
				GuiLoadStyleDefault();
				break;
			case 7:
				strcpy(ui->style_name, "ENEFETE");
				GuiLoadStyleEnefete();
				break;
			case 8:
				strcpy(ui->style_name, "JUNGLE");
				GuiLoadStyleJungle();
				break;
			case 9:
				strcpy(ui->style_name, "LAVANDA");
				GuiLoadStyleLavanda();
				break;
			case 10:
				strcpy(ui->style_name, "SUNNY");
				GuiLoadStyleSunny();
				break;
			case 11:
				strcpy(ui->style_name, "TERMINAL");
				GuiLoadStyleTerminal();
				break;
		}
	}
}

void hide_ui(UserInterface *ui) {
	if(GuiButton((Rectangle){GetRenderWidth() - 220, GetRenderHeight() - 70, 200, 50}, ui->show_ui ? "HIDE UI" : "SHOW UI")) {
		//ToggleFullscreen(); //dwm zna da se ukenja
		ui->show_ui = !ui->show_ui;
	}
}

void draw(AudioData *data, UserInterface *ui) {
	InitWindow(1000, 900, "visualizer");
	SetTargetFPS(360);

	GuiLoadStyleCyber();
	//SetWindowOpacity(0.5); //bas sve bude transparentno

	setup_ui(ui);
	data->speed = 400;

	while(!WindowShouldClose()) {
		BeginDrawing();
		if(ui->show_ui) {
			ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		}
		else {
			ClearBackground((Color){0, 1, 0, 0});
		}
		draw_widget(data, ui);
		hide_ui(ui);
		if(ui->show_ui) {
			set_style(ui);
			refresh_button(data, ui);
			print_devices(data, ui);
			widget_settings(data, ui);
		}
		EndDrawing();
	}
}
