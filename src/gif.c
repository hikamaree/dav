#include "gif.h"
#include "data.h"

#include "ui.h"

Gif* gif_new(const char* path) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	Gif* g = malloc(sizeof(Gif));
	if (!g) return NULL;

	g->path = strdup(path);
	if (!g->path) {
		free(g);
		return NULL;
	}

	g->gif_anim = gdk_pixbuf_animation_new_from_file(path, NULL);
	if (!g->gif_anim) {
		free(g->path);
		free(g);
		return NULL;
	}

	g->gif_iter = gdk_pixbuf_animation_get_iter(g->gif_anim, NULL);
	g->gif_x = 0;
	g->gif_y = 0;
	g->gif_width = gdk_pixbuf_animation_get_width(g->gif_anim);
	g->gif_height = gdk_pixbuf_animation_get_height(g->gif_anim);
	return g;
#pragma GCC diagnostic pop
}

void gif_free(Gif* g) {
	if (!g) return;
	if (g->gif_anim) g_object_unref(g->gif_anim);
	free(g->path);
	free(g);
}

void save_gifs(const char* path, AppData* data) {
	if (!path || !data || !data->gifs) return;

	FILE* f = fopen(path, "w");
	if (!f) return;

	for (GList* l = data->gifs; l; l = l->next) {
		Gif* g = (Gif*)l->data;
		if (!g || !g->path) continue;
		fprintf(f, "%s %d %d %d %d\n", g->path, g->gif_x, g->gif_y, g->gif_width, g->gif_height);
	}

	fclose(f);
}

void load_gifs(const char* path, AppData* data) {
	if (!path || !data) return;

	FILE* f = fopen(path, "r");
	if (!f) return;

	char gif_path[1024];
	int x, y, w, h;

	while (fscanf(f, "%1023s %d %d %d %d", gif_path, &x, &y, &w, &h) == 5) {
		Gif* g = gif_new(gif_path);
		if (!g) continue;
		g->gif_x = x;
		g->gif_y = y;
		g->gif_width = w;
		g->gif_height = h;
		data->gifs = g_list_append(data->gifs, g);
		open_gif_tab(data, g);
	}

	fclose(f);
}
