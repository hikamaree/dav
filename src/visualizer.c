#include "visualizer.h"
#include "wayland.h"
#include "x11.h"

int device = -1;

void open_overlay(AppData* data) {
#if defined(WIN32) || defined(WIN64)
	return;
#elif defined(__linux__)
	if (getenv("WAYLAND_DISPLAY")) {
		open_wayland_overlay(data);
	} else {
		open_x11_overlay(data);
	}
#endif
}

void close_overlay(AppData* data) {
#if defined(WIN32) || defined(WIN64)
	return;
#elif defined(__linux__)
	if (getenv("WAYLAND_DISPLAY")) {
		close_wayland_overlay(data);
	} else {
		close_x11_overlay(data);
	}
#endif
}
