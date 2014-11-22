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
void firfilt_crcf_bench(struct rusage *_start,
                        struct rusage *_finish,
                        unsigned long int *_num_iterations,
                        unsigned int _n)
{
    // adjust number of iterations:
    // cycles/trial ~ 107 + 4.3*_n
    *_num_iterations *= 1000;
    *_num_iterations /= (unsigned int)(107+4.3*_n);

    // generate coefficients
    float h[_n];
    unsigned long int i;
    for (i=0; i<_n; i++)
        h[i] = randnf();

    // create filter object
    firfilt_crcf f = firfilt_crcf_create(h,_n);

    // generate input vector
    float complex x[4];
    for (i=0; i<4; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // output vector
    float complex y[4];

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        firfilt_crcf_push(f, x[0]); firfilt_crcf_execute(f, &y[0]);
        firfilt_crcf_push(f, x[1]); firfilt_crcf_execute(f, &y[1]);
        firfilt_crcf_push(f, x[2]); firfilt_crcf_execute(f, &y[2]);
        firfilt_crcf_push(f, x[3]); firfilt_crcf_execute(f, &y[3]);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    firfilt_crcf_destroy(f);
}

#define FIRFILT_CRCF_BENCHMARK_API(N)   \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ firfilt_crcf_bench(_start, _finish, _num_iterations, N); }

void benchmark_firfilt_crcf_4    FIRFILT_CRCF_BENCHMARK_API(4)
void benchmark_firfilt_crcf_8    FIRFILT_CRCF_BENCHMARK_API(8)
void benchmark_firfilt_crcf_16   FIRFILT_CRCF_BENCHMARK_API(16)
void benchmark_firfilt_crcf_32   FIRFILT_CRCF_BENCHMARK_API(32)
void benchmark_firfilt_crcf_64   FIRFILT_CRCF_BENCHMARK_API(64)

