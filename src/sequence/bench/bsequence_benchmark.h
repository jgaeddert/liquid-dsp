#ifndef __LIQUID_BSEQUENCE_BENCHMARK_H__
#define __LIQUID_BSEQUENCE_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void bsequence_correlate_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    // create and initialize binary sequences
    bsequence bs1 = bsequence_create(_n);
    bsequence bs2 = bsequence_create(_n);

    unsigned long int i;
    int rxy;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        rxy = bsequence_correlate(bs1, bs2);
        rxy = bsequence_correlate(bs1, bs2);
        rxy = bsequence_correlate(bs1, bs2);
        rxy = bsequence_correlate(bs1, bs2);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up memory
    bsequence_destroy(bs1);
    bsequence_destroy(bs2);
}

#define BSEQUENCE_BENCHMARK_API(N)          \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ bsequence_correlate_bench(_start, _finish, _num_iterations, N); }

// 
void benchmark_bsequence_xcorr_n16      BSEQUENCE_BENCHMARK_API(16)
void benchmark_bsequence_xcorr_n64      BSEQUENCE_BENCHMARK_API(64)
void benchmark_bsequence_xcorr_n256     BSEQUENCE_BENCHMARK_API(256)

#endif // __LIQUID_BSEQUENCE_BENCHMARK_H__

