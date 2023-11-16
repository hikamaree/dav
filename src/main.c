#include "audio.h"
#include <raylib.h>

int main() {
    PaStream* stream;
    AudioData data;
    start_audio_server(stream, &data);

    float window_width = 1200;
    float window_height = 900;
    float radius = 200;
    float space = 100;
    float delta, x, y;

    InitWindow(window_width, window_height, "visualizer");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        delta = 2 * PI / data.chanel_cnt;

        BeginDrawing();
        ClearBackground(BLANK);
        //DrawCircleLines(window_width / 2, window_height / 2, space, WHITE);
        //stampam ih u matematicki pozitivnom smeru od desnog
        for(int i = 0; i < data.chanel_cnt; i++) {
            x = (radius + space) * cos(i * delta) + window_width / 2;
            y = (radius + space) * sin(i * delta) + window_height / 2;
            DrawCircle(x, y, data.chanels[i] * radius, RED);
        }
        EndDrawing();
    }

    free(data.chanels);
    close_audio_server(stream);
    return 0;
}
