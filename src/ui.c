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
	ui->space = 50;

	ui->show_devices = false;
	strcpy(ui->device_name, "N/A");

	ui->show_style = false;
	strcpy(ui->style, "CYBER");
}

void print_devices(AudioData *data, UserInterface *ui) {
	if(GuiButton((Rectangle){20, 20, 400, 50}, ui->device_name)) {
		ui->show_devices = !ui->show_devices;
	}

	if(ui->show_devices){
		int numDevices = Pa_GetDeviceCount();
		const PaDeviceInfo* deviceInfo;
		int x = 1;

		for (int i = 0; i < numDevices; i++) {
			deviceInfo = Pa_GetDeviceInfo(i);
			if(deviceInfo->maxInputChannels > 0 && deviceInfo->maxInputChannels < 8) {
				data->device = i;
				if(GuiButton((Rectangle){20, 60 * (x++) + 20, 400, 50}, deviceInfo->name)) {
					strcpy(ui->device_name, deviceInfo->name);
					ui->show_devices = !ui->show_devices;
					start_stream(data);
				}
			}
		}
	}
}

void refresh_button(AudioData *data, UserInterface *ui) {
	if(GuiButton((Rectangle){20, GetRenderHeight() - 70, 400, 50}, "Refresh")) {
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

void widget_settings(UserInterface *ui) {
	GuiGroupBox((Rectangle){480, 20, 500, 130}, " VISUALIZER SETTINGS");
	GuiSliderBar((Rectangle){ 500, 40, 380, 30}, "", TextFormat("SPACE: %i", (int)ui->space), &ui->space, 0, 1000);
	GuiSliderBar((Rectangle){ 500, 100, 380, 30}, "", TextFormat("RADIUS: %i", (int)ui->radius), &ui->radius, 0, 1000);
}

void set_style(UserInterface *ui) {
	int x = GetScreenWidth() - 420;
	if(GuiButton((Rectangle){x, 20, 400, 50}, ui->style)) {
		ui->show_style = !ui->show_style;
	}

	if(ui->show_style){
		if(GuiButton((Rectangle){x, 80, 400, 50}, "ASHES")) {
			ui->show_style = false;
			strcpy(ui->style, "ASHES");
			GuiLoadStyleAshes();
		}
		if(GuiButton((Rectangle){x, 140, 400, 50}, "BLUISH")) {
			ui->show_style = false;
			strcpy(ui->style, "BLUISH");
			GuiLoadStyleBluish();
		}
		if(GuiButton((Rectangle){x, 200, 400, 50}, "CANDY")) {
			ui->show_style = false;
			strcpy(ui->style, "CANDY");
			GuiLoadStyleCandy();
		}
		if(GuiButton((Rectangle){x, 260, 400, 50}, "CHERRY")) {
			ui->show_style = false;
			strcpy(ui->style, "CHERRY");
			GuiLoadStyleCherry();
		}
		if(GuiButton((Rectangle){x, 320, 400, 50}, "CYBER")) {
			ui->show_style = false;
			strcpy(ui->style, "CYBER");
			GuiLoadStyleCyber();
		}
		if(GuiButton((Rectangle){x, 380, 400, 50}, "DARK")) {
			ui->show_style = false;
			strcpy(ui->style, "DARK");
			GuiLoadStyleDark();
		}
		if(GuiButton((Rectangle){x, 440, 400, 50}, "DEFAULT")) {
			ui->show_style = false;
			strcpy(ui->style, "DEFAULT");
			GuiLoadStyleDefault();
		}
		if(GuiButton((Rectangle){x, 500, 400, 50}, "ENEFETE")) {
			ui->show_style = false;
			strcpy(ui->style, "ENEFETE");
			GuiLoadStyleEnefete();
		}
		if(GuiButton((Rectangle){x, 560, 400, 50}, "JUNGLE")) {
			ui->show_style = false;
			strcpy(ui->style, "JUNGLE");
			GuiLoadStyleJungle();
		}
		if(GuiButton((Rectangle){x, 620, 400, 50}, "LAVANDA")) {
			ui->show_style = false;
			strcpy(ui->style, "LAVANDA");
			GuiLoadStyleLavanda();
		}
		if(GuiButton((Rectangle){x, 680, 400, 50}, "SUNNY")) {
			ui->show_style = false;
			strcpy(ui->style, "SUNNY");
			GuiLoadStyleSunny();
		}
		if(GuiButton((Rectangle){x, 740, 400, 50}, "TERMINAL")) {
			ui->show_style = false;
			strcpy(ui->style, "TERMINAL");
			GuiLoadStyleTerminal();
		}
	}
}

void draw(AudioData *data, UserInterface *ui) {
	InitWindow(1500, 900, "visualizer");
	SetTargetFPS(60);

	GuiLoadStyleCyber();

	while(!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
		draw_widget(data, ui);
		print_devices(data, ui);
		refresh_button(data, ui);
		widget_settings(ui);
		set_style(ui);
		EndDrawing();
	}
}
