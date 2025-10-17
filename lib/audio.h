#ifndef AUDIO_H
#define AUDIO_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__linux__)
#include <pthread.h>
#endif
#include <portaudio.h>

#define FRAMES_PER_BUFFER 256

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

typedef struct {
	PaStream* stream;
	int device;
	float speed;
	int channel_cnt;
	float* channels;
	float angle;
} StreamData;

void start_stream(StreamData*);
void refresh_stream(StreamData*);
void close_stream(StreamData*);

#endif
