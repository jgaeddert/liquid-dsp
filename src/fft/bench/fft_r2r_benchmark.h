/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// fft_r2r_benchmark.h
//
// Real even/odd FFT benchmarks (discrete cosine/sine transforms)
//

#ifndef __LIQUID_FFT_R2R_BENCHMARK_H__
#define __LIQUID_FFT_R2R_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define FFT_R2R_BENCH_API(N,K)          \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fft_r2r_bench(_start, _finish, _num_iterations, N, K); }

// Helper function to keep code base small
void fft_r2r_bench(struct rusage *_start,
                   struct rusage *_finish,
                   unsigned long int *_num_iterations,
                   unsigned int _n,
                   int _kind)
{
    // initialize arrays, plan
    float x[_n], y[_n];
    int _flags = 0;
    fftplan p = fft_create_plan_r2r_1d(_n, x, y, _kind, _flags);
    
    unsigned long int i;

    // initialize input with random values
    for (i=0; i<_n; i++)
        x[i] = randnf();

    // scale number of iterations to keep execution time
    // relatively linear
    *_num_iterations /= _n * _n;
    *_num_iterations *= 10;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fft_execute(p);
        fft_execute(p);
        fft_execute(p);
        fft_execute(p);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    fft_destroy_plan(p);
}

// Radix-2
void benchmark_fft_r2r_4        FFT_R2R_BENCH_API(4,        FFT_REDFT10)
void benchmark_fft_r2r_8        FFT_R2R_BENCH_API(8,        FFT_REDFT10)
void benchmark_fft_r2r_16       FFT_R2R_BENCH_API(16,       FFT_REDFT10)
void benchmark_fft_r2r_32       FFT_R2R_BENCH_API(32,       FFT_REDFT10)
void benchmark_fft_r2r_64       FFT_R2R_BENCH_API(64,       FFT_REDFT10)
void benchmark_fft_r2r_128      FFT_R2R_BENCH_API(128,      FFT_REDFT10)

// Non radix-2
void benchmark_fft_r2r_10       FFT_R2R_BENCH_API(10,       FFT_REDFT10)
//void xbenchmark_fft_r2r_100      FFT_R2R_BENCH_API(100,      FFT_REDFT10)

#endif // __LIQUID_FFT_R2R_BENCHMARK_H__

