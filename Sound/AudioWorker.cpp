//
//  AudioWorker.cpp
//  Sound
//
//  Created by Jonathan Adam on 12/11/16.
//  Copyright © 2016 Jonathan Adam. All rights reserved.
//

#include "AudioWorker.hpp"

#include "portaudio.h"
#include "FileIO.hpp"
#include "TPCircularBuffer.h"

#include <iostream>
#include <thread>
#include <chrono>
/* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (512)
#define NUM_SECONDS     (5)
//int NUM_SECONDS;
#define NUM_CHANNELS    (2)
/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/


/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif




int record_short()
{
    printf("Hi there. \n");
//    printf("Please let us know how long you want to record for. Not too long!\n");
//    int seconds;
//    cin >> seconds;
//    NUM_SECONDS = seconds;
    PaStreamParameters  inputParameters,
    outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    int                 numBytes;
    
//    printf("patest_record.c\n"); fflush(stdout);
//    
    data.maxFrameIndex = totalFrames = NUM_SECONDS * SAMPLE_RATE; /* Record for a few seconds. */
    data.frameIndex = 0;
    numSamples = totalFrames * NUM_CHANNELS;
    numBytes = numSamples * sizeof(SAMPLE);
    TPCircularBufferInit(&data.buffer, numBytes);
//
    err = Pa_Initialize();
    if( err != paNoError ) goto done;
    
    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        //        goto done;
    }
    inputParameters.channelCount = 2;                    /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
////
//    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
                        &stream,
                        &inputParameters,
                        NULL,                  /* &outputParameters, */
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                        recordCallback,
                        &data );
    //    if( err != paNoError ) goto done;
    
    
    err = Pa_StartStream( stream );
    if( err != paNoError ) goto done;
    printf("\n=== Now recording!! Please speak into the microphone. ===\n"); fflush(stdout);
    
    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(1000);
    }
    if( err < 0 ) goto done;
    
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;

    
    /*Write audio to a file with a single call */
    
    {
        SoundInfo soundinfo = {totalFrames, SAMPLE_RATE, NUM_CHANNELS, 0x010000|0x0002, NULL, NULL};
        AudioFile testfile = open_file("/Users/jonathanadam/Documents/buffertest.wav",SFM_WRITE, &soundinfo);
        printf("Writing\n");
        
        printf("Number of bytes is");
        cout<< totalFrames;
        write_file_threadworker(testfile, &data.buffer, &data.threadSync);
        printf("\n\n\n\n Number of stuff written is \n");
        //        cout<<stuff;
        close_file(testfile);
    }
    
done:
//    Pa_Terminate();
//    if( data.recordedSamples )       /* Sure it is NULL or valid. */
//        delete[] data.recordedSamples ;
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    //return err;
    

    
    //    /* Playback recorded data.  -------------------------------------------- */
        data.frameIndex = 0;
        TPCircularBufferClear(&data.buffer);
//
    {
        SF_INFO readinfo = {NULL, NULL, NULL, 0, NULL, NULL};
        AudioFile othertest = open_file("/Users/jonathanadam/Documents/buffertest.wav",SFM_READ, &readinfo);
//        sf_count_t moarstuff = read_from_file(othertest, data.recordedSamples, totalFrames);
//        int32_t available;
//        void *head = TPCircularBufferHead(&data.buffer, &available);
//        memcpy(head, data.recordedSamples, numBytes);
//        TPCircularBufferProduce(&data.buffer, numBytes);
        
        printf("trying to do the thing.\n\nn\n");
        read_file_threadworker(othertest, &data.buffer, &data.threadSync);
        //Buffer experiment

        //end of buffer experiment
        printf("weve tried to write stuff");
        close_file(othertest);
    }
    
        outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
        if (outputParameters.device == paNoDevice) {
            printf("Here are teh devices i found");
            cout <<Pa_GetDeviceCount();
            fprintf(stderr,"Error: No default output device.\n");
            goto done_again;
        }
        outputParameters.channelCount = 2;                     /* stereo output */
        outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;
    
        printf("\n=== Now playing back. ===\n"); fflush(stdout);
        err = Pa_OpenStream(
                            &stream,
                            NULL, /* no input */
                            &outputParameters,
                            SAMPLE_RATE,
                            FRAMES_PER_BUFFER,
                            paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                            playCallback,
                            &data );
        if( err != paNoError ) goto done_again;
    
        if( stream )
        {
            err = Pa_StartStream( stream );
            if( err != paNoError ) goto  done_again;
    
            printf("Waiting for playback to finish.\n"); fflush(stdout);
    
            while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
            if( err < 0 ) goto done_again;
    
            err = Pa_CloseStream( stream );
            if( err != paNoError ) goto done_again;
    
            printf("Done.\n"); fflush(stdout);
        }
    
        done_again:
        Pa_Terminate();
//        if( data.recordedSamples )       /* Sure it is NULL or valid. */
//            delete[] data.recordedSamples ;
        if( err != paNoError )
        {
            fprintf( stderr, "An error occured while using the portaudio stream\n" );
            fprintf( stderr, "Error number: %d\n", err );
            fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
            err = 1;          /* Always return 0 or 1, but no other return codes. */
        }
        return err;

}

//static int shortrecordCallback( const void *inputBuffer, void *outputBuffer,
//                          unsigned long framesPerBuffer,
//                          const PaStreamCallbackTimeInfo* timeInfo,
//                          PaStreamCallbackFlags statusFlags,
//                          void *userData )
//{
//    paTestData *data = (paTestData*)userData;
//    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
//    SAMPLE *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
//    long framesToCalc;
//    long i;
//    int finished;
//    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;
//    
//    (void) outputBuffer; /* Prevent unused variable warnings. */
//    (void) timeInfo;
//    (void) statusFlags;
//    (void) userData;
//    
//    if( framesLeft < framesPerBuffer )
//    {
//        framesToCalc = framesLeft;
//        finished = paComplete;
//    }
//    else
//    {
//        framesToCalc = framesPerBuffer;
//        finished = paContinue;
//    }
//    
//    if( inputBuffer == NULL )
//    {
//        for( i=0; i<framesToCalc; i++ )
//        {
//            *wptr++ = SAMPLE_SILENCE;  /* left */
//            if( NUM_CHANNELS == 2 ) *wptr++ = SAMPLE_SILENCE;  /* right */
//        }
//    }
//    else
//    {
//        for( i=0; i<framesToCalc; i++ )
//        {
//            *wptr++ = *rptr++;  /* left */
//            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
//        }
//    }
//    data->frameIndex += framesToCalc;
//    return finished;
//}


static int recordCallback( const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData )
{
    paTestData *data = (paTestData*)userData;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
    int32_t availableBytes;
    float *wptr = (float*)TPCircularBufferHead(&data->buffer, &availableBytes);
    //SAMPLE *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    long framesToCalc;
    long i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;
    
    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;
    
    if( framesLeft < framesPerBuffer )
    {
        framesToCalc = framesLeft;
        finished = paComplete;
    }
    else
    {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }
    
    if( inputBuffer == NULL )
    {
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = SAMPLE_SILENCE;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = SAMPLE_SILENCE;  /* right */
        }
    }
    else
    {
        
        TPCircularBufferProduceBytes(&data->buffer, rptr, framesToCalc*sizeof(float)*NUM_CHANNELS);
//        memcpy(wptr, rptr, framesToCalc*sizeof(float)*NUM_CHANNELS);
//        TPCircularBufferProduce(&data->buffer, framesToCalc*sizeof(float)*NUM_CHANNELS);
//        for( i=0; i<framesToCalc; i++ )
        {
//            if (NUM_CHANNELS == 1)
            {
                //TPCircularBufferProduceBytes(&data->buffer, rptr, NUM_CHANNELS*sizeof(float));
            }
//            *wptr++ = *rptr++;  /* left */
//            TPCircularBufferProduce(&data->buffer, sizeof(float));
//            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
//            TPCircularBufferProduce(&data->buffer, sizeof(float));
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
}


static int playCallback( const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData )
{
    paTestData *data = (paTestData*)userData;
    int32_t availableBytes;
    float *rptr = (float*)TPCircularBufferTail(&data->buffer, &availableBytes); // dangerous
    //SAMPLE *rptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    SAMPLE *wptr = (SAMPLE*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;
    
    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;
    
    int32_t bytes_per_buffer = framesPerBuffer*sizeof(SAMPLE)*NUM_CHANNELS;
    if( availableBytes < bytes_per_buffer ) //used to be framesLeft<framesPerBuffer
    {
        framesLeft = availableBytes/(sizeof(SAMPLE)*NUM_CHANNELS);// This used to not be here
        /* final buffer... */
        //for( i=0; i<framesLeft; i++ ) //used to be i
        for (i= 0; i<framesLeft; i++)
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        int32_t number_consumed = framesLeft* sizeof(float);
        if (NUM_CHANNELS == 2) number_consumed = number_consumed*2;
        TPCircularBufferConsume(&data->buffer, number_consumed);
        for( ; i<framesPerBuffer; i++ )
        {
            *wptr++ = 0;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = 0;  /* right */
        }
        //data->frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            TPCircularBufferConsume(&data->buffer, sizeof(float));
            if( NUM_CHANNELS == 2 )
            {
                *wptr++ = *rptr++;  /* right */
                TPCircularBufferConsume(&data->buffer, sizeof(float));
            }
        }
//        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}

static int shortplayCallback( const void *inputBuffer, void *outputBuffer,
                        unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo* timeInfo,
                        PaStreamCallbackFlags statusFlags,
                        void *userData )
{
    paTestData *data = (paTestData*)userData;
    SAMPLE *rptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    int32_t available;
    //float *rptr = TPCircularBufferTail(&data->buffer, &available);
    SAMPLE *wptr = (SAMPLE*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;
    
    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;
    
    if( framesLeft < framesPerBuffer )
    {
        /* final buffer... */
        for( i=0; i<framesLeft; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        for( ; i<framesPerBuffer; i++ )
        {
            *wptr++ = 0;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = 0;  /* right */
        }
        data->frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    return finished;
}


void record_threadworker(bool keepPlaying)
{
    while (keepPlaying)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        printf("We are still playing");
    }
}


int record_file();

void play_file(const char *filename, double startingpoint)
{
    SF_INFO readinfo = {NULL, NULL, NULL, 0, NULL, NULL};

    // SET UP PORTAUDIO PARAMS
    PaStreamParameters  inputParameters,
    outputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    int                 numBytes;
    
    data.maxFrameIndex = totalFrames = NUM_SECONDS * SAMPLE_RATE; /* Record for a few seconds. */
    data.frameIndex = 0;
    numSamples = totalFrames * NUM_CHANNELS;
    numBytes = numSamples * sizeof(SAMPLE);
    TPCircularBufferInit(&data.buffer, numBytes);
    
    
    err = Pa_Initialize();
    
    AudioFile fileToRead = open_file(filename, SFM_READ, &readinfo);
    read_file_threadworker(fileToRead, &data.buffer, &data.threadSync);
//    std::thread readFileToBuffer(read_file_threadworker, &fileToRead, &data.buffer, &data.threadSync);
//    readFileToBuffer.detach();
   // std::this_thread::sleep_for(std::chrono::seconds(1));
//    printf("okay now lets get started");
    
    if( err != paNoError ) goto done;

    //FIND OUTPUT DEVICES
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default output device.\n");
        goto done;
    }
    outputParameters.channelCount = 2;                     /* stereo output */
    outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    
    
//    {
//        AudioFile fileToRead = open_file(filename, SFM_READ, &readinfo);
//        std::thread readFileToBuffer(read_file_threadworker, fileToRead, &data.buffer, &data.threadSync);
//        readFileToBuffer.detach();
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//        printf("okay now lets get started");
////        readFileToBuffer.join();
////        read_file_threadworker(fileToRead, &data.buffer, &data.threadSync);
//    }
    
    printf("\nplaying back\n"); fflush(stdout);
    err = Pa_OpenStream(
                        &stream,
                        NULL, /* no input */
                        &outputParameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                        playCallback,
                        &data );
    if( err != paNoError ) goto done;
    
    if( stream )
    {
        err = Pa_StartStream( stream );
        if( err != paNoError ) goto  done;
        
        printf("Waiting for playback to finish.\n"); fflush(stdout);
        
        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
        if( err < 0 ) goto done;
        
        err = Pa_CloseStream( stream );
        if( err != paNoError ) goto done;
        
        printf("Done.\n"); fflush(stdout);
    }
    
done:
    Pa_Terminate();
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
   // return err;
    
}
