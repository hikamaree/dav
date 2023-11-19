#include "audio.h"

int print_devices() {
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

    int i = 0;
    while (i < 1 || i > numDevices) {
        printf("Enter device number [1, %d]\n", numDevices);
        scanf("%d", &i);
    }
    return i - 1;
}

int patestCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {

    AudioData *data = (AudioData*)userData;
    float* in = (float*)inputBuffer;

    for(int i = 0; i < data->chanel_cnt; i++) {
        if (data->chanels[i] > 0) {
            data->chanels[i] -= 0.01;
        }
    }
    //ide left right front back
    //a meni treba redosled right front left back
    //ovo radi kad je paran broj chanela
    for (unsigned long i = 0; i < framesPerBuffer * data->chanel_cnt; i += data->chanel_cnt) {
        for(int j = 0; j < data->chanel_cnt / 2; j++) {
            data -> chanels[j] = MAX(data -> chanels[j], fabs(in[2 * j + 1]));
            data -> chanels[j + data->chanel_cnt / 2] = MAX(data -> chanels[j + data->chanel_cnt / 2], fabs(in[2 * j]));
        }
    }

    return 0;
}

void start_audio_server(PaStream *stream, AudioData *data) {
    Pa_Initialize();
    int device = print_devices();

    PaStreamParameters inputParameters;
    memset(&inputParameters, 0, sizeof(inputParameters));
    inputParameters.channelCount = Pa_GetDeviceInfo(device)->maxInputChannels;
    inputParameters.device = device;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;
    data->chanel_cnt = inputParameters.channelCount;
    data->chanels = calloc(data->chanel_cnt, sizeof(float));

    Pa_OpenStream(&stream, &inputParameters, NULL, Pa_GetDeviceInfo(device)->defaultSampleRate, FRAMES_PER_BUFFER, paNoFlag, patestCallback, data);
    Pa_StartStream(stream);
}

void close_audio_server(PaStream *stream) {
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
}
