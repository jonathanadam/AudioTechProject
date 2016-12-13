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

void read_file_threadworker(AudioFile file, paTestData* testData)
{
    paTestData* data = testData;
    while (data->threadSync == 0){
        int32_t availableBytes;
        void *write_start = TPCircularBufferHead(&data->buffer, &availableBytes);
        float bytes_to_read[availableBytes];
        sf_count_t amount_read = read_from_file(file, bytes_to_read, availableBytes); ///  FIGURE OUT HOW TO GET FRAMES FROM BYTES
        if (amount_read < availableBytes){ // done reading
            data->threadSync = 1;
        }
        TPCircularBufferProduceBytes(&data->buffer, bytes_to_read, availableBytes);
        
    }
}

void write_file_threadworker(AudioFile file, paTestData* testData)
{
    {
        paTestData* data = testData;
        while (data->threadSync == 0){
            int32_t availableBytes;
            void *read_start = TPCircularBufferTail(&data->buffer, &availableBytes);
            float bytes_to_write[availableBytes];
            memcpy(bytes_to_write, read_start, availableBytes); // FIGURE OUT FRAMES FROM BYTES
            TPCircularBufferConsume(&data->buffer, availableBytes);
            write_to_file(file, bytes_to_write, availableBytes);
        }
    }
}


int seek(AudioFile file, double ms);
