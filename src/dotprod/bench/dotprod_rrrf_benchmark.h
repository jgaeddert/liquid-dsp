#ifndef __LIQUID_DOTPROD_BENCHMARK_H__
#define __LIQUID_DOTPROD_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void dotprod_rrrf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    float x[_n], h[_n], y;
    unsigned int i;
    for (i=0; i<_n; i++) {
        x[i] = 1.0f;
        h[i] = 1.0f;
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = dotprod_rrrf_run(x,h,_n);
        y = dotprod_rrrf_run(x,h,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 2;

}

#define dotprod_rrrf_BENCHMARK_API(N)       \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ dotprod_rrrf_bench(_start, _finish, _num_iterations, N); }

void benchmark_dotprod_rrrf_4       dotprod_rrrf_BENCHMARK_API(4)
void benchmark_dotprod_rrrf_16      dotprod_rrrf_BENCHMARK_API(16)
void benchmark_dotprod_rrrf_64      dotprod_rrrf_BENCHMARK_API(64)
void benchmark_dotprod_rrrf_256     dotprod_rrrf_BENCHMARK_API(256)

#endif // __LIQUID_DOTPROD_BENCHMARK_H__

