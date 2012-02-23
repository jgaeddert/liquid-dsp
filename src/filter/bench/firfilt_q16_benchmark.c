/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
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

// API definition macro; helper function to keep code base small
#define LIQUID_FIRFILT_Q16_BENCH_API(FIRFILT,TO,TC,TI)              \
                                                                    \
void FIRFILT(_bench)(struct rusage *     _start,                    \
                     struct rusage *     _finish,                   \
                     unsigned long int * _num_iterations,           \
                     unsigned int        _n)                        \
{                                                                   \
    /* TODO: adjust number of iterations */                         \
                                                                    \
    unsigned long int i;                                            \
                                                                    \
    /* generate coefficients (random bytes) */                      \
    TC h[_n];                                                       \
    unsigned char * hp = (unsigned char*)h;                         \
    for (i=0; i<sizeof(h); i++)                                     \
        hp[i] = rand() & 0xff;                                      \
                                                                    \
    /* create filter object */                                      \
    FIRFILT() q = FIRFILT(_create)(h,_n);                           \
                                                                    \
    /* generate input vector (random bytes) */                      \
    TI x[4];                                                        \
    unsigned char * xp = (unsigned char*)x;                         \
    for (i=0; i<sizeof(x); i++)                                     \
        xp[i] = rand() & 0xff;                                      \
                                                                    \
    /* output vector */                                             \
    TO y[4];                                                        \
                                                                    \
    /* start trials */                                              \
    getrusage(RUSAGE_SELF, _start);                                 \
    for (i=0; i<(*_num_iterations); i++) {                          \
        FIRFILT(_push)(q, x[0]); FIRFILT(_execute)(q, &y[0]);       \
        FIRFILT(_push)(q, x[1]); FIRFILT(_execute)(q, &y[1]);       \
        FIRFILT(_push)(q, x[2]); FIRFILT(_execute)(q, &y[2]);       \
        FIRFILT(_push)(q, x[3]); FIRFILT(_execute)(q, &y[3]);       \
    }                                                               \
    getrusage(RUSAGE_SELF, _finish);                                \
    *_num_iterations *= 4;                                          \
                                                                    \
    FIRFILT(_destroy)(q);                                           \
}                                                                   \

//
// BENCHMARKS : rrrq16
//
LIQUID_FIRFILT_Q16_BENCH_API(FIRFILT_MANGLE_RRRQ16,  q16_t,  q16_t,  q16_t)

#define FIRFILT_RRRQ16_BENCHMARK_API(N) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ firfilt_rrrq16_bench(_start, _finish, _num_iterations, N); }

void benchmark_firfilt_rrrq16_4     FIRFILT_RRRQ16_BENCHMARK_API(4)
void benchmark_firfilt_rrrq16_8     FIRFILT_RRRQ16_BENCHMARK_API(8)
void benchmark_firfilt_rrrq16_16    FIRFILT_RRRQ16_BENCHMARK_API(16)
void benchmark_firfilt_rrrq16_32    FIRFILT_RRRQ16_BENCHMARK_API(32)
void benchmark_firfilt_rrrq16_64    FIRFILT_RRRQ16_BENCHMARK_API(64)



//
// BENCHMARKS : crcq16
//
LIQUID_FIRFILT_Q16_BENCH_API(FIRFILT_MANGLE_CRCQ16, cq16_t,  q16_t, cq16_t)

#define FIRFILT_CRCQ16_BENCHMARK_API(N) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ firfilt_crcq16_bench(_start, _finish, _num_iterations, N); }

void benchmark_firfilt_crcq16_4     FIRFILT_CRCQ16_BENCHMARK_API(4)
void benchmark_firfilt_crcq16_8     FIRFILT_CRCQ16_BENCHMARK_API(8)
void benchmark_firfilt_crcq16_16    FIRFILT_CRCQ16_BENCHMARK_API(16)
void benchmark_firfilt_crcq16_32    FIRFILT_CRCQ16_BENCHMARK_API(32)
void benchmark_firfilt_crcq16_64    FIRFILT_CRCQ16_BENCHMARK_API(64)



//
// BENCHMARKS : cccq16
//
LIQUID_FIRFILT_Q16_BENCH_API(FIRFILT_MANGLE_CCCQ16, cq16_t, cq16_t, cq16_t)

#define FIRFILT_CCCQ16_BENCHMARK_API(N) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ firfilt_cccq16_bench(_start, _finish, _num_iterations, N); }

void benchmark_firfilt_cccq16_4     FIRFILT_CCCQ16_BENCHMARK_API(4)
void benchmark_firfilt_cccq16_8     FIRFILT_CCCQ16_BENCHMARK_API(8)
void benchmark_firfilt_cccq16_16    FIRFILT_CCCQ16_BENCHMARK_API(16)
void benchmark_firfilt_cccq16_32    FIRFILT_CCCQ16_BENCHMARK_API(32)
void benchmark_firfilt_cccq16_64    FIRFILT_CCCQ16_BENCHMARK_API(64)

