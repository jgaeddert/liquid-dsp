#ifndef __LIQUID_GPORT_BENCHMARK_H__
#define __LIQUID_GPORT_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define GPORT_BENCH_API(N)              \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ gport_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void gport_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    // initialize port
    gport p = gport_create(2*_n-1,sizeof(int));

    unsigned long int i;

    // start trials:
    //   write to port, read from port
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        gport_producer_lock(p,_n);
        gport_producer_unlock(p,_n);

        gport_consumer_lock(p,_n);
        gport_consumer_unlock(p,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= _n;

    gport_destroy(p);
}

// 
void benchmark_gport_n1     GPORT_BENCH_API(1)
void benchmark_gport_n4     GPORT_BENCH_API(4)
void benchmark_gport_n16    GPORT_BENCH_API(16)
void benchmark_gport_n64    GPORT_BENCH_API(64)

#endif // __LIQUID_GPORT_BENCHMARK_H__

