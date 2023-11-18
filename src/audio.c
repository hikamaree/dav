#include "audio.h"

void print_devices() {
    int numDevices = Pa_GetDeviceCount();
    printf("Number of devices: %d\n", numDevices);

    if (numDevices < 0) {
        printf("Error getting device count.\n");
        exit(EXIT_FAILURE);
    } else if (numDevices == 0) {
        printf("There are no available audio devices on this machine.\n");
        exit(EXIT_SUCCESS);
    }
    const PaDeviceInfo* deviceInfo;
    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("Device %d:\n", i + 1);
        printf("  name: %s\n", deviceInfo->name);
        printf("  maxInputChannels: %d\n", deviceInfo->maxInputChannels);
        printf("  maxOutputChannels: %d\n", deviceInfo->maxOutputChannels);
        printf("  defaultSampleRate: %f\n", deviceInfo->defaultSampleRate);
    }
}

int patestCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {

    AudioData *data = (AudioData*)userData;
    float* in = (float*)inputBuffer;

    for(int i = 0; i < data->chanel_cnt; i++) {
        if (data->chanels[i] > 0) {
            data->chanels[i] -= 0.005;
        }
    }

    for (unsigned long i = 0; i < framesPerBuffer * data->chanel_cnt; i += data->chanel_cnt) {
        for(int j = 0; j < data->chanel_cnt; j++) {
            data -> chanels[j] = MAX(data -> chanels[j], fabs(in[i + j]));
        }
    }
    return 0;
}

void start_stream(AudioData *data) {
    close_stream(data);
    Pa_Initialize();
    print_devices();

    PaStreamParameters inputParameters;
    memset(&inputParameters, 0, sizeof(inputParameters));
    inputParameters.channelCount = Pa_GetDeviceInfo(data->device)->maxInputChannels;
    inputParameters.device = data->device;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(data->device)->defaultLowInputLatency;

    data->chanel_cnt = inputParameters.channelCount;
    data->chanels = calloc(data->chanel_cnt, sizeof(float));

    Pa_OpenStream(&data->stream, &inputParameters, NULL, Pa_GetDeviceInfo(data->device)->defaultSampleRate, FRAMES_PER_BUFFER, paNoFlag, patestCallback, data);
    Pa_StartStream(data->stream);
}

void close_stream(AudioData *data) {
    if (data->stream != NULL) {
        Pa_StopStream(data->stream);
        Pa_CloseStream(data->stream);
        Pa_Terminate();
        data->stream = NULL;
    }

    if(data->chanels) {
        free(data->chanels);
    }
}
