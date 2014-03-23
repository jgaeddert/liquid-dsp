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

#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>
#include "liquid.h"

#define INTERLEAVER_BENCH_API(N)        \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ interleaver_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void interleaver_bench(struct rusage *_start,
                       struct rusage *_finish,
                       unsigned long int *_num_iterations,
                       unsigned int _n)
{
    // scale number of iterations by block size
    // iterations = 4: cycles/trial ~ exp( -0.883 + 0.708*log(_n) )
    *_num_iterations /= 0.7f*expf( -0.883 + 0.708*logf(_n) );

    // initialize interleaver
    interleaver q = interleaver_create(_n);
    interleaver_set_depth(q, 4);

    unsigned char x[_n];
    unsigned char y[_n];
    
    unsigned long int i;
    for (i=0; i<_n; i++)
        x[i] = rand() & 0xff;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        interleaver_encode(q, x, y);
        interleaver_encode(q, x, y);
        interleaver_encode(q, x, y);
        interleaver_encode(q, x, y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // destroy interleaver object
    interleaver_destroy(q);
}

void benchmark_interleaver_8    INTERLEAVER_BENCH_API(8     )
void benchmark_interleaver_16   INTERLEAVER_BENCH_API(16    )
void benchmark_interleaver_32   INTERLEAVER_BENCH_API(32    )
void benchmark_interleaver_64   INTERLEAVER_BENCH_API(64    )
void benchmark_interleaver_128  INTERLEAVER_BENCH_API(128   )
void benchmark_interleaver_256  INTERLEAVER_BENCH_API(256   )
void benchmark_interleaver_512  INTERLEAVER_BENCH_API(512   )
void benchmark_interleaver_1024 INTERLEAVER_BENCH_API(1024  )

