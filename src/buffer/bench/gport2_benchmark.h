#ifndef __LIQUID_GPORT2_BENCHMARK_H__
#define __LIQUID_GPORT2_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define GPORT2_BENCH_API(N)             \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ gport2_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void gport2_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    // initialize port
    gport2 p = gport2_create(2*_n-1,sizeof(int));
    int w[_n], r[_n];

    unsigned long int i;

    // start trials:
    //   write to port, read from port
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        gport2_produce(p,(void*)w,_n);
        gport2_consume(p,(void*)r,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= _n;

    gport2_destroy(p);
}

// 
void benchmark_gport2_n1    GPORT2_BENCH_API(1)
void benchmark_gport2_n4    GPORT2_BENCH_API(4)
void benchmark_gport2_n16   GPORT2_BENCH_API(16)
void benchmark_gport2_n64   GPORT2_BENCH_API(64)

#endif // __LIQUID_GPORT2_BENCHMARK_H__

