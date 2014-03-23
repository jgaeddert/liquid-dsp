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

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include "liquid.h"

#define CBUFFERCF_BENCH_API(N, W, R)        \
(   struct rusage *     _start,             \
    struct rusage *     _finish,            \
    unsigned long int * _num_iterations)    \
{ cbuffercf_bench(_start, _finish, _num_iterations, N, W, R); }

// Helper function to keep code base small
void cbuffercf_bench(struct rusage *     _start,
                     struct rusage *     _finish,
                     unsigned long int * _num_iterations,
                     unsigned int        _n,
                     unsigned int        _write_size,
                     unsigned int        _read_size)
{
    // validate input
    if (_n < 2) {
        fprintf(stderr,"error: cbuffercf_bench(), number of elements must be at least 2\n");
        exit(1);
    } else if (_write_size > _n-1) {
        fprintf(stderr,"error: cbuffercf_bench(), write size must be in (0,n)\n");
        exit(1);
    } else if (_read_size > _n-1) {
        fprintf(stderr,"error: cbuffercf_bench(), read size must be in (0,n)\n");
        exit(1);
    }

    // normalize number of iterations
    *_num_iterations *= _n;

    // create object
    cbuffercf q = cbuffercf_create(_n);

    // 
    float complex   v[_write_size]; // array for writing
    float complex * r;              // read pointer
    unsigned int num_requested;     // number of elements requested
    unsigned int num_read;          // number of elements read

    // initialize array for writing
    unsigned int i;
    for (i=0; i<_write_size; i++)
        v[i] = 0.0f;

    // accumulate total number of elements
    unsigned long int num_total_elements = 0;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    while (num_total_elements < *_num_iterations) {
        // write elements to buffer if space is available
        if (_n - cbuffercf_size(q) > _write_size)
            cbuffercf_write(q, v, _write_size);

        // read up to '_read_size' elements
        num_requested = _read_size;
        cbuffercf_read(q, num_requested, &r, &num_read);

        // release elements that were read
        cbuffercf_release(q, num_read);

        // increment counter by number of elements passing through
        num_total_elements += num_read;
    }
    getrusage(RUSAGE_SELF, _finish);

    // total number of iterations equal to to total number of elements
    // that have passed through the buffer
    *_num_iterations = num_total_elements;

    // clean up allocated memory
    cbuffercf_destroy(q);
}

// 
void benchmark_cbuffercf_n16     CBUFFERCF_BENCH_API(  16,  12,  11);
void benchmark_cbuffercf_n32     CBUFFERCF_BENCH_API(  32,  24,  23);
void benchmark_cbuffercf_n64     CBUFFERCF_BENCH_API(  64,  48,  47);
void benchmark_cbuffercf_n128    CBUFFERCF_BENCH_API( 128,  96,  95);
void benchmark_cbuffercf_n256    CBUFFERCF_BENCH_API( 256, 192, 191);
void benchmark_cbuffercf_n512    CBUFFERCF_BENCH_API( 512, 384, 383);
void benchmark_cbuffercf_n1024   CBUFFERCF_BENCH_API(1024, 768, 767);

