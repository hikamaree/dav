#include "audio.h"
#include <stdio.h>

// int patestCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
// 	StreamData* data = (StreamData*)userData;
// 	float* in = (float*)inputBuffer;
//
// 	printf("in buffer:\n");
//
// 	for (int i = 0; i < data->channel_cnt; i++) {
// 		if (data->channels[i] > 0) {
// 			data->channels[i] -= data->speed / 100000;
// 		}
// 	}
//
// 	for (unsigned long i = 0; i < framesPerBuffer * data->channel_cnt; i += data->channel_cnt) {
// 		for (int j = 0; j < data->channel_cnt; j++) {
// 			data->channels[j] = MAX(data->channels[j], fabs(in[i + j]));
// 			printf("%f, ", in[i + j]);
// 		}
// 	}
// 	printf("\n\n");
//
// 	return 0;
// }

int patestCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    StreamData* data = (StreamData*)userData;
    float* in = (float*)inputBuffer;

    float left_level = 0.0f, right_level = 0.0f;
    float left_peak = 0.0f, right_peak = 0.0f;
    float left_high_freq = 0.0f, right_high_freq = 0.0f;
    float left_low_freq = 0.0f, right_low_freq = 0.0f;
    
    for (unsigned long i = 0; i < framesPerBuffer * data->channel_cnt; i += data->channel_cnt) {
        float left_sample = in[i];
        float right_sample = in[i + 1];
        
        float left_abs = fabsf(left_sample);
        float right_abs = fabsf(right_sample);
        
        left_level += left_abs;
        right_level += right_abs;
        
        if (left_abs > left_peak) left_peak = left_abs;
        if (right_abs > right_peak) right_peak = right_abs;
        
        float left_high = left_abs * (left_abs > 0.08f ? 2.0f : 0.3f);
        float right_high = right_abs * (right_abs > 0.08f ? 2.0f : 0.3f);
        float left_low = left_abs * 0.4f;
        float right_low = right_abs * 0.4f;
        
        left_high_freq += left_high;
        right_high_freq += right_high;
        left_low_freq += left_low;
        right_low_freq += right_low;
    }
    
    left_level /= framesPerBuffer;
    right_level /= framesPerBuffer;
    left_high_freq /= framesPerBuffer;
    right_high_freq /= framesPerBuffer;
    left_low_freq /= framesPerBuffer;
    right_low_freq /= framesPerBuffer;
    
    float left_combined = (left_level * 0.7f) + (left_peak * 0.3f);
    float right_combined = (right_level * 0.7f) + (right_peak * 0.3f);
    
    float total_energy = (left_combined + right_combined) / 2.0f;
    
    if (total_energy < 0.001f) {
        data->angle = data->angle * 0.95f + 90.0f * 0.05f;
    } else {
        float balance = (right_combined - left_combined) / (left_combined + right_combined + 0.0001f);
        
        float sign = balance > 0 ? 1.0f : -1.0f;
        float abs_balance = fabsf(balance);
        
        float transformed_balance;
        if (abs_balance < 0.3f) {
            transformed_balance = abs_balance * 0.5f;
        } else {
            transformed_balance = 0.15f + (abs_balance - 0.3f) * 1.7f;
        }
        
        float lateral_angle = sign * fminf(transformed_balance, 1.0f) * 90.0f;
        
        float high_freq_total = (left_high_freq + right_high_freq) / 2.0f;
        float low_freq_total = (left_low_freq + right_low_freq) / 2.0f;
        float front_back_ratio = (high_freq_total - low_freq_total) / (high_freq_total + low_freq_total + 0.0001f);
        
        float front_back_weight = fminf(fabsf(front_back_ratio) * 3.0f, 1.0f);
        
        float target_angle;
        if (front_back_ratio > 0.1f) {
            target_angle = 90.0f + lateral_angle;
        } else if (front_back_ratio < -0.1f) {
            target_angle = 270.0f - lateral_angle;
        } else {
            target_angle = 90.0f + lateral_angle;
        }
        
        float angle_diff = fabsf(target_angle - data->angle);
        if (angle_diff > 180.0f) angle_diff = 360.0f - angle_diff;
        
        float smoothing = 0.05f + (angle_diff / 180.0f) * 0.15f;
        smoothing = fminf(smoothing, 0.2f);
        
        float current_angle = data->angle;
        float diff = target_angle - current_angle;
        
        if (diff > 180.0f) diff -= 360.0f;
        if (diff < -180.0f) diff += 360.0f;
        
        data->angle = current_angle + diff * smoothing;
        
        if (data->angle >= 360.0f) data->angle -= 360.0f;
        if (data->angle < 0.0f) data->angle += 360.0f;
    }

    static int counter = 0;
    if (counter++ % 30 == 0) {
        const char* sector = "PRED";
        if (data->angle > 180.0f && data->angle <= 360.0f) sector = "ZAD";
        
        printf("left: %.4f, right: %.4f, angle: %.1f (%s)\n", left_level, right_level, data->angle, sector);
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
