#ifndef __LIQUID_FIRHILB_BENCHMARK_H__
#define __LIQUID_FIRHILB_BENCHMARK_H__

#include <sys/resource.h>
#include "../src/filter.h"

// Helper function to keep code base small
void firhilb_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _h_len)
{
    float h[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++)
        h[i] = 1.0f;

    firhilb q = firhilb_create(_h_len);

    float x[] = {1.0f, -1.0f};
    float complex y;
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        firhilb_execute(q,x,&y);
        firhilb_execute(q,x,&y);
        firhilb_execute(q,x,&y);
        firhilb_execute(q,x,&y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    firhilb_destroy(q);
}

#define FIRHILB_BENCHMARK_API(H_LEN)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ firhilb_bench(_start, _finish, _num_iterations, H_LEN); }

void benchmark_firhilb_h13      FIRHILB_BENCHMARK_API(13)   // m=3
void benchmark_firhilb_h21      FIRHILB_BENCHMARK_API(21)   // m=5
void benchmark_firhilb_h37      FIRHILB_BENCHMARK_API(37)   // m=9
void benchmark_firhilb_h53      FIRHILB_BENCHMARK_API(53)   // m=13

#endif // __LIQUID_FIRHILB_BENCHMARK_H__

