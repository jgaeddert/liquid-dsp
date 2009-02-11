#ifndef __LIQUID_INTERLEAVER_BENCHMARK_H__
#define __LIQUID_INTERLEAVER_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define INTERLEAVER_BENCH_API(N,T) \
(   struct rusage *_start, \
    struct rusage *_finish, \
    unsigned long int *_num_iterations) \
{ interleaver_bench(_start, _finish, _num_iterations, N, T); }

// Helper function to keep code base small
void interleaver_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n,
    interleaver_type _type)
{
    // initialize interleaver
    interleaver q = interleaver_create(_n, _type);

    unsigned char x[_n];
    unsigned char y[_n];
    
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        interleaver_interleave(q, x, y);
        interleaver_interleave(q, x, y);
        interleaver_interleave(q, x, y);
        interleaver_interleave(q, x, y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

}

void benchmark_interleaver_8    INTERLEAVER_BENCH_API(8,    INT_BLOCK)
void benchmark_interleaver_16   INTERLEAVER_BENCH_API(16,   INT_BLOCK)
void benchmark_interleaver_64   INTERLEAVER_BENCH_API(64,   INT_BLOCK)
void benchmark_interleaver_256  INTERLEAVER_BENCH_API(256,  INT_BLOCK)

#endif // __LIQUID_INTERLEAVER_BENCHMARK_H__

