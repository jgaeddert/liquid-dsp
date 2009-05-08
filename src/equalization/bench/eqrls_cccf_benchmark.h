#ifndef __LIQUID_EQRLS_CCCF_BENCHMARK_H__
#define __LIQUID_EQRLS_CCCF_BENCHMARK_H__

#include <sys/resource.h>
#include <math.h>
#include "liquid.h"

#define EQRLS_CCCF_TRAIN_BENCH_API(N) \
(   struct rusage *_start, \
    struct rusage *_finish, \
    unsigned long int *_num_iterations) \
{ eqrls_cccf_train_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void eqrls_cccf_train_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _h_len)
{
    eqrls_cccf eq = eqrls_cccf_create(_h_len);
    
    unsigned long int i;

    float complex y;
    float complex d;
    float complex z;

    // reduce number of iterations relative to speed (keeps execution
    // time from exploding)
    *_num_iterations /= _h_len * logf(_h_len);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // periodically initializing the input and training sequence
        // to a random number helps prevent the RLS algorithm's
        // internal matrices don't explode, and adds negligible time
        // to the benchmark
        crandnf(&y);
        crandnf(&d);

        eqrls_cccf_execute(eq, y, d, &z);
        eqrls_cccf_execute(eq, y, d, &z);
        eqrls_cccf_execute(eq, y, d, &z);
        eqrls_cccf_execute(eq, y, d, &z);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    eqrls_cccf_destroy(eq);
}

// 
void benchmark_eqrls_cccf_n4    EQRLS_CCCF_TRAIN_BENCH_API(4)
void benchmark_eqrls_cccf_n8    EQRLS_CCCF_TRAIN_BENCH_API(8)
void benchmark_eqrls_cccf_n16   EQRLS_CCCF_TRAIN_BENCH_API(16)
void benchmark_eqrls_cccf_n32   EQRLS_CCCF_TRAIN_BENCH_API(32)
void benchmark_eqrls_cccf_n64   EQRLS_CCCF_TRAIN_BENCH_API(64)

#endif // __LIQUID_EQRLS_CCCF_BENCHMARK_H__

