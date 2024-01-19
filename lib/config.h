#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	const char *path;
    int radius;
    int space;
    int speed;
} Config;

char* get_config_path();
void read_config(Config *config);
void write_config(const Config *config);
