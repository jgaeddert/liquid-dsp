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

#ifndef __LIQUID_FIR_FILTER_RRRF_BENCHMARK_H__
#define __LIQUID_FIR_FILTER_RRRF_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void fir_filter_rrrf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    float b[_n], y;
    unsigned int i;
    for (i=0; i<_n; i++)
        b[i] = 1.0f;

    fir_filter_rrrf f = fir_filter_rrrf_create(b,_n);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        fir_filter_rrrf_push(f,1.0f);
        fir_filter_rrrf_execute(f,&y);

        fir_filter_rrrf_push(f,1.0f);
        fir_filter_rrrf_execute(f,&y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 2;

    fir_filter_rrrf_destroy(f);

}

#define FIR_FILTER_RRRF_BENCHMARK_API(N)     \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ fir_filter_rrrf_bench(_start, _finish, _num_iterations, N); }

void benchmark_fir_filter_rrrf_4    FIR_FILTER_RRRF_BENCHMARK_API(4)
void benchmark_fir_filter_rrrf_8    FIR_FILTER_RRRF_BENCHMARK_API(8)
void benchmark_fir_filter_rrrf_16   FIR_FILTER_RRRF_BENCHMARK_API(16)

#endif // __LIQUID_FIR_FILTER_RRRF_BENCHMARK_H__

