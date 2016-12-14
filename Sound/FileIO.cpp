//
//  FileIO.cpp
//  Sound
//
//  Created by Jonathan Adam on 12/11/16.
//  Copyright © 2016 Jonathan Adam. All rights reserved.
//

#include "FileIO.hpp"
#include <sndfile.h>
#include "TPCircularBuffer.h"
#include <stdio.h>
#include <iostream>
#include "portaudio.h"
#include "AudioWorker.hpp"


SF_INFO readinfo = {NULL, NULL, NULL, 0, NULL, NULL};
AudioFile open_file (const char *path, int mode, SF_INFO *sndinfo)
{
    SNDFILE* file = sf_open(path, mode, sndinfo);
    bool test = (file == NULL);
    cout << test;
    AudioFile soundFile = {file, *sndinfo};
    return soundFile;
}

void close_file (AudioFile file)
{
    sf_close(file.sndfile);
}


// reads max. size buffers into buffer from file
sf_count_t read_from_file(AudioFile file, float* buffer, sf_count_t frames)
{
    SNDFILE* soundfile = file.sndfile;
    sf_count_t items_read = sf_readf_float(soundfile, buffer, frames);
    return items_read;
}

// writes size buffers from buffer to file
sf_count_t write_to_file(AudioFile file, float* buffer, sf_count_t frames)
{
    SNDFILE* soundfile = file.sndfile;
    sf_count_t items_written = sf_writef_float(soundfile, buffer, frames);
    return items_written;
    
}

void read_file_threadworker(AudioFile file, TPCircularBuffer* buffer, int* threadSync)
{
    int FRAME_SIZE = file.sndinfo.channels * sizeof(float);
    printf("we are so here");

//    while(1)
        int32_t availableBytes;
        printf("does this still work");
        void* write_start = TPCircularBufferHead(buffer, &availableBytes);
        printf("and this");
        float bytes_to_read[availableBytes];
   //     printf("we are so here too");

        int channels = file.sndinfo.channels;
        sf_count_t amount_to_read = availableBytes/FRAME_SIZE;
        printf("we are so here");
        sf_count_t amount_read =read_from_file(file, bytes_to_read, amount_to_read); ///  FIGURE OUT HOW TO GET FRAMES FROM BYTES
        if (amount_read < availableBytes){ // done reading
           *threadSync = 1;
        }
        TPCircularBufferProduceBytes(buffer, bytes_to_read, availableBytes);
    
}

void write_file_threadworker(AudioFile file, TPCircularBuffer* buffer, int* threadSync)
{
    int FRAME_SIZE = file.sndinfo.channels *sizeof(float);
    int stopWriting = 0;
    {
        while (stopWriting == 0){
            int32_t availableBytes;
            cout << availableBytes << endl;
            void *read_start = TPCircularBufferTail(buffer, &availableBytes);
            float bytes_to_write[availableBytes];
            int number_of_frames = availableBytes/FRAME_SIZE;
            memcpy(bytes_to_write, read_start, availableBytes); // FIGURE OUT FRAMES FROM BYTES
            TPCircularBufferConsume(buffer, availableBytes);
            sf_count_t numberWritten = write_to_file(file, bytes_to_write, number_of_frames);
            if ((numberWritten != number_of_frames) || (availableBytes == 0)){
                stopWriting = 1;
            }
        }
    }
}


int seek(AudioFile file, double ms);
