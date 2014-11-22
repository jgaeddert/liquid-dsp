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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>

#include "liquid.internal.h"

// Helper function to keep code base small
void bsync_cccf_bench(struct rusage *     _start,
                      struct rusage *     _finish,
                      unsigned long int * _num_iterations,
                      unsigned int        _n)
{
    // adjust number of iterations
    *_num_iterations *= 4;
    *_num_iterations /= _n;

    // generate sequence (random)
    float complex h[_n];
    unsigned long int i;
    for (i=0; i<_n; i++) {
        h[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    // generate synchronizer
    bsync_cccf q = bsync_cccf_create(_n,h);

    // input sequence (random)
    float complex x[7];
    for (i=0; i<7; i++) {
        x[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    float complex rxy;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // push input sequence through synchronizer
        bsync_cccf_correlate(q, x[0], &rxy);
        bsync_cccf_correlate(q, x[1], &rxy);
        bsync_cccf_correlate(q, x[2], &rxy);
        bsync_cccf_correlate(q, x[3], &rxy);
        bsync_cccf_correlate(q, x[4], &rxy);
        bsync_cccf_correlate(q, x[5], &rxy);
        bsync_cccf_correlate(q, x[6], &rxy);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 7;

    // clean up allocated objects
    bsync_cccf_destroy(q);
}

#define BSYNC_CCCF_BENCHMARK_API(N)         \
(   struct rusage *     _start,             \
    struct rusage *     _finish,            \
    unsigned long int * _num_iterations)    \
{ bsync_cccf_bench(_start, _finish, _num_iterations, N); }

void benchmark_bsync_cccf_16   BSYNC_CCCF_BENCHMARK_API(16)
void benchmark_bsync_cccf_32   BSYNC_CCCF_BENCHMARK_API(32)
void benchmark_bsync_cccf_64   BSYNC_CCCF_BENCHMARK_API(64)
void benchmark_bsync_cccf_128  BSYNC_CCCF_BENCHMARK_API(128)
void benchmark_bsync_cccf_256  BSYNC_CCCF_BENCHMARK_API(256)

