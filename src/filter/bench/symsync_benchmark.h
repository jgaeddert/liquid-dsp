#ifndef __LIQUID_SYMSYNC_BENCHMARK_H__
#define __LIQUID_SYMSYNC_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void symsync_crcf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _h_len)
{
    unsigned int i;
    unsigned int num_filters=16;
    unsigned int k=2;
    unsigned int m=3;
    float beta=0.3f;

    // design polyphase filter
    unsigned int H_len = 2*num_filters*k*m + 1;
    float H[H_len];
    design_rrc_filter(k*num_filters,m,beta,0,H);

    // create symbol synchronizer
    symsync_crcf q = symsync_crcf_create(k, num_filters, H, H_len);

    unsigned int num_samples=k;
    unsigned int num_written;
    float complex x[num_samples];
    float complex y[num_samples];

    for (i=0; i<num_samples; i++)
        x[i] = 0.0f;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        symsync_crcf_execute(q,x,num_samples,y,&num_written);
        symsync_crcf_execute(q,x,num_samples,y,&num_written);
        symsync_crcf_execute(q,x,num_samples,y,&num_written);
        symsync_crcf_execute(q,x,num_samples,y,&num_written);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    symsync_crcf_destroy(q);
}

#define SYMSYNC_CRCF_BENCHMARK_API(H_LEN)   \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ symsync_crcf_bench(_start, _finish, _num_iterations, H_LEN); }

// 
// Interpolators
//
void benchmark_symsync_crcf SYMSYNC_CRCF_BENCHMARK_API(13)   // m=3

#endif // __LIQUID_SYMSYNC_BENCHMARK_H__

