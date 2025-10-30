#include <gtk/gtk.h>
#include "data.h"

typedef struct {
	GdkPixbufAnimation* gif_anim;
	GdkPixbufAnimationIter* gif_iter;

	char* path;

	int gif_x;
	int gif_y;
	int gif_width;
	int gif_height;
} Gif;

Gif* gif_new(const char* path);
void gif_free(Gif* g);

void load_gifs(const char* path, AppData* data);
void save_gifs(const char* path, AppData* data);
