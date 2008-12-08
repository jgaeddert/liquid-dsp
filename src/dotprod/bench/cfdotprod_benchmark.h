#ifndef __LIQUID_CFDOTPROD_BENCHMARK_H__
#define __LIQUID_CFDOTPROD_BENCHMARK_H__

#include <sys/resource.h>
#include "../src/dotprod.h"

// Helper function to keep code base small
void cfdotprod_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    float complex x[_n], h[_n], y;
    unsigned int i;
    for (i=0; i<_n; i++) {
        x[_n] = 1.0f + _Complex_I*1.0f;
        h[_n] = 1.0f - _Complex_I*1.0f;
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = cfdotprod_run(x,h,_n);
        y = cfdotprod_run(x,h,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 2;

}

#define CFDOTPROD_BENCHMARK_API(N)      \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ cfdotprod_bench(_start, _finish, _num_iterations, N); }

void benchmark_cfdotprod_4      CFDOTPROD_BENCHMARK_API(4)
void benchmark_cfdotprod_16     CFDOTPROD_BENCHMARK_API(16)
void benchmark_cfdotprod_64     CFDOTPROD_BENCHMARK_API(64)
void benchmark_cfdotprod_256    CFDOTPROD_BENCHMARK_API(256)

#endif // __LIQUID_CFDOTPROD_BENCHMARK_H__

