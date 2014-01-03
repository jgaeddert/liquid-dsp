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

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void matrixf_mul_bench(struct rusage *_start,
                       struct rusage *_finish,
                       unsigned long int *_num_iterations,
                       unsigned int _n)
{
    // normalize number of iterations
    // time ~ _n ^ 2
    *_num_iterations /= _n * _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    float a[_n*_n];
    float b[_n*_n];
    float c[_n*_n];
    unsigned int i;
    for (i=0; i<_n*_n; i++) {
        a[i] = randnf();
        b[i] = randnf();
    }
    
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        matrixf_mul(a,_n,_n,  b,_n,_n,  c,_n,_n);
        matrixf_mul(a,_n,_n,  b,_n,_n,  c,_n,_n);
        matrixf_mul(a,_n,_n,  b,_n,_n,  c,_n,_n);
        matrixf_mul(a,_n,_n,  b,_n,_n,  c,_n,_n);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

#define MATRIXF_MUL_BENCHMARK_API(N)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ matrixf_mul_bench(_start, _finish, _num_iterations, N); }

void benchmark_matrixf_mul_n2      MATRIXF_MUL_BENCHMARK_API(2)
void benchmark_matrixf_mul_n4      MATRIXF_MUL_BENCHMARK_API(4)
void benchmark_matrixf_mul_n8      MATRIXF_MUL_BENCHMARK_API(8)
void benchmark_matrixf_mul_n16     MATRIXF_MUL_BENCHMARK_API(16)
void benchmark_matrixf_mul_n32     MATRIXF_MUL_BENCHMARK_API(32)
void benchmark_matrixf_mul_n64     MATRIXF_MUL_BENCHMARK_API(64)

