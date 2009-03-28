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
    fwindow w = fwindow_create(_n);

    unsigned long int i;

    // start trials:
    //   write to port, read from port
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fwindow_push(w, 1.0f);
        fwindow_push(w, 1.0f);
        fwindow_push(w, 1.0f);
        fwindow_push(w, 1.0f);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    fwindow_destroy(w);
}

// 
void xbenchmark_window_n1     WINDOW_BENCH_API(1)
void xbenchmark_window_n4     WINDOW_BENCH_API(4)
void benchmark_window_n16    WINDOW_BENCH_API(16)
void xbenchmark_window_n64    WINDOW_BENCH_API(64)

#endif // __LIQUID_WINDOW_BENCHMARK_H__

