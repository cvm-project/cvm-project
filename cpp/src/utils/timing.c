//Use gcc -lrt flag


#ifndef CPP_TIMING_H
#define CPP_TIMING_H

#include <stdio.h>
#include <time.h>


struct timespec timing_tp1, timing_tp2;
#define TICK1 clock_gettime( CLOCK_MONOTONIC, &timing_tp1);
#define TOCK1 clock_gettime( CLOCK_MONOTONIC, &timing_tp2);
#define DIFF1 (timing_tp2.tv_sec - timing_tp1.tv_sec) * 1000 + (timing_tp2.tv_nsec - timing_tp1.tv_nsec) / 1000 / 1000
#define GETTICK1 timing_tp1.tv_sec * 1000  + timing_tp1.tv_nsec / 1000 / 1000

struct timespec timing_tp3, timing_tp4;
#define TICK2 clock_gettime( CLOCK_MONOTONIC, &timing_tp3);
#define TOCK2 clock_gettime( CLOCK_MONOTONIC, &timing_tp4);
#define DIFF2 (timing_tp4.tv_sec - timing_tp3.tv_sec) * 1000*1000 + (timing_tp4.tv_nsec - timing_tp3.tv_nsec) / 1000

struct timespec timing_tp5, timing_tp6;
#define TICK3 clock_gettime( CLOCK_MONOTONIC, &timing_tp5);
#define TOCK3 clock_gettime( CLOCK_MONOTONIC, &timing_tp6);
#define DIFF3 (timing_tp6.tv_sec - timing_tp5.tv_sec) * 1000*1000 + (timing_tp6.tv_nsec - timing_tp5.tv_nsec) / 1000

struct timespec timing_tp7, timing_tp8;
#define TICK4 clock_gettime( CLOCK_MONOTONIC, &timing_tp7);
#define TOCK4 clock_gettime( CLOCK_MONOTONIC, &timing_tp8);
#define DIFF4 (timing_tp8.tv_sec - timing_tp7.tv_sec) * 1000*1000 + (timing_tp8.tv_nsec - timing_tp7.tv_nsec) / 1000

//int main ()
//{

//    TICK1
//    sleep(1);
//    TOCK1
//
//    TICK2
//    sleep(2);
//    TOCK2
//
//    TICK3
//    sleep(3);
//    TOCK3
//
//    TICK4
//    sleep(4);
//    TOCK4
//
//    printf ( "diff %d\n", DIFF1);
//    printf ( "diff %d\n", DIFF2);
//    printf ( "diff %d\n", DIFF3);
//    printf ( "diff %d\n", DIFF4);

//    return 0;
//}


#endif