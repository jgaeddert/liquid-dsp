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
// gport_dma_threaded_benchmark.c
//
// Run gport (generic data port) using direct memory
// access benchmark with threads
//

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/resource.h>

#include "liquid.experimental.h"

// prototype for thread routines
void gport_dma_producer_handler ( void *ptr );
void gport_dma_consumer_handler ( void *ptr );

// internal structure to keep track of total data handled
// by produer and consumer threads
typedef struct {
    gport p;
    unsigned int producer_size;
    unsigned int consumer_size;
    unsigned long int num_trials;
} gport_dma_threaded_bench_data_t;

#define GPORT_DMA_THREADED_BENCH_API(N) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ gport_dma_threaded_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void gport_dma_threaded_bench(struct rusage *_start,
                              struct rusage *_finish,
                              unsigned long int *_num_iterations,
                              unsigned int _n)
{
    // adjust number of iterations
    *_num_iterations = (*_num_iterations*_n)/4;

    // initialize port
    gport_dma_threaded_bench_data_t data;
    data.p = gport_create(8*_n,sizeof(int));
    data.producer_size = _n;
    data.consumer_size = _n;
    data.num_trials = *_num_iterations;

    // threads
    pthread_t producer_thread;
    pthread_t consumer_thread;
    pthread_attr_t thread_attr;
    void * status;

    // set thread attributes
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_JOINABLE);

    // create threads
    pthread_create(&producer_thread, &thread_attr, (void*) &gport_dma_producer_handler, (void*) &data);
    pthread_create(&consumer_thread, &thread_attr, (void*) &gport_dma_consumer_handler, (void*) &data);

    // destroy attributes object (no longer needed)
    pthread_attr_destroy(&thread_attr);

    // start trials:
    getrusage(RUSAGE_SELF, _start);

    // join threads
    pthread_join(producer_thread, &status);
    pthread_join(consumer_thread, &status);

    getrusage(RUSAGE_SELF, _finish);
    //*_num_iterations *= _n;

    // clean up memory
    gport_destroy(data.p);
}

void gport_dma_producer_handler(void * _data)
{
    // type-cast internal structure
    gport_dma_threaded_bench_data_t * data = 
        (gport_dma_threaded_bench_data_t*)_data;

    unsigned long int i;
    int * w;    // buffer pointer
    for (i=0; i<data->num_trials; i+=data->producer_size) {
        // producer: lock samples for writing
        w = gport_producer_lock(data->p, data->producer_size);

        // producer: once samples are ready, unlock semaphore
        gport_producer_unlock(data->p, data->producer_size);
    }
    pthread_exit(0);
}


void gport_dma_consumer_handler(void * _data)
{
    // type-cast internal structure
    gport_dma_threaded_bench_data_t * data =
        (gport_dma_threaded_bench_data_t*)_data;

    unsigned long int i;
    int * r;    // buffer pointer
    for (i=0; i<data->num_trials; i+=data->consumer_size) {
        // consumer: lock samples for reading
        r = gport_consumer_lock(data->p, data->consumer_size);

        // consumer: release samples when finished
        gport_consumer_unlock(data->p, data->consumer_size);
    }
    pthread_exit(0);
}

// 
void benchmark_gport_dma_threaded_n1    GPORT_DMA_THREADED_BENCH_API(1)
void benchmark_gport_dma_threaded_n4    GPORT_DMA_THREADED_BENCH_API(4)
void benchmark_gport_dma_threaded_n16   GPORT_DMA_THREADED_BENCH_API(16)
void benchmark_gport_dma_threaded_n64   GPORT_DMA_THREADED_BENCH_API(64)
void benchmark_gport_dma_threaded_n256  GPORT_DMA_THREADED_BENCH_API(256)

