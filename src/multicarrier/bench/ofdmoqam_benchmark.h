#ifndef __LIQUID_OFDMOQAM_BENCH_H__
#define __LIQUID_OFDMOQAM_BENCH_H__

#include <sys/resource.h>
#include <math.h>
#include "liquid.h"

#define OFDMOQAM_EXECUTE_BENCH_API(NUM_CHANNELS,M,TYPE) \
(   struct rusage *_start,                              \
    struct rusage *_finish,                             \
    unsigned long int *_num_iterations)                 \
{ ofdmoqam_execute_bench(_start, _finish, _num_iterations, NUM_CHANNELS, M, TYPE); }

// Helper function to keep code base small
void ofdmoqam_execute_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _num_channels,
    unsigned int _m,
    int _type)
{
    // initialize channelizer
    ofdmoqam c = ofdmoqam_create(_num_channels, _m, _type);

    unsigned long int i;

    float complex x[_num_channels];
    float complex y[_num_channels];
    for (i=0; i<_num_channels; i++)
        x[i] = 1.0f + _Complex_I*1.0f;

    // start trials
    *_num_iterations /= _num_channels;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        ofdmoqam_execute(c,x,y);
        ofdmoqam_execute(c,x,y);
        ofdmoqam_execute(c,x,y);
        ofdmoqam_execute(c,x,y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    ofdmoqam_destroy(c);
}

//
void benchmark_ofdmoqam_execute_n4      OFDMOQAM_EXECUTE_BENCH_API(4,   2,  OFDMOQAM_ANALYZER)
void benchmark_ofdmoqam_execute_n16     OFDMOQAM_EXECUTE_BENCH_API(16,  2,  OFDMOQAM_ANALYZER)
void benchmark_ofdmoqam_execute_n64     OFDMOQAM_EXECUTE_BENCH_API(64,  2,  OFDMOQAM_ANALYZER)
void benchmark_ofdmoqam_execute_n256    OFDMOQAM_EXECUTE_BENCH_API(256, 2,  OFDMOQAM_ANALYZER)

#endif // __LIQUID_OFDMOQAM_BENCH_H__

