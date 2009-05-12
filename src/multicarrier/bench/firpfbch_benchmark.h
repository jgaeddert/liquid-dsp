#ifndef __LIQUID_FIRPFBCH_BENCH_H__
#define __LIQUID_FIRPFBCH_BENCH_H__

#include <sys/resource.h>
#include "liquid.h"

#define FIRPFBCH_EXECUTE_BENCH_API(NUM_CHANNELS,M,TYPE) \
(   struct rusage *_start,                              \
    struct rusage *_finish,                             \
    unsigned long int *_num_iterations)                 \
{ firpfbch_execute_bench(_start, _finish, _num_iterations, NUM_CHANNELS, M, TYPE); }

// Helper function to keep code base small
void firpfbch_execute_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _num_channels,
    unsigned int _m,
    int _type)
{
    // initialize channelizer
    firpfbch c = firpfbch_create(_num_channels, _m, -60.0f, FIRPFBCH_NYQUIST, _type);

    unsigned long int i;

    float complex x[_num_channels];
    float complex y[_num_channels];
    for (i=0; i<_num_channels; i++)
        x[i] = 1.0f + _Complex_I*1.0f;

    // scale number of iterations to keep execution time
    // relatively linear
    *_num_iterations /= _num_channels;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        firpfbch_execute(c,x,y);
        firpfbch_execute(c,x,y);
        firpfbch_execute(c,x,y);
        firpfbch_execute(c,x,y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    firpfbch_destroy(c);
}

//
void benchmark_firpfbch_execute_n4      FIRPFBCH_EXECUTE_BENCH_API(4,   2,  FIRPFBCH_ANALYZER)
void benchmark_firpfbch_execute_n16     FIRPFBCH_EXECUTE_BENCH_API(16,  2,  FIRPFBCH_ANALYZER)
void benchmark_firpfbch_execute_n64     FIRPFBCH_EXECUTE_BENCH_API(64,  2,  FIRPFBCH_ANALYZER)
void benchmark_firpfbch_execute_n256    FIRPFBCH_EXECUTE_BENCH_API(256, 2,  FIRPFBCH_ANALYZER)

#endif // __LIQUID_FIRPFBCH_BENCH_H__

