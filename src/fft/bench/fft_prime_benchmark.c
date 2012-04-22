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
// fft_prime_benchmark.c : benchmark FFTs of prime length
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include "liquid.h"

#define FFT_PRIME_BENCH_API(NFFT,D)     \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fft_prime_bench(_start, _finish, _num_iterations, NFFT, D); }

// Helper function to keep code base small
void fft_prime_bench(struct rusage *     _start,
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

// prime numbers
void benchmark_fft_3      FFT_PRIME_BENCH_API(     3, FFT_FORWARD)
void benchmark_fft_5      FFT_PRIME_BENCH_API(     5, FFT_FORWARD)
void benchmark_fft_7      FFT_PRIME_BENCH_API(     7, FFT_FORWARD)
void benchmark_fft_11     FFT_PRIME_BENCH_API(    11, FFT_FORWARD)
void benchmark_fft_13     FFT_PRIME_BENCH_API(    13, FFT_FORWARD)
void benchmark_fft_17     FFT_PRIME_BENCH_API(    17, FFT_FORWARD)
void benchmark_fft_19     FFT_PRIME_BENCH_API(    19, FFT_FORWARD)
void benchmark_fft_23     FFT_PRIME_BENCH_API(    23, FFT_FORWARD)
void benchmark_fft_29     FFT_PRIME_BENCH_API(    29, FFT_FORWARD)
void benchmark_fft_31     FFT_PRIME_BENCH_API(    31, FFT_FORWARD)
void benchmark_fft_37     FFT_PRIME_BENCH_API(    37, FFT_FORWARD)
void benchmark_fft_41     FFT_PRIME_BENCH_API(    41, FFT_FORWARD)
void benchmark_fft_43     FFT_PRIME_BENCH_API(    43, FFT_FORWARD)
void benchmark_fft_47     FFT_PRIME_BENCH_API(    47, FFT_FORWARD)
void benchmark_fft_53     FFT_PRIME_BENCH_API(    53, FFT_FORWARD)
void benchmark_fft_59     FFT_PRIME_BENCH_API(    59, FFT_FORWARD)
void benchmark_fft_61     FFT_PRIME_BENCH_API(    61, FFT_FORWARD)
void benchmark_fft_67     FFT_PRIME_BENCH_API(    67, FFT_FORWARD)
void benchmark_fft_71     FFT_PRIME_BENCH_API(    71, FFT_FORWARD)
void benchmark_fft_73     FFT_PRIME_BENCH_API(    73, FFT_FORWARD)
void benchmark_fft_79     FFT_PRIME_BENCH_API(    79, FFT_FORWARD)
void benchmark_fft_83     FFT_PRIME_BENCH_API(    83, FFT_FORWARD)
void benchmark_fft_89     FFT_PRIME_BENCH_API(    89, FFT_FORWARD)
void benchmark_fft_97     FFT_PRIME_BENCH_API(    97, FFT_FORWARD)
void benchmark_fft_101    FFT_PRIME_BENCH_API(   101, FFT_FORWARD)
void benchmark_fft_103    FFT_PRIME_BENCH_API(   103, FFT_FORWARD)
void benchmark_fft_107    FFT_PRIME_BENCH_API(   107, FFT_FORWARD)
void benchmark_fft_109    FFT_PRIME_BENCH_API(   109, FFT_FORWARD)
void benchmark_fft_113    FFT_PRIME_BENCH_API(   113, FFT_FORWARD)
void benchmark_fft_127    FFT_PRIME_BENCH_API(   127, FFT_FORWARD)
void benchmark_fft_131    FFT_PRIME_BENCH_API(   131, FFT_FORWARD)
void benchmark_fft_137    FFT_PRIME_BENCH_API(   137, FFT_FORWARD)
void benchmark_fft_139    FFT_PRIME_BENCH_API(   139, FFT_FORWARD)
void benchmark_fft_149    FFT_PRIME_BENCH_API(   149, FFT_FORWARD)
void benchmark_fft_151    FFT_PRIME_BENCH_API(   151, FFT_FORWARD)
void benchmark_fft_157    FFT_PRIME_BENCH_API(   157, FFT_FORWARD)
void benchmark_fft_163    FFT_PRIME_BENCH_API(   163, FFT_FORWARD)
void benchmark_fft_167    FFT_PRIME_BENCH_API(   167, FFT_FORWARD)
void benchmark_fft_173    FFT_PRIME_BENCH_API(   173, FFT_FORWARD)
void benchmark_fft_179    FFT_PRIME_BENCH_API(   179, FFT_FORWARD)
void benchmark_fft_181    FFT_PRIME_BENCH_API(   181, FFT_FORWARD)
void benchmark_fft_191    FFT_PRIME_BENCH_API(   191, FFT_FORWARD)
void benchmark_fft_193    FFT_PRIME_BENCH_API(   193, FFT_FORWARD)
void benchmark_fft_197    FFT_PRIME_BENCH_API(   197, FFT_FORWARD)
void benchmark_fft_199    FFT_PRIME_BENCH_API(   199, FFT_FORWARD)
void benchmark_fft_211    FFT_PRIME_BENCH_API(   211, FFT_FORWARD)
void benchmark_fft_223    FFT_PRIME_BENCH_API(   223, FFT_FORWARD)
void benchmark_fft_227    FFT_PRIME_BENCH_API(   227, FFT_FORWARD)
void benchmark_fft_229    FFT_PRIME_BENCH_API(   229, FFT_FORWARD)
void benchmark_fft_233    FFT_PRIME_BENCH_API(   233, FFT_FORWARD)
void benchmark_fft_239    FFT_PRIME_BENCH_API(   239, FFT_FORWARD)
void benchmark_fft_241    FFT_PRIME_BENCH_API(   241, FFT_FORWARD)
void benchmark_fft_251    FFT_PRIME_BENCH_API(   251, FFT_FORWARD)
void benchmark_fft_257    FFT_PRIME_BENCH_API(   257, FFT_FORWARD)
void benchmark_fft_263    FFT_PRIME_BENCH_API(   263, FFT_FORWARD)
void benchmark_fft_269    FFT_PRIME_BENCH_API(   269, FFT_FORWARD)
void benchmark_fft_271    FFT_PRIME_BENCH_API(   271, FFT_FORWARD)
void benchmark_fft_277    FFT_PRIME_BENCH_API(   277, FFT_FORWARD)
void benchmark_fft_281    FFT_PRIME_BENCH_API(   281, FFT_FORWARD)
void benchmark_fft_283    FFT_PRIME_BENCH_API(   283, FFT_FORWARD)
void benchmark_fft_293    FFT_PRIME_BENCH_API(   293, FFT_FORWARD)
void benchmark_fft_307    FFT_PRIME_BENCH_API(   307, FFT_FORWARD)
void benchmark_fft_311    FFT_PRIME_BENCH_API(   311, FFT_FORWARD)
void benchmark_fft_313    FFT_PRIME_BENCH_API(   313, FFT_FORWARD)
void benchmark_fft_317    FFT_PRIME_BENCH_API(   317, FFT_FORWARD)
void benchmark_fft_331    FFT_PRIME_BENCH_API(   331, FFT_FORWARD)
void benchmark_fft_337    FFT_PRIME_BENCH_API(   337, FFT_FORWARD)
void benchmark_fft_347    FFT_PRIME_BENCH_API(   347, FFT_FORWARD)
void benchmark_fft_349    FFT_PRIME_BENCH_API(   349, FFT_FORWARD)
void benchmark_fft_353    FFT_PRIME_BENCH_API(   353, FFT_FORWARD)
void benchmark_fft_359    FFT_PRIME_BENCH_API(   359, FFT_FORWARD)
void benchmark_fft_367    FFT_PRIME_BENCH_API(   367, FFT_FORWARD)
void benchmark_fft_373    FFT_PRIME_BENCH_API(   373, FFT_FORWARD)
void benchmark_fft_379    FFT_PRIME_BENCH_API(   379, FFT_FORWARD)
void benchmark_fft_383    FFT_PRIME_BENCH_API(   383, FFT_FORWARD)
void benchmark_fft_389    FFT_PRIME_BENCH_API(   389, FFT_FORWARD)
void benchmark_fft_397    FFT_PRIME_BENCH_API(   397, FFT_FORWARD)
void benchmark_fft_401    FFT_PRIME_BENCH_API(   401, FFT_FORWARD)
void benchmark_fft_409    FFT_PRIME_BENCH_API(   409, FFT_FORWARD)
void benchmark_fft_419    FFT_PRIME_BENCH_API(   419, FFT_FORWARD)
void benchmark_fft_421    FFT_PRIME_BENCH_API(   421, FFT_FORWARD)
void benchmark_fft_431    FFT_PRIME_BENCH_API(   431, FFT_FORWARD)
void benchmark_fft_433    FFT_PRIME_BENCH_API(   433, FFT_FORWARD)
void benchmark_fft_439    FFT_PRIME_BENCH_API(   439, FFT_FORWARD)
void benchmark_fft_443    FFT_PRIME_BENCH_API(   443, FFT_FORWARD)
void benchmark_fft_449    FFT_PRIME_BENCH_API(   449, FFT_FORWARD)
void benchmark_fft_457    FFT_PRIME_BENCH_API(   457, FFT_FORWARD)
void benchmark_fft_461    FFT_PRIME_BENCH_API(   461, FFT_FORWARD)
void benchmark_fft_463    FFT_PRIME_BENCH_API(   463, FFT_FORWARD)
void benchmark_fft_467    FFT_PRIME_BENCH_API(   467, FFT_FORWARD)
void benchmark_fft_479    FFT_PRIME_BENCH_API(   479, FFT_FORWARD)
void benchmark_fft_487    FFT_PRIME_BENCH_API(   487, FFT_FORWARD)
void benchmark_fft_491    FFT_PRIME_BENCH_API(   491, FFT_FORWARD)
void benchmark_fft_499    FFT_PRIME_BENCH_API(   499, FFT_FORWARD)
void benchmark_fft_503    FFT_PRIME_BENCH_API(   503, FFT_FORWARD)
void benchmark_fft_509    FFT_PRIME_BENCH_API(   509, FFT_FORWARD)
