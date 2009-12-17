#ifndef __LIQUID_WINDOW_BENCHMARK_H__
#define __LIQUID_WINDOW_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define WINDOW_BENCH_API(N)              \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ window_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void window_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    // initialize port
    cfwindow w = cfwindow_create(_n);

    unsigned long int i;

    // start trials:
    //   write to port, read from port
    *_num_iterations *= 32;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        cfwindow_push(w, 1.0f);
        cfwindow_push(w, 1.0f);
        cfwindow_push(w, 1.0f);
        cfwindow_push(w, 1.0f);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    cfwindow_destroy(w);
}

// 
void benchmark_cfwindow_n16     WINDOW_BENCH_API(16)
void benchmark_cfwindow_n32     WINDOW_BENCH_API(32)
void benchmark_cfwindow_n64     WINDOW_BENCH_API(64)
void benchmark_cfwindow_n128    WINDOW_BENCH_API(128)
void benchmark_cfwindow_n256    WINDOW_BENCH_API(256)

#endif // __LIQUID_WINDOW_BENCHMARK_H__

