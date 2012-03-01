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
void benchmark_fft_256      FFT_BENCH_API(256,      FFT_FORWARD)
void benchmark_fft_512      FFT_BENCH_API(512,      FFT_FORWARD)

// Non radix-2
void benchmark_fft_10   FFT_BENCH_API(  10, FFT_FORWARD)
void benchmark_fft_11   FFT_BENCH_API(  11, FFT_FORWARD)
void benchmark_fft_12   FFT_BENCH_API(  12, FFT_FORWARD)
void benchmark_fft_13   FFT_BENCH_API(  13, FFT_FORWARD)
void benchmark_fft_15   FFT_BENCH_API(  15, FFT_FORWARD)
void benchmark_fft_18   FFT_BENCH_API(  18, FFT_FORWARD)
void benchmark_fft_19   FFT_BENCH_API(  19, FFT_FORWARD)
void benchmark_fft_21   FFT_BENCH_API(  21, FFT_FORWARD)
void benchmark_fft_24   FFT_BENCH_API(  24, FFT_FORWARD)
void benchmark_fft_26   FFT_BENCH_API(  26, FFT_FORWARD)
void benchmark_fft_29   FFT_BENCH_API(  29, FFT_FORWARD)
void benchmark_fft_31   FFT_BENCH_API(  31, FFT_FORWARD)
void benchmark_fft_35   FFT_BENCH_API(  35, FFT_FORWARD)
void benchmark_fft_38   FFT_BENCH_API(  38, FFT_FORWARD)
void benchmark_fft_42   FFT_BENCH_API(  42, FFT_FORWARD)
void benchmark_fft_46   FFT_BENCH_API(  46, FFT_FORWARD)
void benchmark_fft_51   FFT_BENCH_API(  51, FFT_FORWARD)
void benchmark_fft_56   FFT_BENCH_API(  56, FFT_FORWARD)
void benchmark_fft_61   FFT_BENCH_API(  61, FFT_FORWARD)
void benchmark_fft_67   FFT_BENCH_API(  67, FFT_FORWARD)
void benchmark_fft_74   FFT_BENCH_API(  74, FFT_FORWARD)
void benchmark_fft_81   FFT_BENCH_API(  81, FFT_FORWARD)
void benchmark_fft_90   FFT_BENCH_API(  90, FFT_FORWARD)
void benchmark_fft_98   FFT_BENCH_API(  98, FFT_FORWARD)
void benchmark_fft_108  FFT_BENCH_API( 108, FFT_FORWARD)
void benchmark_fft_119  FFT_BENCH_API( 119, FFT_FORWARD)
void benchmark_fft_131  FFT_BENCH_API( 131, FFT_FORWARD)
void benchmark_fft_144  FFT_BENCH_API( 144, FFT_FORWARD)
void benchmark_fft_159  FFT_BENCH_API( 159, FFT_FORWARD)
void benchmark_fft_174  FFT_BENCH_API( 174, FFT_FORWARD)
void benchmark_fft_192  FFT_BENCH_API( 192, FFT_FORWARD)
void benchmark_fft_211  FFT_BENCH_API( 211, FFT_FORWARD)
void benchmark_fft_232  FFT_BENCH_API( 232, FFT_FORWARD)
void benchmark_fft_255  FFT_BENCH_API( 255, FFT_FORWARD)
void benchmark_fft_309  FFT_BENCH_API( 309, FFT_FORWARD)
void benchmark_fft_340  FFT_BENCH_API( 340, FFT_FORWARD)
void benchmark_fft_374  FFT_BENCH_API( 374, FFT_FORWARD)
void benchmark_fft_411  FFT_BENCH_API( 411, FFT_FORWARD)
void benchmark_fft_453  FFT_BENCH_API( 453, FFT_FORWARD)
void benchmark_fft_498  FFT_BENCH_API( 498, FFT_FORWARD)
void benchmark_fft_548  FFT_BENCH_API( 548, FFT_FORWARD)

// prime numbers
void benchmark_fft_263  FFT_BENCH_API( 263, FFT_FORWARD)
void benchmark_fft_269  FFT_BENCH_API( 269, FFT_FORWARD)
void benchmark_fft_271  FFT_BENCH_API( 271, FFT_FORWARD)
void benchmark_fft_277  FFT_BENCH_API( 277, FFT_FORWARD)
void benchmark_fft_281  FFT_BENCH_API( 281, FFT_FORWARD)
void benchmark_fft_283  FFT_BENCH_API( 283, FFT_FORWARD)
void benchmark_fft_293  FFT_BENCH_API( 293, FFT_FORWARD)
void benchmark_fft_307  FFT_BENCH_API( 307, FFT_FORWARD)
void benchmark_fft_311  FFT_BENCH_API( 311, FFT_FORWARD)
void benchmark_fft_313  FFT_BENCH_API( 313, FFT_FORWARD)
void benchmark_fft_317  FFT_BENCH_API( 317, FFT_FORWARD)
void benchmark_fft_331  FFT_BENCH_API( 331, FFT_FORWARD)
void benchmark_fft_337  FFT_BENCH_API( 337, FFT_FORWARD)
void benchmark_fft_347  FFT_BENCH_API( 347, FFT_FORWARD)
void benchmark_fft_349  FFT_BENCH_API( 349, FFT_FORWARD)
void benchmark_fft_353  FFT_BENCH_API( 353, FFT_FORWARD)
void benchmark_fft_359  FFT_BENCH_API( 359, FFT_FORWARD)
void benchmark_fft_367  FFT_BENCH_API( 367, FFT_FORWARD)
void benchmark_fft_373  FFT_BENCH_API( 373, FFT_FORWARD)
void benchmark_fft_379  FFT_BENCH_API( 379, FFT_FORWARD)
void benchmark_fft_383  FFT_BENCH_API( 383, FFT_FORWARD)
void benchmark_fft_389  FFT_BENCH_API( 389, FFT_FORWARD)
void benchmark_fft_397  FFT_BENCH_API( 397, FFT_FORWARD)
void benchmark_fft_401  FFT_BENCH_API( 401, FFT_FORWARD)
void benchmark_fft_409  FFT_BENCH_API( 409, FFT_FORWARD)
void benchmark_fft_419  FFT_BENCH_API( 419, FFT_FORWARD)
void benchmark_fft_421  FFT_BENCH_API( 421, FFT_FORWARD)
void benchmark_fft_431  FFT_BENCH_API( 431, FFT_FORWARD)
void benchmark_fft_433  FFT_BENCH_API( 433, FFT_FORWARD)
void benchmark_fft_439  FFT_BENCH_API( 439, FFT_FORWARD)
void benchmark_fft_443  FFT_BENCH_API( 443, FFT_FORWARD)
void benchmark_fft_449  FFT_BENCH_API( 449, FFT_FORWARD)
void benchmark_fft_457  FFT_BENCH_API( 457, FFT_FORWARD)
void benchmark_fft_461  FFT_BENCH_API( 461, FFT_FORWARD)
void benchmark_fft_463  FFT_BENCH_API( 463, FFT_FORWARD)
void benchmark_fft_467  FFT_BENCH_API( 467, FFT_FORWARD)
void benchmark_fft_479  FFT_BENCH_API( 479, FFT_FORWARD)
void benchmark_fft_487  FFT_BENCH_API( 487, FFT_FORWARD)
void benchmark_fft_491  FFT_BENCH_API( 491, FFT_FORWARD)
void benchmark_fft_499  FFT_BENCH_API( 499, FFT_FORWARD)
void benchmark_fft_503  FFT_BENCH_API( 503, FFT_FORWARD)
void benchmark_fft_509  FFT_BENCH_API( 509, FFT_FORWARD)

