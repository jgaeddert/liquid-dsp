#ifndef __LIQUID_RANDOM_BENCHMARK_H__
#define __LIQUID_RANDOM_BENCHMARK_H__

#include <sys/resource.h>
#include "../src/random.h"

// 
// BENCHMARK: uniform
//
void benchmark_random_uniform(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    float x;
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        x = randf();
        x = randf();
        x = randf();
        x = randf();
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: normal
//
void benchmark_random_normal(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    float x, y;
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        randnf(&x, &y);
        randnf(&x, &y);
        randnf(&x, &y);
        randnf(&x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: Weibull
//
void benchmark_random_weibull(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    float x, alpha=1.0f, beta=2.0f, gamma=6.0f;
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        x = rand_weibullf(alpha,beta,gamma);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: Rice-K
//
void benchmark_random_ricek(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    float x, K=2.0f, omega=1.0f;
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        x = rand_ricekf(K,omega);
        x = rand_ricekf(K,omega);
        x = rand_ricekf(K,omega);
        x = rand_ricekf(K,omega);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}


#endif // __LIQUID_RANDOM_BENCHMARK_H__

