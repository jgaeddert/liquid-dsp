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
void bpresync_cccf_bench(struct rusage *     _start,
                         struct rusage *     _finish,
                         unsigned long int * _num_iterations,
                         unsigned int        _n,
                         unsigned int        _m)
{
    // adjust number of iterations
    *_num_iterations *= 4;
    *_num_iterations /= _n;
    *_num_iterations /= _m;

    // generate sequence (random)
    float complex h[_n];
    unsigned long int i;
    for (i=0; i<_n; i++) {
        h[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    // generate synchronizer
    bpresync_cccf q = bpresync_cccf_create(h, _n, 0.1f, _m);

    // input sequence (random)
    float complex x[7];
    for (i=0; i<7; i++) {
        x[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    float complex rxy;
    float dphi_hat;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // push input sequence through synchronizer
        bpresync_cccf_push(q, x[0]);  bpresync_cccf_correlate(q, &rxy, &dphi_hat);
        bpresync_cccf_push(q, x[1]);  bpresync_cccf_correlate(q, &rxy, &dphi_hat);
        bpresync_cccf_push(q, x[2]);  bpresync_cccf_correlate(q, &rxy, &dphi_hat);
        bpresync_cccf_push(q, x[3]);  bpresync_cccf_correlate(q, &rxy, &dphi_hat);
        bpresync_cccf_push(q, x[4]);  bpresync_cccf_correlate(q, &rxy, &dphi_hat);
        bpresync_cccf_push(q, x[5]);  bpresync_cccf_correlate(q, &rxy, &dphi_hat);
        bpresync_cccf_push(q, x[6]);  bpresync_cccf_correlate(q, &rxy, &dphi_hat);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 7;

    // clean up allocated objects
    bpresync_cccf_destroy(q);
}

#define BPRESYNC_CCCF_BENCHMARK_API(N,M)    \
(   struct rusage *     _start,             \
    struct rusage *     _finish,            \
    unsigned long int * _num_iterations)    \
{ bpresync_cccf_bench(_start, _finish, _num_iterations, N, M); }

void benchmark_bpresync_cccf_16   BPRESYNC_CCCF_BENCHMARK_API(16,   6);
void benchmark_bpresync_cccf_32   BPRESYNC_CCCF_BENCHMARK_API(32,   6);
void benchmark_bpresync_cccf_64   BPRESYNC_CCCF_BENCHMARK_API(64,   6);
void benchmark_bpresync_cccf_128  BPRESYNC_CCCF_BENCHMARK_API(128,  6);
void benchmark_bpresync_cccf_256  BPRESYNC_CCCF_BENCHMARK_API(256,  6);

