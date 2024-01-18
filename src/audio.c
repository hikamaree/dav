#include "audio.h"

int patestCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
	StreamData* data = (StreamData*)userData;
	float* in = (float*)inputBuffer;

	for (int i = 0; i < data->channel_cnt; i++) {
		if (data->channels[i] > 0) {
			data->channels[i] -= data->speed / 100000;
		}
	}

	for (unsigned long i = 0; i < framesPerBuffer * data->channel_cnt; i += data->channel_cnt) {
		for (int j = 0; j < data->channel_cnt; j++) {
			data->channels[j] = MAX(data->channels[j], fabs(in[i + j]));
		}
	}

	return 0;
}

void start_stream(StreamData *data) {
	close_stream(data);
	if(data->device > Pa_GetDeviceCount()) return;

	PaStreamParameters inputParameters;
	memset(&inputParameters, 0, sizeof(inputParameters));
	inputParameters.channelCount = Pa_GetDeviceInfo(data->device)->maxInputChannels;
	inputParameters.device = data->device;
	inputParameters.hostApiSpecificStreamInfo = NULL;
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo(data->device)->defaultLowInputLatency;

	data->channel_cnt = inputParameters.channelCount;
	data->channels = (float*)calloc(data->channel_cnt, sizeof(float));

	Pa_OpenStream(&data->stream, &inputParameters, NULL, Pa_GetDeviceInfo(data->device)->defaultSampleRate, FRAMES_PER_BUFFER, paNoFlag, patestCallback, data);
	Pa_StartStream(data->stream);
}

void refresh_devices(StreamData *data) {
	close_stream(data);
	Pa_Terminate();
	Pa_Initialize();
}

void close_stream(StreamData *data) {
	if (data->stream != NULL) {
		Pa_StopStream(data->stream);
		Pa_CloseStream(data->stream);
		data->stream = NULL;
	}
	if (data->channels) {
		free(data->channels);
		data->channels = NULL;
	}
	data->channel_cnt = 0;
}
