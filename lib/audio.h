#ifndef AUDIO_H
#define AUDIO_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <portaudio.h>

#define FRAMES_PER_BUFFER 256

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

typedef struct {
    int device;
    PaStream *stream;
    int chanel_cnt;
    float *chanels;
} AudioData;

void start_stream(AudioData *data);
void close_stream(AudioData *data);

#endif
