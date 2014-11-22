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
void bsequence_correlate_bench(struct rusage *_start,
                               struct rusage *_finish,
                               unsigned long int *_num_iterations,
                               unsigned int _n)
{
    // normalize number of iterations
    *_num_iterations *= 1000;
    *_num_iterations /= _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    // create and initialize binary sequences
    bsequence bs1 = bsequence_create(_n);
    bsequence bs2 = bsequence_create(_n);

    unsigned long int i;
    int rxy;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        rxy = bsequence_correlate(bs1, bs2);
        rxy = bsequence_correlate(bs1, bs2);
        rxy = bsequence_correlate(bs1, bs2);
        rxy = bsequence_correlate(bs1, bs2);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up memory
    bsequence_destroy(bs1);
    bsequence_destroy(bs2);
}

#define BSEQUENCE_BENCHMARK_API(N)          \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ bsequence_correlate_bench(_start, _finish, _num_iterations, N); }

// 
void benchmark_bsequence_xcorr_n16      BSEQUENCE_BENCHMARK_API(16)
void benchmark_bsequence_xcorr_n64      BSEQUENCE_BENCHMARK_API(64)
void benchmark_bsequence_xcorr_n256     BSEQUENCE_BENCHMARK_API(256)
void benchmark_bsequence_xcorr_n1024    BSEQUENCE_BENCHMARK_API(1024)

