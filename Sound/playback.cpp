//
//
//#include <stdio.h>
//#include <string.h>
//#include <portaudio.h>
//#include <string>
//#include <stdint.h>
//#include <unistd.h>
//#include <assert.h>
//#include <iostream>
//#include <sndfile.h>
//#include <math.h>
//
//#define NUM_SECONDS   (5)
//#define SAMPLE_RATE   (44100)
//#define FRAMES_PER_BUFFER  (64)
//#define BUFFER_LEN (1024)
//#define MAX_CHANNELS (6)
//
//#ifndef M_PI
//#define M_PI  (3.14159265)
//#endif
//
//#define TABLE_SIZE   (200)
//typedef struct
//{
//    float sine[TABLE_SIZE];
//    int left_phase;
//    int right_phase;
//    char message[20];
//}
//paTestData;
//
//
//static int playCallback( const void *inputBuffer, void *outputBuffer,
//                        unsigned long framesPerBuffer,
//                        const PaStreamCallbackTimeInfo* timeInfo,
//                        PaStreamCallbackFlags statusFlags,
//                        void *userData )
//{
//    paTestData *data = (paTestData*)userData;
//    SAMPLE *rptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
//    SAMPLE *wptr = (SAMPLE*)outputBuffer;
//    unsigned int i;
//    int finished;
//    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;
//    
//    (void) inputBuffer; /* Prevent unused variable warnings. */
//    (void) timeInfo;
//    (void) statusFlags;
//    (void) userData;
//    
//    if( framesLeft < framesPerBuffer )
//    {
//        /* final buffer... */
//        for( i=0; i<framesLeft; i++ )
//        {
//            *wptr++ = *rptr++;  /* left */
//            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
//        }
//        for( ; i<framesPerBuffer; i++ )
//        {
//            *wptr++ = 0;  /* left */
//            if( NUM_CHANNELS == 2 ) *wptr++ = 0;  /* right */
//        }
//        data->frameIndex += framesLeft;
//        finished = paComplete;
//    }
//    else
//    {
//        for( i=0; i<framesPerBuffer; i++ )
//        {
//            *wptr++ = *rptr++;  /* left */
//            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
//        }
//        data->frameIndex += framesPerBuffer;
//        finished = paContinue;
//    }
//    return finished;
//}
//
//int no_main(){
//    data.frameIndex = 0;
//    
//    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
//    if (outputParameters.device == paNoDevice) {
//        fprintf(stderr,"Error: No default output device.\n");
//        goto done;
//    }
//    outputParameters.channelCount = 2;                     /* stereo output */
//    outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
//    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
//    outputParameters.hostApiSpecificStreamInfo = NULL;
//    
//    printf("\n=== Now playing back. ===\n"); fflush(stdout);
//    err = Pa_OpenStream(
//                        &stream,
//                        NULL, /* no input */
//                        &outputParameters,
//                        SAMPLE_RATE,
//                        FRAMES_PER_BUFFER,
//                        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
//                        playCallback,
//                        &data );
//    if( err != paNoError ) goto done;
//    
//    if( stream )
//    {
//        err = Pa_StartStream( stream );
//        if( err != paNoError ) goto done;
//        
//        printf("Waiting for playback to finish.\n"); fflush(stdout);
//        
//        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
//        if( err < 0 ) goto done;
//        
//        err = Pa_CloseStream( stream );
//        if( err != paNoError ) goto done;
//        
//        printf("Done.\n"); fflush(stdout);
//    }
//
//    
//}
