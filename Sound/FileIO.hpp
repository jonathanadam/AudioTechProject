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


typedef SF_INFO SoundInfo;
typedef struct{
    SNDFILE* sndfile;
    SF_INFO sndinfo;
} AudioFile;


AudioFile open_file (const char *path, int mode, SF_INFO *sndinfo);
void close_file (AudioFile file);

// reads max. size buffers into buffer from file
size_t read_from_file(AudioFile file, void* buffer, size_t size);

// writes size buffers from buffer to file
size_t write_to_file(AudioFile file, void* buffer, size_t size);

void read_file_threadworker(AudioFile file, void* buffer);

void write_file_threadworker(AudioFile file, void* buffer);

int seek(AudioFile file, double ms);

#endif /* FileIO_hpp */
