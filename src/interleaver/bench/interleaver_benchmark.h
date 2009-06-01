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

#ifndef __LIQUID_INTERLEAVER_BENCHMARK_H__
#define __LIQUID_INTERLEAVER_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define INTERLEAVER_BENCH_API(N,T) \
(   struct rusage *_start, \
    struct rusage *_finish, \
    unsigned long int *_num_iterations) \
{ interleaver_bench(_start, _finish, _num_iterations, N, T); }

// Helper function to keep code base small
void interleaver_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n,
    interleaver_type _type)
{
    // initialize interleaver
    interleaver q = interleaver_create(_n, _type);

    unsigned char x[_n];
    unsigned char y[_n];
    
    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        interleaver_interleave(q, x, y);
        interleaver_interleave(q, x, y);
        interleaver_interleave(q, x, y);
        interleaver_interleave(q, x, y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

}

void benchmark_interleaver_8    INTERLEAVER_BENCH_API(8,    INT_BLOCK)
void benchmark_interleaver_16   INTERLEAVER_BENCH_API(16,   INT_BLOCK)
void benchmark_interleaver_64   INTERLEAVER_BENCH_API(64,   INT_BLOCK)
void benchmark_interleaver_256  INTERLEAVER_BENCH_API(256,  INT_BLOCK)

#endif // __LIQUID_INTERLEAVER_BENCHMARK_H__

