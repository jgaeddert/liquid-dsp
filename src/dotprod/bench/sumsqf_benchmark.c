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
void sumsqf_bench(struct rusage *     _start,
                  struct rusage *     _finish,
                  unsigned long int * _num_iterations,
                  unsigned int        _n)
{
    // normalize number of iterations
    *_num_iterations *= 128;
    *_num_iterations /= _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    float x[_n];
    float y;
    unsigned int long i;
    for (i=0; i<_n; i++)
        x[i] = 1.0f;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = liquid_sumsqf(x, _n);
        y = liquid_sumsqf(x, _n);
        y = liquid_sumsqf(x, _n);
        y = liquid_sumsqf(x, _n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

#define SUMSQF_BENCHMARK_API(N)         \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ sumsqf_bench(_start, _finish, _num_iterations, N); }

void benchmark_sumsqf_4         SUMSQF_BENCHMARK_API(4)
void benchmark_sumsqf_16        SUMSQF_BENCHMARK_API(16)
void benchmark_sumsqf_64        SUMSQF_BENCHMARK_API(64)
void benchmark_sumsqf_256       SUMSQF_BENCHMARK_API(256)

