#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
	char* path;
	int radius;
	int space;
	int speed;
	double red;
	double green;
	double blue;
	double alpha;

    char* gif_path;
    int gif_x;
    int gif_y;
    int gif_width;
    int gif_height;
} Config;

Config* read_config();
void write_config(const Config *config);

#endif
