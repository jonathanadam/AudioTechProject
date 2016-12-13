//
//  AudioWorker.hpp
//  Sound
//
//  Created by Jonathan Adam on 12/11/16.
//  Copyright © 2016 Jonathan Adam. All rights reserved.
//

#ifndef AudioWorker_hpp
#define AudioWorker_hpp
#include "portaudio.h"

#include <stdio.h>

#include "TPCircularBuffer.h"

typedef struct
{
    int          frameIndex;  /* Index into sample array. */
    int          maxFrameIndex;
    int          threadSync;
    float      *recordedSamples;
    TPCircularBuffer buffer;
}
paTestData;

int record_short();

int record_file();

void record_threadworker(bool keepPlaying);

void play_file(const char *filename, double startingpoint);

static int playCallback(const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData);
static int recordCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData);
static int shortrecordCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData);
static int shortplayCallback(const void *inputBuffer, void *outputBuffer,
                          unsigned long framesPerBuffer,
                          const PaStreamCallbackTimeInfo* timeInfo,
                          PaStreamCallbackFlags statusFlags,
                          void *userData);
#endif /* AudioWorker_hpp */
