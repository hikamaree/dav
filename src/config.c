#include "config.h"
#include <stdlib.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_config_path() {
#ifdef _WIN32
	const char* appData = getenv("APPDATA");
	if (appData == NULL || strlen(appData) == 0) {
		fprintf(stderr, "Error: APPDATA environment variable not set.\n");
		exit(EXIT_FAILURE);
	}
	return strcat(strcpy(malloc(strlen(appData) + strlen("\\dav\\config") + 1), appData), "\\dav\\config");
#else
	const char* xdgConfigHome = getenv("XDG_CONFIG_HOME");
	if (xdgConfigHome == NULL || strlen(xdgConfigHome) == 0) {
		const char* homeDir = getenv("HOME");
		if (homeDir == NULL || strlen(homeDir) == 0) {
			fprintf(stderr, "Error: HOME environment variable not set.\n");
			exit(EXIT_FAILURE);
		}
		return strcat(strcpy(malloc(strlen(homeDir) + strlen("/.config/dav/config") + 1), homeDir), "/.config/dav/config");
	}
	return strcat(strcpy(malloc(strlen(xdgConfigHome) + strlen("/dav/config") + 1), xdgConfigHome), "/dav/config");
#endif
}

void write_config(const Config* config) {
	FILE* file = fopen(config->path, "w");
	if (file == NULL) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	fprintf(file, "# Configuration file for Directional Audio Visualizer\n\n");
	fprintf(file, "# Radius\n");
	fprintf(file, "radius = %d\n\n", config->radius);
	fprintf(file, "# Space\n");
	fprintf(file, "space = %d\n\n", config->space);
	fprintf(file, "# Speed\n");
	fprintf(file, "speed = %d\n\n", config->speed);
	fprintf(file, "# Color\n");
	fprintf(file, "red = %lf\n\n", config->red);
	fprintf(file, "green = %lf\n\n", config->green);
	fprintf(file, "blue = %lf\n\n", config->blue);
	fprintf(file, "alpha = %lf\n\n", config->alpha);

	fprintf(file, "# GIF Settings\n");
	fprintf(file, "gif_path = %s\n\n", config->gif_path);
	fprintf(file, "gif_x = %d\n\n", config->gif_x);
	fprintf(file, "gif_y = %d\n\n", config->gif_y);
	fprintf(file, "gif_width = %d\n\n", config->gif_width);
	fprintf(file, "gif_height = %d\n\n", config->gif_height);

	fclose(file);
}

Config* read_config() {
	Config* config = malloc(sizeof(Config));
	*config = (Config){
		.path = get_config_path(),
		.radius = -1,
		.space = -1,
		.speed = -1,
		.red = -1,
		.green = -1,
		.blue = -1,
		.alpha = -1,

		.gif_path = NULL,
		.gif_x = -1,
		.gif_y = -1,
		.gif_width = -1,
		.gif_height = -1,
	};

	Config default_config = {
		.path = NULL,
		.radius = 100,
		.space = 200,
		.speed = 300,
		.red = 0.0,
		.green = 1.0,
		.blue = 1.0,
		.alpha = 0.5,

		.gif_path = "animation.gif",
		.gif_x = 10,
		.gif_y = 10,
		.gif_width = 64,
		.gif_height = 64,
	};

	char* config_dir = strdup(config->path);
#ifdef _WIN32
	config_dir[strlen(config_dir) - strlen("\\config")] = '\0';
#else
	config_dir[strlen(config_dir) - strlen("/config")] = '\0';
#endif
	struct stat st = {0};
	if (stat(config_dir, &st) == -1) {
#ifdef _WIN32
		if (mkdir(config_dir) == -1) {
#else
		if (mkdir(config_dir, 0700) == -1) {
#endif
			perror("Error creating config directory");
			exit(EXIT_FAILURE);
		}
	}

	FILE* file = fopen(config->path, "r");

	if (file == NULL) {
		default_config.path = config->path;
		*config = default_config;
		config->gif_path = strdup(default_config.gif_path);
	}
	else {
		char buffer[1000];
		while (fgets(buffer, sizeof(buffer), file) != NULL) {
			char key[1000];
			double value;
			char string_value[1000];

			if (sscanf(buffer, "%s = %[^\n]", key, string_value) == 2) {
				if (strcmp(key, "gif_path") == 0) {
					config->gif_path = strdup(string_value);
				}
			}
			if (sscanf(buffer, "%s = %lf", key, &value) == 2) {
				if (strcmp(key, "radius") == 0) {
					config->radius = value;
				} else if (strcmp(key, "space") == 0) {
					config->space = (int)value;
				} else if (strcmp(key, "speed") == 0) {
					config->speed = value;
				} else if (strcmp(key, "red") == 0) {
					config->red = value;
				} else if (strcmp(key, "green") == 0) {
					config->green = value;
				} else if (strcmp(key, "blue") == 0) {
					config->blue = value;
				} else if (strcmp(key, "alpha") == 0) {
					config->alpha = value;
				} else if (strcmp(key, "gif_x") == 0) {
					config->gif_x = (int)value;
				} else if (strcmp(key, "gif_y") == 0) {
					config->gif_y = (int)value;
				} else if (strcmp(key, "gif_width") == 0) {
					config->gif_width = (int)value;
				} else if (strcmp(key, "gif_height") == 0) {
					config->gif_height = (int)value;
				}
			}
		}
		if(config->radius < 0) config->radius = default_config.radius;
		if(config->space < 0) config->space = default_config.space;
		if(config->speed < 0) config->speed = default_config.speed;
		if(config->red < 0) config->red = default_config.red;
		if(config->green < 0) config->green = default_config.green;
		if(config->blue < 0) config->blue = default_config.blue;
		if(config->alpha < 0) config->alpha = default_config.alpha;
		if(config->gif_x < 0) config->gif_x = default_config.gif_x;
		if(config->gif_y < 0) config->gif_y = default_config.gif_y;
		if(config->gif_width < 0) config->gif_width = default_config.gif_width;
		if(config->gif_height < 0) config->gif_height = default_config.gif_height;
		if(config->gif_path == NULL) config->gif_path = strdup(default_config.gif_path);

		fclose(file);
	}

	write_config(config);
	return config;
}
