/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

