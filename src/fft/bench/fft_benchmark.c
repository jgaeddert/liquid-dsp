/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#include <sys/resource.h>
#include "liquid.h"

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
    int _method = 0;
    fftplan p = fft_create_plan(_n, x, y, _direction, _method);
    
    unsigned long int i;

    // initialize input with random values
    for (i=0; i<_n; i++)
        x[i] = randnf() + randnf()*_Complex_I;

    // scale number of iterations to keep execution time
    // relatively linear
    *_num_iterations /= _n;

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
void benchmark_fft_4        FFT_BENCH_API(4,        FFT_FORWARD)
void benchmark_fft_8        FFT_BENCH_API(8,        FFT_FORWARD)
void benchmark_fft_16       FFT_BENCH_API(16,       FFT_FORWARD)
void benchmark_fft_32       FFT_BENCH_API(32,       FFT_FORWARD)
void benchmark_fft_64       FFT_BENCH_API(64,       FFT_FORWARD)
void benchmark_fft_128      FFT_BENCH_API(128,      FFT_FORWARD)

// Non radix-2
void benchmark_fft_10       FFT_BENCH_API(10,       FFT_FORWARD)
//void xbenchmark_fft_100      FFT_BENCH_API(100,      FFT_FORWARD)

