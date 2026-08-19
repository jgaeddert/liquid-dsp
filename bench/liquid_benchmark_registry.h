#ifndef __LIQUID_BENCHMARK_REGISTRY_H__
#define __LIQUID_BENCHMARK_REGISTRY_H__

#include "liquid.benchmark.h"

// ./src/filter/bench/firfilt_crcf_benchmark.c
extern struct liquid_benchmark_s firfilt_crcf_4_s;
extern struct liquid_benchmark_s firfilt_crcf_8_s;
extern struct liquid_benchmark_s firfilt_crcf_16_s;
extern struct liquid_benchmark_s firfilt_crcf_32_s;
extern struct liquid_benchmark_s firfilt_crcf_64_s;

// compile benchmark registry
liquid_benchmark liquid_benchmarks[] =
{
    &firfilt_crcf_4_s,
    &firfilt_crcf_8_s,
    &firfilt_crcf_16_s,
    &firfilt_crcf_32_s,
    &firfilt_crcf_64_s,
    NULL
};

#endif // __LIQUID_BENCHMARK_REGISTRY_H__

