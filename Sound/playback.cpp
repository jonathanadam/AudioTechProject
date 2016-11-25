

#include <stdio.h>
#include <string.h>
#include <portaudio.h>
#include <string>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>


PaStream* stream;
FILE* wavfile;
int numChannels;
int sampleRate;
PaSampleFormat sampleFormat;
int bytesPerSample, bitsPerSample;

int paStreamCallback(const void *input,
                     void *output,
                     unsigned long frameCount,
                     const PaStreamCallbackTimeInfo* timeInfo,
                     PaStreamCallbackFlags statusFlags,
                     void *userData)
{
    size_t numRead = fread(output, bytesPerSample * numChannels, frameCount, wavfile);
    output = (uint8_t*)output + numRead * numChannels * bytesPerSample;
    frameCount -= numRead;
    
    if(frameCount > 0) {
        memset(output, 0, frameCount * numChannels * bytesPerSample);
        return paComplete;
    }
    
    return paContinue;
}

bool portAudioOpen() {
    assert(Pa_Initialize() == paNoError);
    
    PaStreamParameters outputParameters;
    
    outputParameters.device = Pa_GetDefaultOutputDevice();
    assert(outputParameters.device != paNoDevice);
    
    outputParameters.channelCount = numChannels;
    outputParameters.sampleFormat = sampleFormat;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    
    PaError ret = Pa_OpenStream(
                                &stream,
                                NULL, // no input
                                &outputParameters,
                                sampleRate,
                                paFramesPerBufferUnspecified, // framesPerBuffer
                                0, // flags
                                &paStreamCallback,
                                NULL //void *userData
                                );
    
    if(ret != paNoError) {
        fprintf(stderr, "Pa_OpenStream failed: (err %i) %s\n", ret, Pa_GetErrorText(ret));
        if(stream)
            Pa_CloseStream(stream);
        return false;
    }
    
    assert(Pa_StartStream(stream) == paNoError);
    return true;
}

std::string freadStr(FILE* f, size_t len) {
    std::cout<<"now in freadstr line coming up"<<"\n";
    std::string s(len, '\0');
    fread(&s[0], 1, len, f);
    std::cout << s <<"\n";
//    assert(fread(&s[0], 1, len, f) == len);
    return s;
}

template<typename T>
T freadNum(FILE* f) {
    T value;
    assert(fread(&value, sizeof(value), 1, f) == 1);
    return value; // no endian-swap for now... WAV is LE anyway...
}

void readFmtChunk(uint32_t chunkLen) {
    assert(chunkLen >= 16);
    uint16_t fmttag = freadNum<uint16_t>(wavfile); // 1: PCM (int). 3: IEEE float
    assert(fmttag == 1 || fmttag == 3);
    numChannels = freadNum<uint16_t>(wavfile);
    assert(numChannels > 0);
    printf("%i channels\n", numChannels);
    sampleRate = freadNum<uint32_t>(wavfile);
    printf("%i Hz\n", sampleRate);
    uint32_t byteRate = freadNum<uint32_t>(wavfile);
    uint16_t blockAlign = freadNum<uint16_t>(wavfile);
    bitsPerSample = freadNum<uint16_t>(wavfile);
    bytesPerSample = bitsPerSample / 8;
    assert(byteRate == sampleRate * numChannels * bytesPerSample);
    assert(blockAlign == numChannels * bytesPerSample);
    if(fmttag == 1 /*PCM*/) {
        switch(bitsPerSample) {
            case 8: sampleFormat = paInt8; break;
            case 16: sampleFormat = paInt16; break;
            case 32: sampleFormat = paInt32; break;
            default: assert(false);
        }
        printf("PCM %ibit int\n", bitsPerSample);
    } else {
        assert(fmttag == 3 /* IEEE float */);
        assert(bitsPerSample == 32);
        sampleFormat = paFloat32;
        printf("32bit float\n");
    }
    if(chunkLen > 16) {
        uint16_t extendedSize = freadNum<uint16_t>(wavfile);
        assert(chunkLen == 18 + extendedSize);
        fseek(wavfile, extendedSize, SEEK_CUR);
    }
}

int main_function(int argc, char** argv) {
//    assert(argc > 1);
//    wavfile = fopen(argv[1], "r");
    wavfile = fopen("/Users/jonathanadam/Documents/Sound/Sound", "r");
    assert(wavfile != NULL);
    std::cout<< wavfile<< "\n";
    assert(freadStr(wavfile, 4) == "RIFF");
    uint32_t wavechunksize = freadNum<uint32_t>(wavfile);
    assert(freadStr(wavfile, 4) == "WAVE");
    while(true) {
        std::string chunkName = freadStr(wavfile, 4);
        uint32_t chunkLen = freadNum<uint32_t>(wavfile);
        if(chunkName == "fmt ")
            readFmtChunk(chunkLen);
        else if(chunkName == "data") {
            assert(sampleRate != 0);
            assert(numChannels > 0);
            assert(bytesPerSample > 0);
            printf("len: %.0f secs\n", double(chunkLen) / sampleRate / numChannels / bytesPerSample);
            break; // start playing now
        } else {
            // skip chunk
            assert(fseek(wavfile, chunkLen, SEEK_CUR) == 0);
        }
    }
    
    printf("start playing...\n");
    assert(portAudioOpen());
    
    // wait until stream has finished playing
    while(Pa_IsStreamActive(stream) > 0)
        usleep(1000);
    
    printf("finished\n");
    fclose(wavfile);
    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
