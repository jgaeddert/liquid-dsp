#ifndef __LIQUID_BENCHMARK_REGISTRY_H__
#define __LIQUID_BENCHMARK_REGISTRY_H__

#include "liquid.benchmark.h"

// ./src/agc/bench/agc_crcf_benchmark.c
extern struct liquid_benchmark_s agc_crcf_s;
// ./src/audio/bench/cvsd_benchmark.c
extern struct liquid_benchmark_s cvsd_encode_s;
extern struct liquid_benchmark_s cvsd_decode_s;
// ./src/buffer/bench/cbuffercf_benchmark.c
extern struct liquid_benchmark_s cbuffercf_n16_s;
extern struct liquid_benchmark_s cbuffercf_n32_s;
extern struct liquid_benchmark_s cbuffercf_n64_s;
extern struct liquid_benchmark_s cbuffercf_n128_s;
extern struct liquid_benchmark_s cbuffercf_n256_s;
extern struct liquid_benchmark_s cbuffercf_n512_s;
extern struct liquid_benchmark_s cbuffercf_n1024_s;
// ./src/buffer/bench/window_push_benchmark.c
extern struct liquid_benchmark_s windowcf_push_n16_s;
extern struct liquid_benchmark_s windowcf_push_n32_s;
extern struct liquid_benchmark_s windowcf_push_n64_s;
extern struct liquid_benchmark_s windowcf_push_n128_s;
extern struct liquid_benchmark_s windowcf_push_n256_s;
// ./src/buffer/bench/window_read_benchmark.c
extern struct liquid_benchmark_s windowcf_read_n16_s;
extern struct liquid_benchmark_s windowcf_read_n32_s;
extern struct liquid_benchmark_s windowcf_read_n64_s;
extern struct liquid_benchmark_s windowcf_read_n128_s;
extern struct liquid_benchmark_s windowcf_read_n256_s;
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
// ./src/filter/bench/firhilb_benchmark.c
extern struct liquid_benchmark_s firhilbf_decim_m3_s;
extern struct liquid_benchmark_s firhilbf_decim_m5_s;
extern struct liquid_benchmark_s firhilbf_decim_m9_s;
extern struct liquid_benchmark_s firhilbf_decim_m13_s;
// ./src/filter/bench/firinterp_crcf_benchmark.c
extern struct liquid_benchmark_s firinterp_crcf_m2_h8_s;
extern struct liquid_benchmark_s firinterp_crcf_m4_h16_s;
extern struct liquid_benchmark_s firinterp_crcf_m8_h32_s;
extern struct liquid_benchmark_s firinterp_crcf_m16_h64_s;
extern struct liquid_benchmark_s firinterp_crcf_m32_h128_s;
// ./src/filter/bench/iirdecim_crcf_benchmark.c
extern struct liquid_benchmark_s iirdecim_crcf_M2_s;
extern struct liquid_benchmark_s iirdecim_crcf_M4_s;
extern struct liquid_benchmark_s iirdecim_crcf_M8_s;
extern struct liquid_benchmark_s iirdecim_crcf_M16_s;
extern struct liquid_benchmark_s iirdecim_crcf_M32_s;
// ./src/filter/bench/iirfilt_crcf_benchmark.c
extern struct liquid_benchmark_s iirfilt_crcf_4_s;
extern struct liquid_benchmark_s iirfilt_crcf_8_s;
extern struct liquid_benchmark_s iirfilt_crcf_16_s;
extern struct liquid_benchmark_s iirfilt_crcf_32_s;
extern struct liquid_benchmark_s iirfilt_crcf_64_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_4_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_8_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_16_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_32_s;
extern struct liquid_benchmark_s iirfilt_crcf_sos_64_s;
extern struct liquid_benchmark_s iirfilt_crcf_dcblock_s;
// ./src/filter/bench/iirinterp_crcf_benchmark.c
extern struct liquid_benchmark_s iirinterp_crcf_M2_s;
extern struct liquid_benchmark_s iirinterp_crcf_M4_s;
extern struct liquid_benchmark_s iirinterp_crcf_M8_s;
extern struct liquid_benchmark_s iirinterp_crcf_M16_s;
extern struct liquid_benchmark_s iirinterp_crcf_M32_s;
// ./src/filter/bench/resamp2_crcf_benchmark.c
extern struct liquid_benchmark_s resamp2_crcf_decim_m2_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m4_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m8_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m16_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m32_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m64_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m128_s;
extern struct liquid_benchmark_s resamp2_crcf_decim_m256_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m2_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m4_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m8_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m16_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m32_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m64_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m128_s;
extern struct liquid_benchmark_s resamp2_crcf_interp_m256_s;
// ./src/filter/bench/resamp_crcf_benchmark.c
extern struct liquid_benchmark_s resamp_crcf_P17_Q1_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q2_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q4_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q8_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q16_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q32_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q64_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q128_s;
extern struct liquid_benchmark_s resamp_crcf_P17_Q256_s;
// ./src/filter/bench/rresamp_crcf_benchmark.c
extern struct liquid_benchmark_s rresamp_crcf_P17_Q1_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q2_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q4_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q8_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q16_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q32_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q64_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q128_s;
extern struct liquid_benchmark_s rresamp_crcf_P17_Q256_s;
// ./src/filter/bench/symsync_crcf_benchmark.c
extern struct liquid_benchmark_s symsync_crcf_k2_m2_s;
extern struct liquid_benchmark_s symsync_crcf_k2_m4_s;
extern struct liquid_benchmark_s symsync_crcf_k2_m8_s;
extern struct liquid_benchmark_s symsync_crcf_k2_m16_s;

// compile benchmark registry
liquid_benchmark liquid_benchmarks[] =
{
    &agc_crcf_s,
    &cvsd_encode_s,
    &cvsd_decode_s,
    &cbuffercf_n16_s,
    &cbuffercf_n32_s,
    &cbuffercf_n64_s,
    &cbuffercf_n128_s,
    &cbuffercf_n256_s,
    &cbuffercf_n512_s,
    &cbuffercf_n1024_s,
    &windowcf_push_n16_s,
    &windowcf_push_n32_s,
    &windowcf_push_n64_s,
    &windowcf_push_n128_s,
    &windowcf_push_n256_s,
    &windowcf_read_n16_s,
    &windowcf_read_n32_s,
    &windowcf_read_n64_s,
    &windowcf_read_n128_s,
    &windowcf_read_n256_s,
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
    &firhilbf_decim_m3_s,
    &firhilbf_decim_m5_s,
    &firhilbf_decim_m9_s,
    &firhilbf_decim_m13_s,
    &firinterp_crcf_m2_h8_s,
    &firinterp_crcf_m4_h16_s,
    &firinterp_crcf_m8_h32_s,
    &firinterp_crcf_m16_h64_s,
    &firinterp_crcf_m32_h128_s,
    &iirdecim_crcf_M2_s,
    &iirdecim_crcf_M4_s,
    &iirdecim_crcf_M8_s,
    &iirdecim_crcf_M16_s,
    &iirdecim_crcf_M32_s,
    &iirfilt_crcf_4_s,
    &iirfilt_crcf_8_s,
    &iirfilt_crcf_16_s,
    &iirfilt_crcf_32_s,
    &iirfilt_crcf_64_s,
    &iirfilt_crcf_sos_4_s,
    &iirfilt_crcf_sos_8_s,
    &iirfilt_crcf_sos_16_s,
    &iirfilt_crcf_sos_32_s,
    &iirfilt_crcf_sos_64_s,
    &iirfilt_crcf_dcblock_s,
    &iirinterp_crcf_M2_s,
    &iirinterp_crcf_M4_s,
    &iirinterp_crcf_M8_s,
    &iirinterp_crcf_M16_s,
    &iirinterp_crcf_M32_s,
    &resamp2_crcf_decim_m2_s,
    &resamp2_crcf_decim_m4_s,
    &resamp2_crcf_decim_m8_s,
    &resamp2_crcf_decim_m16_s,
    &resamp2_crcf_decim_m32_s,
    &resamp2_crcf_decim_m64_s,
    &resamp2_crcf_decim_m128_s,
    &resamp2_crcf_decim_m256_s,
    &resamp2_crcf_interp_m2_s,
    &resamp2_crcf_interp_m4_s,
    &resamp2_crcf_interp_m8_s,
    &resamp2_crcf_interp_m16_s,
    &resamp2_crcf_interp_m32_s,
    &resamp2_crcf_interp_m64_s,
    &resamp2_crcf_interp_m128_s,
    &resamp2_crcf_interp_m256_s,
    &resamp_crcf_P17_Q1_s,
    &resamp_crcf_P17_Q2_s,
    &resamp_crcf_P17_Q4_s,
    &resamp_crcf_P17_Q8_s,
    &resamp_crcf_P17_Q16_s,
    &resamp_crcf_P17_Q32_s,
    &resamp_crcf_P17_Q64_s,
    &resamp_crcf_P17_Q128_s,
    &resamp_crcf_P17_Q256_s,
    &rresamp_crcf_P17_Q1_s,
    &rresamp_crcf_P17_Q2_s,
    &rresamp_crcf_P17_Q4_s,
    &rresamp_crcf_P17_Q8_s,
    &rresamp_crcf_P17_Q16_s,
    &rresamp_crcf_P17_Q32_s,
    &rresamp_crcf_P17_Q64_s,
    &rresamp_crcf_P17_Q128_s,
    &rresamp_crcf_P17_Q256_s,
    &symsync_crcf_k2_m2_s,
    &symsync_crcf_k2_m4_s,
    &symsync_crcf_k2_m8_s,
    &symsync_crcf_k2_m16_s,
    NULL
};

#endif // __LIQUID_BENCHMARK_REGISTRY_H__

