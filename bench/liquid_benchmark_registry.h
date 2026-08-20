#ifndef __LIQUID_BENCHMARK_REGISTRY_H__
#define __LIQUID_BENCHMARK_REGISTRY_H__

#include "liquid.benchmark.h"

// ./src/filter/bench/fftfilt_crcf_benchmark.c
extern struct liquid_benchmark_s fftfilt_crcf_4_s;
extern struct liquid_benchmark_s fftfilt_crcf_8_s;
extern struct liquid_benchmark_s fftfilt_crcf_16_s;
extern struct liquid_benchmark_s fftfilt_crcf_32_s;
extern struct liquid_benchmark_s fftfilt_crcf_64_s;
// ./src/filter/bench/firdecim_crcf_benchmark.c
extern struct liquid_benchmark_s firdecim_crcf_m2_h8_s;
extern struct liquid_benchmark_s firdecim_crcf_m4_h16_s;
extern struct liquid_benchmark_s firdecim_crcf_m8_h32_s;
extern struct liquid_benchmark_s firdecim_crcf_m16_h64_s;
extern struct liquid_benchmark_s firdecim_crcf_m32_h128_s;
// ./src/filter/bench/firfilt_crcf_benchmark.c
extern struct liquid_benchmark_s firfilt_crcf_4_s;
extern struct liquid_benchmark_s firfilt_crcf_8_s;
extern struct liquid_benchmark_s firfilt_crcf_16_s;
extern struct liquid_benchmark_s firfilt_crcf_32_s;
extern struct liquid_benchmark_s firfilt_crcf_64_s;

// compile benchmark registry
liquid_benchmark liquid_benchmarks[] =
{
    &fftfilt_crcf_4_s,
    &fftfilt_crcf_8_s,
    &fftfilt_crcf_16_s,
    &fftfilt_crcf_32_s,
    &fftfilt_crcf_64_s,
    &firdecim_crcf_m2_h8_s,
    &firdecim_crcf_m4_h16_s,
    &firdecim_crcf_m8_h32_s,
    &firdecim_crcf_m16_h64_s,
    &firdecim_crcf_m32_h128_s,
    &firfilt_crcf_4_s,
    &firfilt_crcf_8_s,
    &firfilt_crcf_16_s,
    &firfilt_crcf_32_s,
    &firfilt_crcf_64_s,
    NULL
};

#endif // __LIQUID_BENCHMARK_REGISTRY_H__

