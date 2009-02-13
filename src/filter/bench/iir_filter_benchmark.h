#ifndef __LIQUID_IIR_FILTER_RRRF_BENCHMARK_H__
#define __LIQUID_IIR_FILTER_RRRF_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void iir_filter_rrrf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    float b[_n], a[_n], y;
    unsigned int i;
    for (i=0; i<_n; i++) {
        b[i] = 1.0f;
        a[i] = 1.0f;
    }

    iir_filter_rrrf f = iir_filter_rrrf_create(b,_n,a,_n);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        iir_filter_rrrf_execute(f,1.0f,&y);
        iir_filter_rrrf_execute(f,1.0f,&y);
        iir_filter_rrrf_execute(f,1.0f,&y);
        iir_filter_rrrf_execute(f,1.0f,&y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    iir_filter_rrrf_destroy(f);

}

#define IIR_FILTER_RRRF_BENCHMARK_API(N)     \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ iir_filter_rrrf_bench(_start, _finish, _num_iterations, N); }

void benchmark_iir_filter_rrrf_4    IIR_FILTER_RRRF_BENCHMARK_API(4)
void benchmark_iir_filter_rrrf_8    IIR_FILTER_RRRF_BENCHMARK_API(8)
void benchmark_iir_filter_rrrf_16   IIR_FILTER_RRRF_BENCHMARK_API(16)

#endif // __LIQUID_IIR_FILTER_RRRF_BENCHMARK_H__

