#ifndef __LIQUID_FFT_BENCHMARK_H__
#define __LIQUID_FFT_BENCHMARK_H__

#include <sys/resource.h>
#include <string.h>
#include <complex.h>
#include "../src/fft.h"

#define FFT_BENCH_API(N,D) \
(   struct rusage *_start, \
    struct rusage *_finish, \
    unsigned long int *_num_iterations) \
{ fft_bench(_start, _finish, _num_iterations, N, D); }

// Helper function to keep code base small
void fft_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n,
    int _direction)
{
    // initialize arrays, plan
    float complex x[_n], y[_n];
    memset(x, 0, _n*sizeof(float complex));
    fftplan p = fft_create_plan(_n, x, y, _direction);
    
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fft_execute(p);
        //fft_execute(p);
        //fft_execute(p);
        //fft_execute(p);
    }
    getrusage(RUSAGE_SELF, _finish);
    //*_num_iterations *= 4;

    fft_destroy_plan(p);
}

// Radix-2
void benchmark_fft_4        FFT_BENCH_API(4,        FFT_FORWARD)
void benchmark_fft_8        FFT_BENCH_API(8,        FFT_FORWARD)
void benchmark_fft_16       FFT_BENCH_API(16,       FFT_FORWARD)
void benchmark_fft_32       FFT_BENCH_API(32,       FFT_FORWARD)
//void xbenchmark_fft_64       FFT_BENCH_API(64,       FFT_FORWARD)
//void xbenchmark_fft_128      FFT_BENCH_API(128,      FFT_FORWARD)

// Non radix-2
void benchmark_fft_10       FFT_BENCH_API(10,       FFT_FORWARD)
//void xbenchmark_fft_100      FFT_BENCH_API(100,      FFT_FORWARD)

#endif // __LIQUID_FFT_BENCHMARK_H__

