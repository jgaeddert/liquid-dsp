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

#include <sys/resource.h>
#include "liquid.h"

#define MDCT_BENCH_API(N,D)             \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ mdct_bench(_start, _finish, _num_iterations, N, D); }

// Helper function to keep code base small
void mdct_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n,
    int _direction)
{
    // scale number of iterations to keep execution time
    // relatively linear
    *_num_iterations /= (_n*_n)/4;
    if (*_num_iterations < 10) *_num_iterations = 10;

    // initialize arrays, plan
    float x[2*_n];
    float y[_n];
    
    unsigned long int i;

    // initialize input with random values
    for (i=0; i<2*_n; i++)
        x[i] = randnf();

    // initialize window
    float w[2*_n];
    liquid_kbd_window(2*_n,10.0f,w);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        mdct(x, y, w, _n);
        mdct(x, y, w, _n);
        mdct(x, y, w, _n);
        mdct(x, y, w, _n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// Radix-2
void benchmark_mdct_4        MDCT_BENCH_API(4,        FFT_FORWARD)
void benchmark_mdct_8        MDCT_BENCH_API(8,        FFT_FORWARD)
void benchmark_mdct_16       MDCT_BENCH_API(16,       FFT_FORWARD)
void benchmark_mdct_32       MDCT_BENCH_API(32,       FFT_FORWARD)
void benchmark_mdct_64       MDCT_BENCH_API(64,       FFT_FORWARD)
void benchmark_mdct_128      MDCT_BENCH_API(128,      FFT_FORWARD)

// Non radix-2
void benchmark_mdct_10       MDCT_BENCH_API(10,       FFT_FORWARD)
//void xbenchmark_mdct_100      MDCT_BENCH_API(100,      FFT_FORWARD)

