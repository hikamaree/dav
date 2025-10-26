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
} Config;

Config* read_config();
void write_config(const Config *config);

#endif
