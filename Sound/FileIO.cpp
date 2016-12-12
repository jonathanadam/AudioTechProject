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
size_t read_from_file(AudioFile file, void* buffer, size_t size);

// writes size buffers from buffer to file
size_t write_to_file(AudioFile file, float* buffer, size_t size)
{
    SNDFILE* soundfile = file.sndfile;
    size_t items_written = sf_write_float(soundfile, buffer, size);
    return items_written;
    
}

int seek(AudioFile file, double ms);
