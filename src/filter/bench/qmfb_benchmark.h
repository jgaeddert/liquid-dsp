#ifndef __LIQUID_QMFB_BENCHMARK_H__
#define __LIQUID_QMFB_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void qmfb_crcf_synthesis_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _m)
{
    qmfb_crcf q = qmfb_crcf_create(_m, -60.0f);

    float complex x0 = 1.0f + 1.0f*_Complex_I;
    float complex x1 = 1.0f - 1.0f*_Complex_I;
    float complex y0;
    float complex y1;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        qmfb_crcf_synthesis_execute(q,x0,x1,&y0,&y1);
        qmfb_crcf_synthesis_execute(q,x0,x1,&y0,&y1);
        qmfb_crcf_synthesis_execute(q,x0,x1,&y0,&y1);
        qmfb_crcf_synthesis_execute(q,x0,x1,&y0,&y1);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    qmfb_crcf_destroy(q);
}

#define QMFB_SYNTHESIS_BENCHMARK_API(M)     \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ qmfb_crcf_synthesis_bench(_start, _finish, _num_iterations, M); }

void benchmark_qmfb_crcf_synth_m3   QMFB_SYNTHESIS_BENCHMARK_API(3)
void benchmark_qmfb_crcf_synth_m5   QMFB_SYNTHESIS_BENCHMARK_API(5)
void benchmark_qmfb_crcf_synth_m9   QMFB_SYNTHESIS_BENCHMARK_API(9)
void benchmark_qmfb_crcf_synth_m13  QMFB_SYNTHESIS_BENCHMARK_API(13)

#endif // __LIQUID_QMFB_BENCHMARK_H__

