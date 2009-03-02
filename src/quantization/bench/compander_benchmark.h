#ifndef __LIQUID_COMPANDER_BENCHMARK_H__
#define __LIQUID_COMPANDER_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// 
void benchmark_compress_mulaw(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;

    float x = -0.1f;
    float mu = 255.0f;
    float y;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = compress_mulaw(x,mu);
        y = compress_mulaw(x,mu);
        y = compress_mulaw(x,mu);
        y = compress_mulaw(x,mu);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
void benchmark_expand_mulaw(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;

    float x;
    float mu = 255.0f;
    float y = 0.75f;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        x = expand_mulaw(y,mu);
        x = expand_mulaw(y,mu);
        x = expand_mulaw(y,mu);
        x = expand_mulaw(y,mu);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

#endif // __LIQUID_COMPANDER_BENCHMARK_H__

