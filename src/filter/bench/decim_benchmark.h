#ifndef __LIQUID_BENCH_BENCHMARK_H__
#define __LIQUID_BENCH_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void decim_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _M,
    unsigned int _h_len)
{
    float h[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++)
        h[i] = 1.0f;

    decim q = decim_create(_M,h,_h_len);

    float x[_M], y;
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        decim_execute(q,x,&y,0);
        decim_execute(q,x,&y,0);
        decim_execute(q,x,&y,0);
        decim_execute(q,x,&y,0);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    decim_destroy(q);
}

#define BENCH_BENCHMARK_API(M,H_LEN)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ decim_bench(_start, _finish, _num_iterations, M, H_LEN); }

void benchmark_decim_m2_h8      BENCH_BENCHMARK_API(2,8)
void benchmark_decim_m4_h16     BENCH_BENCHMARK_API(4,16)
void benchmark_decim_m8_h64     BENCH_BENCHMARK_API(8,64)

#endif // __LIQUID_BENCH_BENCHMARK_H__

