/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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

// 
// gport_dma_benchmark.c
//
// Run gport (generic data port) using direct memory
// access benchmark
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include "liquid.experimental.h"

#define GPORT_DMA_BENCH_API(N)          \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ gport_dma_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void gport_dma_bench(struct rusage *_start,
                     struct rusage *_finish,
                     unsigned long int *_num_iterations,
                     unsigned int _n)
{
    // initialize port
    gport p = gport_create(2*_n-1,sizeof(int));

    unsigned long int i;

    // pointer to internal buffer
    void * buffer = NULL;

    // start trials:
    //   write to port, read from port
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // producer (write to port)
        buffer = gport_producer_lock(p,_n);
        gport_producer_unlock(p,_n);

        // consumer (read from port)
        buffer = gport_consumer_lock(p,_n);
        gport_consumer_unlock(p,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= _n;

    gport_destroy(p);
}

// 
void benchmark_gport_dma_n1     GPORT_DMA_BENCH_API(1)
void benchmark_gport_dma_n4     GPORT_DMA_BENCH_API(4)
void benchmark_gport_dma_n16    GPORT_DMA_BENCH_API(16)
void benchmark_gport_dma_n64    GPORT_DMA_BENCH_API(64)
void benchmark_gport_dma_n256   GPORT_DMA_BENCH_API(256)

