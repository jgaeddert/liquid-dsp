#ifndef __AGC_BENCHMARK_H__
#define __AGC_BENCHMARK_H__

#include <sys/resource.h>
#include "../src/agc.h"

void benchmark_agc(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned int i;

    // initialize AGC object
    agc g = agc_create(1.0f, 0.05f);

    float complex x=1.0f, y;

    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        agc_execute(g, x, &y);
        agc_execute(g, x, &y);
        agc_execute(g, x, &y);
        agc_execute(g, x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);

    *_num_iterations *= 4;

    agc_destroy(g);
}

#endif // __AGC_BENCHMARK_H__

