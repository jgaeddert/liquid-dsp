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

#include <stdlib.h>
#include <sys/resource.h>

#include "liquid.h"

// Helper function to keep code base small
void smatrixf_mul_bench(struct rusage *     _start,
                        struct rusage *     _finish,
                        unsigned long int * _num_iterations,
                        unsigned int        _n)
{
    // normalize number of iterations
    // time ~ _n ^ 3
    *_num_iterations /= _n * _n * _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    unsigned long int i;

    // generate random matrices
    smatrixf a = smatrixf_create(_n, _n);
    smatrixf b = smatrixf_create(_n, _n);
    smatrixf c = smatrixf_create(_n, _n);

    // number of random non-zero entries
    unsigned int nnz = _n / 20 < 4 ? 4 : _n / 20;

    // initialize _a
    for (i=0; i<nnz; i++) {
        unsigned int row = rand() % _n;
        unsigned int col = rand() % _n;
        float value      = randf();
        smatrixf_set(a, row, col, value);
    }
    
    // initialize _b
    for (i=0; i<nnz; i++) {
        unsigned int row = rand() % _n;
        unsigned int col = rand() % _n;
        float value      = randf();
        smatrixf_set(b, row, col, value);
    }

    // initialize c with first multiplication
    smatrixf_mul(a,b,c);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        smatrixf_mul(a,b,c);
        smatrixf_mul(a,b,c);
        smatrixf_mul(a,b,c);
        smatrixf_mul(a,b,c);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // free smatrix objects
    smatrixf_destroy(a);
    smatrixf_destroy(b);
    smatrixf_destroy(c);
}

#define SMATRIXF_MUL_BENCHMARK_API(M)   \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ smatrixf_mul_bench(_start, _finish, _num_iterations, M); }

void benchmark_smatrixf_mul_n32     SMATRIXF_MUL_BENCHMARK_API( 32)
void benchmark_smatrixf_mul_n64     SMATRIXF_MUL_BENCHMARK_API( 64)
void benchmark_smatrixf_mul_n128    SMATRIXF_MUL_BENCHMARK_API(128)
void benchmark_smatrixf_mul_n256    SMATRIXF_MUL_BENCHMARK_API(256)
void benchmark_smatrixf_mul_n512    SMATRIXF_MUL_BENCHMARK_API(512)

