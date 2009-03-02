#ifndef __LIQUID_QUANTIZER_BENCHMARK_H__
#define __LIQUID_QUANTIZER_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// 
void benchmark_quantize_adc(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;

    unsigned int q;
    unsigned int num_bits=8;
    float x=-0.1f;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q = quantize_adc(x,num_bits);
        q = quantize_adc(x,num_bits);
        q = quantize_adc(x,num_bits);
        q = quantize_adc(x,num_bits);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
void benchmark_quantize_dac(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations)
{
    unsigned long int i;

    unsigned int q=0x0f;
    unsigned int num_bits=8;
    float x;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        x = quantize_dac(q,num_bits);
        x = quantize_dac(q,num_bits);
        x = quantize_dac(q,num_bits);
        x = quantize_dac(q,num_bits);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

#endif // __LIQUID_QUANTIZER_BENCHMARK_H__

