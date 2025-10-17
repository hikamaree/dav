#include "audio.h"

int patestCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    StreamData* data = (StreamData*)userData;
    float* in = (float*)inputBuffer;

    float left_level = 0.0f, right_level = 0.0f;
    float left_peak = 0.0f, right_peak = 0.0f;
    
    for (unsigned long i = 0; i < framesPerBuffer * data->channel_cnt; i += data->channel_cnt) {
        float left_sample = fabsf(in[i]);
        float right_sample = fabsf(in[i + 1]);
        
        left_level += left_sample;
        right_level += right_sample;
        
        if (left_sample > left_peak) left_peak = left_sample;
        if (right_sample > right_peak) right_peak = right_sample;
    }
    
    left_level /= framesPerBuffer;
    right_level /= framesPerBuffer;
    
    float left_combined = (left_level * 0.4f) + (left_peak * 0.6f);
    float right_combined = (right_level * 0.4f) + (right_peak * 0.6f);
    
    float total_energy = (left_combined + right_combined) / 2.0f;
    
    if (total_energy < 0.001f) {
        data->angle = 90.0f;
    } else {
        float balance = (right_combined - left_combined) / (left_combined + right_combined + 0.0001f);
        
        float sign = balance > 0 ? 1.0f : -1.0f;
        float abs_balance = fabsf(balance);
        
        float amplified_balance = sign * (abs_balance * abs_balance * 1.5f);
        amplified_balance = fmaxf(fminf(amplified_balance, 1.0f), -1.0f);
        
        float target_angle = 90.0f + (amplified_balance * 90.0f);
        
        target_angle = fmaxf(fminf(target_angle, 180.0f), 0.0f);
        
        float smoothing = data->speed / 10000;
        data->angle = data->angle * (1.0f - smoothing) + target_angle * smoothing;
    }

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

void refresh_stream(StreamData *data) {
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
