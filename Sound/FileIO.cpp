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

int seek(AudioFile file, double ms);
