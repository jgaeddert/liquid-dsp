/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
#include "liquid.experimental.h"

// Helper function to keep code base small
void smatrixf_mul_bench(struct rusage *     _start,
                        struct rusage *     _finish,
                        unsigned long int * _num_iterations,
                        unsigned int        _m)
{
    unsigned long int i;

    // generate random matrices
    smatrixf a = smatrixf_create(_m, _m);
    smatrixf b = smatrixf_create(_m, _m);
    smatrixf c = smatrixf_create(_m, _m);

    // number of random non-zero entries
    unsigned int nnz = _m / 10 < 5 ? 5 : _m / 10;

    // initialize _a
    for (i=0; i<nnz; i++) {
        unsigned int r = rand() % _m;
        unsigned int c = rand() % _m;
        float value    = randf();
        smatrixf_set(a, r,c, value);
    }
    
    // initialize _a
    for (i=0; i<nnz; i++) {
        unsigned int r = rand() % _m;
        unsigned int c = rand() % _m;
        float value    = randf();
        smatrixf_set(b, r,c, value);
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

void benchmark_smatrixf_mul_m32     SMATRIXF_MUL_BENCHMARK_API( 32)
void benchmark_smatrixf_mul_m64     SMATRIXF_MUL_BENCHMARK_API( 64)
void benchmark_smatrixf_mul_m128    SMATRIXF_MUL_BENCHMARK_API(128)
void benchmark_smatrixf_mul_m256    SMATRIXF_MUL_BENCHMARK_API(256)
void benchmark_smatrixf_mul_m512    SMATRIXF_MUL_BENCHMARK_API(512)

