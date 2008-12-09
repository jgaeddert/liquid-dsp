#ifndef __LIQUID_INTERP_BENCHMARK_H__
#define __LIQUID_INTERP_BENCHMARK_H__

#include <sys/resource.h>
#include "../src/filter.h"

// Helper function to keep code base small
void interp_bench(
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

    interp q = interp_create(_M,h,_h_len);

    float y[_M];
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        interp_execute(q,1.0f,y);
        interp_execute(q,1.0f,y);
        interp_execute(q,1.0f,y);
        interp_execute(q,1.0f,y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    interp_destroy(q);
}

#define INTERP_BENCHMARK_API(M,H_LEN)   \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ interp_bench(_start, _finish, _num_iterations, M, H_LEN); }

void benchmark_interp_m2_h8     INTERP_BENCHMARK_API(2,8)
void benchmark_interp_m4_h16    INTERP_BENCHMARK_API(4,16)
void benchmark_interp_m8_h64    INTERP_BENCHMARK_API(8,64)

#endif // __LIQUID_INTERP_BENCHMARK_H__

