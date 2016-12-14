#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sndfile.h>
#include <thread>
#include <chrono>
#include "portaudio.h"
#include "AudioWorker.hpp"
using namespace std;

/* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (512)
#define NUM_SECONDS     (5)
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

//typedef struct
//{
//    int          frameIndex;  /* Index into sample array. */
//    int          maxFrameIndex;
//    SAMPLE      *recordedSamples;
//}
//paTestData;


/*******************************************************************/

int main(void)
{
//    bool isPlaying = true;
//    std::thread record_thread (record_threadworker,&isPlaying);
//    record_thread.detach();
//    std::this_thread::sleep_for(std::chrono::seconds(5));
//    isPlaying =false;
////    record_thread.join();
//    printf("Were done");
  
    
    //record_short();
    play_file("/Users/jonathanadam/Documents/moartest.wav", 00);
    
    
    
    
//
//    printf("Please select options: 1 - play file; 2 - record short; 3 - record long\n");
//    int choice;
//    cin >> choice;
//    cout << choice;
//    switch (choice)
//    {
//        case(1):
//            cout << "You chose to play a file";
//            break;
//        case(2):
//            cout << "You chose to record a short piece";
//            break;
//        case(3):
////            cout << "You chose to record a file";
//            break;
//            
//    }
//    
}
