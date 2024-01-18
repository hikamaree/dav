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
	PaStream *stream;
	int device;
	float speed;
	int channel_cnt;
	float *channels;
} StreamData;

void start_stream(StreamData *data);
void refresh_devices(StreamData *data);
void close_stream(StreamData *data);

#endif
