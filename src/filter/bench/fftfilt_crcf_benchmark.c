/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

// Helper function to keep code base small
void fftfilt_crcf_bench(struct rusage *     _start,
                        struct rusage *     _finish,
                        unsigned long int * _num_iterations,
                        unsigned int        _n)
{
    // adjust number of iterations:
    *_num_iterations *= 100;

    if      (_n <  6) *_num_iterations /= 120;
    else if (_n < 12) *_num_iterations /= 40;
    else              *_num_iterations /= 5*_n;

    // generate coefficients
    unsigned int h_len = _n+1;
    float h[h_len];
    unsigned long int i;
    for (i=0; i<h_len; i++)
        h[i] = randnf();

    // create filter object
    fftfilt_crcf q = fftfilt_crcf_create(h,h_len,_n);

    // generate input vector
    float complex x[_n + 4];
    for (i=0; i<_n+4; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // output vector
    float complex y[_n];

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fftfilt_crcf_execute(q, &x[0], y);
        fftfilt_crcf_execute(q, &x[1], y);
        fftfilt_crcf_execute(q, &x[2], y);
        fftfilt_crcf_execute(q, &x[3], y);
    }
    getrusage(RUSAGE_SELF, _finish);

    // scale number of iterations: loop unrolled 4 times, _n samples/block
    *_num_iterations *= 4 * _n;

    // destroy filter object
    fftfilt_crcf_destroy(q);
}

#define FFTFILT_CRCF_BENCHMARK_API(N)   \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fftfilt_crcf_bench(_start, _finish, _num_iterations, N); }

void benchmark_fftfilt_crcf_4    FFTFILT_CRCF_BENCHMARK_API(4)
void benchmark_fftfilt_crcf_8    FFTFILT_CRCF_BENCHMARK_API(8)
void benchmark_fftfilt_crcf_16   FFTFILT_CRCF_BENCHMARK_API(16)
void benchmark_fftfilt_crcf_32   FFTFILT_CRCF_BENCHMARK_API(32)
void benchmark_fftfilt_crcf_64   FFTFILT_CRCF_BENCHMARK_API(64)

