//
//  FileIO.hpp
//  Sound
//
//  Created by Jonathan Adam on 12/11/16.
//  Copyright © 2016 Jonathan Adam. All rights reserved.
//

#ifndef FileIO_hpp
#define FileIO_hpp

#include <stdio.h>
#include <sndfile.h>
#include "TPCircularBuffer.h"
#include "AudioWorker.hpp"





typedef SF_INFO SoundInfo;
typedef struct{
    SNDFILE* sndfile;
    SF_INFO sndinfo;
} AudioFile;


AudioFile open_file (const char *path, int mode, SF_INFO *sndinfo);
void close_file (AudioFile file);

// reads max. size buffers into buffer from file
sf_count_t read_from_file(AudioFile file, float* buffer, sf_count_t frames);

// writes size buffers from buffer to file
sf_count_t write_to_file(AudioFile file, float* buffer, sf_count_t frames);

void read_file_threadworker(AudioFile file, TPCircularBuffer* buffer, int* threadSync);

void write_file_threadworker(AudioFile file, TPCircularBuffer* buffer, int* threadSync);

int seek(AudioFile file, double ms);

#endif /* FileIO_hpp */
