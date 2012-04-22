/*
 * Copyright (c) 2007, 2009, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2012 Virginia Polytechnic Institute & State University
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
// fft_radix2_benchmark.c : benchmark FFTs of prime length
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include "liquid.h"

#define FFT_RADIX2_BENCH_API(NFFT,D)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fft_radix2_bench(_start, _finish, _num_iterations, NFFT, D); }

// Helper function to keep code base small
void fft_radix2_bench(struct rusage *     _start,
                      struct rusage *     _finish,
                      unsigned long int * _num_iterations,
                      unsigned int        _nfft,
                      int                 _direction)
{
    // initialize arrays, plan
    float complex * x = (float complex *) malloc(_nfft*sizeof(float complex));
    float complex * y = (float complex *) malloc(_nfft*sizeof(float complex));
    int _method = 0;
    fftplan q = fft_create_plan(_nfft, x, y, _direction, _method);
    
    unsigned long int i;

    // initialize input with random values
    for (i=0; i<_nfft; i++)
        x[i] = randnf() + randnf()*_Complex_I;

    // scale number of iterations to keep execution time
    // relatively linear
    *_num_iterations /= _nfft;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fft_execute(q);
        fft_execute(q);
        fft_execute(q);
        fft_execute(q);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    fft_destroy_plan(q);
    free(x);
    free(y);
}


// Radix-2
void benchmark_fft_2      FFT_RADIX2_BENCH_API(2,       FFT_FORWARD)
void benchmark_fft_4      FFT_RADIX2_BENCH_API(4,       FFT_FORWARD)
void benchmark_fft_8      FFT_RADIX2_BENCH_API(8,       FFT_FORWARD)
void benchmark_fft_16     FFT_RADIX2_BENCH_API(16,      FFT_FORWARD)
void benchmark_fft_32     FFT_RADIX2_BENCH_API(32,      FFT_FORWARD)
void benchmark_fft_64     FFT_RADIX2_BENCH_API(64,      FFT_FORWARD)
void benchmark_fft_128    FFT_RADIX2_BENCH_API(128,     FFT_FORWARD)
void benchmark_fft_256    FFT_RADIX2_BENCH_API(256,     FFT_FORWARD)
void benchmark_fft_512    FFT_RADIX2_BENCH_API(512,     FFT_FORWARD)
void benchmark_fft_1024   FFT_RADIX2_BENCH_API(1024,    FFT_FORWARD)
void benchmark_fft_2048   FFT_RADIX2_BENCH_API(2048,    FFT_FORWARD)
void benchmark_fft_4096   FFT_RADIX2_BENCH_API(4096,    FFT_FORWARD)
void benchmark_fft_8192   FFT_RADIX2_BENCH_API(8192,    FFT_FORWARD)
void benchmark_fft_16384  FFT_RADIX2_BENCH_API(16384,   FFT_FORWARD)
void benchmark_fft_32768  FFT_RADIX2_BENCH_API(32768,   FFT_FORWARD)

