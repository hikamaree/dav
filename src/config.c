#include "config.h"
#include <sys/stat.h>

char* get_config_path() {
    const char *xdgConfigHome = getenv("XDG_CONFIG_HOME");
    if (xdgConfigHome == NULL || strlen(xdgConfigHome) == 0) {
        const char *homeDir = getenv("HOME");
        if (homeDir == NULL || strlen(homeDir) == 0) {
            fprintf(stderr, "Error: HOME environment variable not set.\n");
            exit(EXIT_FAILURE);
        }
        return strcat(strcpy(malloc(strlen(homeDir) + strlen("/.config/dav/config") + 1), homeDir), "/.config/dav/config");
    }
    return strcat(strcpy(malloc(strlen(xdgConfigHome) + strlen("/dav/config") + 1), xdgConfigHome), "/dav/config");
}

void write_config(const Config *config) {
	FILE *file = fopen(config->path, "w");
	if (file == NULL) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	fprintf(file, "# Configuration file for Directional Audio Visualizer\n");
	fprintf(file, "\n");
	fprintf(file, "# Radius\n");
	fprintf(file, "radius = %d\n", config->radius);
	fprintf(file, "\n");
	fprintf(file, "# Space\n");
	fprintf(file, "space = %d\n", config->space);
	fprintf(file, "\n");
	fprintf(file, "# Speed\n");
	fprintf(file, "speed = %d\n", config->speed);

	fclose(file);
}

void read_config(Config *config) {
	char* config_dir = strdup(config->path);
	config_dir[strlen(config_dir) - strlen("/config")] = '\0';
	struct stat st = {0};
	if (stat(config_dir, &st) == -1) {
		if (mkdir(config_dir, 0700) == -1) {
			perror("Error creating config directory");
			exit(EXIT_FAILURE);
		}
	}
	FILE *file = fopen(config->path, "r");
	if (file == NULL) {
		config->radius = 100;
		config->space = 200;
		config->speed = 300;
		write_config(config);
	}
	else {
		char buffer[1000];
		while (fgets(buffer, sizeof(buffer), file) != NULL) {
			char key[1000];
			int value;
			if (sscanf(buffer, "%s = %d", key, &value) == 2) {
				if (strcmp(key, "radius") == 0) {
					config->radius = value;
				} else if (strcmp(key, "space") == 0) {
					config->space = (int)value;
				} else if (strcmp(key, "speed") == 0) {
					config->speed = value;
				}
			}
		}
		fclose(file);
	}
}
