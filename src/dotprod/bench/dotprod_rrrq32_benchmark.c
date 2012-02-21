/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
 *                                      Institute & State University
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
#include <stdlib.h>
#include "liquid.h"

// Helper function to keep code base small
void dotprod_rrrq32_bench(struct rusage *_start,
                          struct rusage *_finish,
                          unsigned long int *_num_iterations,
                          unsigned int _n)
{
    // normalize number of iterations
    *_num_iterations *= 128;
    *_num_iterations /= _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    q32_t x[_n], h[_n], y;
    unsigned int i;
    for (i=0; i<_n; i++) {
        x[i] = rand() % q32_one;
        h[i] = rand() % q32_one;
    }

    // create dotprod structure;
    dotprod_rrrq32 q = dotprod_rrrq32_create(h,_n);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        dotprod_rrrq32_execute(q, x, &y);
        dotprod_rrrq32_execute(q, x, &y);
        dotprod_rrrq32_execute(q, x, &y);
        dotprod_rrrq32_execute(q, x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up objects
    dotprod_rrrq32_destroy(q);
}

#define DOTPROD_RRRQ32_BENCHMARK_API(N) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ dotprod_rrrq32_bench(_start, _finish, _num_iterations, N); }

void benchmark_dotprod_rrrq32_4       DOTPROD_RRRQ32_BENCHMARK_API(4)
void benchmark_dotprod_rrrq32_16      DOTPROD_RRRQ32_BENCHMARK_API(16)
void benchmark_dotprod_rrrq32_64      DOTPROD_RRRQ32_BENCHMARK_API(64)
void benchmark_dotprod_rrrq32_256     DOTPROD_RRRQ32_BENCHMARK_API(256)

