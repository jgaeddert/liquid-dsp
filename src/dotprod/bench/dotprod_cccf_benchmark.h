#ifndef __LIQUID_dotprod_cccf_BENCHMARK_H__
#define __LIQUID_dotprod_cccf_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void dotprod_cccf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    float complex x[_n], h[_n], y;
    unsigned int i;
    for (i=0; i<_n; i++) {
        x[i] = 1.0f + _Complex_I*1.0f;
        h[i] = 1.0f - _Complex_I*1.0f;
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = dotprod_cccf_run(x,h,_n);
        y = dotprod_cccf_run(x,h,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 2;

}

#define dotprod_cccf_BENCHMARK_API(N)      \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ dotprod_cccf_bench(_start, _finish, _num_iterations, N); }

void benchmark_dotprod_cccf_4      dotprod_cccf_BENCHMARK_API(4)
void benchmark_dotprod_cccf_16     dotprod_cccf_BENCHMARK_API(16)
void benchmark_dotprod_cccf_64     dotprod_cccf_BENCHMARK_API(64)
void benchmark_dotprod_cccf_256    dotprod_cccf_BENCHMARK_API(256)

#endif // __LIQUID_dotprod_cccf_BENCHMARK_H__

