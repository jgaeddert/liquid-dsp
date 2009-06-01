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

#ifndef __LIQUID_GPORT_BENCHMARK_H__
#define __LIQUID_GPORT_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define GPORT_BENCH_API(N)              \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ gport_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void gport_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n)
{
    // initialize port
    gport p = gport_create(2*_n-1,sizeof(int));

    unsigned long int i;

    // start trials:
    //   write to port, read from port
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        gport_producer_lock(p,_n);
        gport_producer_unlock(p,_n);

        gport_consumer_lock(p,_n);
        gport_consumer_unlock(p,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= _n;

    gport_destroy(p);
}

// 
void benchmark_gport_n1     GPORT_BENCH_API(1)
void benchmark_gport_n4     GPORT_BENCH_API(4)
void benchmark_gport_n16    GPORT_BENCH_API(16)
void benchmark_gport_n64    GPORT_BENCH_API(64)

#endif // __LIQUID_GPORT_BENCHMARK_H__

