#include "audio.h"
#include <raylib.h>

int main() {
    PaStream* stream;
    AudioData data;
    start_audio_server(stream, &data);

    float window_width = 900;
    float window_height = 900;
    float radius = 200;
    float space = 50;
    float delta, x, y;

    InitWindow(window_width, window_height, "visualizer");
    SetTargetFPS(360);

    while(!WindowShouldClose()) {
        delta = 2 * PI / data.chanel_cnt;

        BeginDrawing();
        ClearBackground(BLANK);
        DrawCircleLines(window_width / 2, window_height / 2, space, WHITE);

        for(int i = 0; i < data.chanel_cnt; i += 2) {
            x = (radius + space) * cos(i * delta / 2);
            y = (radius + space) * sin(i * delta / 2);

            DrawCircle(window_width / 2 - x, window_height / 2 - y, data.chanels[i] * radius, RED);
            DrawCircle(window_width / 2 + x, window_height / 2 + y, data.chanels[i + 1] * radius, RED);
        }
        EndDrawing();
    }

    free(data.chanels);
    close_audio_server(stream);
    return 0;
}
