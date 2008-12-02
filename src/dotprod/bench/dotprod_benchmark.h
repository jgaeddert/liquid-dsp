#ifndef __LIQUID_DOTPROD_BENCHMARK_H__
#define __LIQUID_DOTPROD_BENCHMARK_H__

#include <sys/resource.h>
#include "../src/dotprod.h"

// Helper function to keep code base small
void fdotprod_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    float x[_n], h[_n], y;
    unsigned int i;
    for (i=0; i<_n; i++) {
        x[_n] = 1.0f;
        h[_n] = 1.0f;
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = fdotprod_run(x,h,_n);
        y = fdotprod_run(x,h,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 2;

}

#define FDOTPROD_BENCHMARK_API(N)       \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fdotprod_bench(_start, _finish, _num_iterations, N); }

void benchmark_fdotprod_4       FDOTPROD_BENCHMARK_API(4)
void benchmark_fdotprod_16      FDOTPROD_BENCHMARK_API(16)
void benchmark_fdotprod_64      FDOTPROD_BENCHMARK_API(64)
void benchmark_fdotprod_256     FDOTPROD_BENCHMARK_API(256)

#endif // __LIQUID_DOTPROD_BENCHMARK_H__

