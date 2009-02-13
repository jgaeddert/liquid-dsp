#ifndef __LIQUID_INTERP_RRRF_BENCHMARK_H__
#define __LIQUID_INTERP_RRRF_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void interp_rrrf_bench(
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

    interp_rrrf q = interp_rrrf_create(_M,h,_h_len);

    float y[_M];
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        interp_rrrf_execute(q,1.0f,y);
        interp_rrrf_execute(q,1.0f,y);
        interp_rrrf_execute(q,1.0f,y);
        interp_rrrf_execute(q,1.0f,y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    interp_rrrf_destroy(q);
}

#define INTERP_RRRF_BENCHMARK_API(M,H_LEN)   \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ interp_rrrf_bench(_start, _finish, _num_iterations, M, H_LEN); }

void benchmark_interp_rrrf_m2_h8    INTERP_RRRF_BENCHMARK_API(2,8)
void benchmark_interp_rrrf_m4_h16   INTERP_RRRF_BENCHMARK_API(4,16)
void benchmark_interp_rrrf_m8_h64   INTERP_RRRF_BENCHMARK_API(8,64)

#endif // __LIQUID_INTERP_RRRF_BENCHMARK_H__

