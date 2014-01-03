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
void sumsqcq16_bench(struct rusage *     _start,
                     struct rusage *     _finish,
                     unsigned long int * _num_iterations,
                     unsigned int        _n)
{
    // normalize number of iterations
    *_num_iterations *= 128;
    *_num_iterations /= _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    cq16_t x[_n];
    q16_t y;
    unsigned int long i;
    for (i=0; i<_n; i++) {
        x[i].real = q16_one >> 2;
        x[i].imag = q16_one >> 2;
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = liquid_sumsqcq16(x, _n);
        y = liquid_sumsqcq16(x, _n);
        y = liquid_sumsqcq16(x, _n);
        y = liquid_sumsqcq16(x, _n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

#define SUMSQCQ16_BENCHMARK_API(N)      \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ sumsqcq16_bench(_start, _finish, _num_iterations, N); }

void benchmark_sumsqcq16_4      SUMSQCQ16_BENCHMARK_API(4)
void benchmark_sumsqcq16_16     SUMSQCQ16_BENCHMARK_API(16)
void benchmark_sumsqcq16_64     SUMSQCQ16_BENCHMARK_API(64)
void benchmark_sumsqcq16_256    SUMSQCQ16_BENCHMARK_API(256)

