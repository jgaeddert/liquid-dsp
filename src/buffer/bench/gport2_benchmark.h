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

#ifndef __LIQUID_GPORT2_BENCHMARK_H__
#define __LIQUID_GPORT2_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define GPORT2_BENCH_API(N)             \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ gport2_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void gport2_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    // initialize port
    gport2 p = gport2_create(2*_n-1,sizeof(int));
    int w[_n], r[_n];

    unsigned long int i;

    // start trials:
    //   write to port, read from port
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        gport2_produce(p,(void*)w,_n);
        gport2_consume(p,(void*)r,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= _n;

    gport2_destroy(p);
}

// 
void benchmark_gport2_n1    GPORT2_BENCH_API(1)
void benchmark_gport2_n4    GPORT2_BENCH_API(4)
void benchmark_gport2_n16   GPORT2_BENCH_API(16)
void benchmark_gport2_n64   GPORT2_BENCH_API(64)

#endif // __LIQUID_GPORT2_BENCHMARK_H__

