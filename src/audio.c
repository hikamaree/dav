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

    if (data->left > 0) {
	    data->left -= 0.005;
    }

    if (data->right > 0) {
	    data->right -= 0.005;
    }

    for (unsigned long i = 0; i < framesPerBuffer * 2; i += 2) {
        data -> left = MAX(data -> left, fabs(in[i]));
        data -> right = MAX(data -> right, fabs(in[i + 1]));
    }

    return 0;
}

void start_audio_server(PaStream *stream, AudioData *data) {
    Pa_Initialize();
    int device = print_devices();

    PaStreamParameters inputParameters;
    memset(&inputParameters, 0, sizeof(inputParameters));
    inputParameters.channelCount = 2;
    inputParameters.device = device;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowInputLatency;

    Pa_OpenStream(&stream, &inputParameters, NULL, Pa_GetDeviceInfo(device)->defaultSampleRate, FRAMES_PER_BUFFER, paNoFlag, patestCallback, data);
    Pa_StartStream(stream);
}

void close_audio_server(PaStream *stream) {
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
}
