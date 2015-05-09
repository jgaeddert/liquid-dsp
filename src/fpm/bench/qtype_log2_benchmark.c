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
#include <math.h>

#include "liquid.h"
#include "liquidfpm.internal.h"

// 
// BENCHMARK: q16 log2 (shift/add)
//
void benchmark_q16_log2_shiftadd(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 unsigned int _n)
{
    // initialize arrays
    q16_t x0 = rand() & 0x7fff;
    q16_t x1 = rand() & 0x7fff;
    q16_t x2 = rand() & 0x7fff;
    q16_t x3 = rand() & 0x7fff;
    q16_t y;
    unsigned int precision = 16;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q16_log2_shiftadd(x0, precision);
        y = q16_log2_shiftadd(x1, precision);
        y = q16_log2_shiftadd(x2, precision);
        y = q16_log2_shiftadd(x3, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: q16 log2 (look-up table)
//
void benchmark_q16_log2_frac(struct rusage *_start,
                             struct rusage *_finish,
                             unsigned long int *_num_iterations,
                             unsigned int _n)
{
    // initialize arrays
    q16_t x0 = rand() & 0x7fff;
    q16_t x1 = rand() & 0x7fff;
    q16_t x2 = rand() & 0x7fff;
    q16_t x3 = rand() & 0x7fff;
    q16_t y;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q16_log2_frac(x0);
        y = q16_log2_frac(x1);
        y = q16_log2_frac(x2);
        y = q16_log2_frac(x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: q32 log2 (shift/add)
//
void benchmark_q32_log2_shiftadd(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 unsigned int _n)
{
    // initialize arrays
    q32_t x0 = rand() & 0x7fff;
    q32_t x1 = rand() & 0x7fff;
    q32_t x2 = rand() & 0x7fff;
    q32_t x3 = rand() & 0x7fff;
    q32_t y;
    unsigned int precision = 32;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q32_log2_shiftadd(x0, precision);
        y = q32_log2_shiftadd(x1, precision);
        y = q32_log2_shiftadd(x2, precision);
        y = q32_log2_shiftadd(x3, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: q32 log2
//
void benchmark_q32_log2_frac(struct rusage *_start,
                             struct rusage *_finish,
                             unsigned long int *_num_iterations,
                             unsigned int _n)
{
    // initialize arrays
    q32_t x0 = rand() & 0x7fffffff;
    q32_t x1 = rand() & 0x7fffffff;
    q32_t x2 = rand() & 0x7fffffff;
    q32_t x3 = rand() & 0x7fffffff;
    q32_t y;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q32_log2_frac(x0);
        y = q32_log2_frac(x1);
        y = q32_log2_frac(x2);
        y = q32_log2_frac(x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}


// 
// BENCHMARK: floating-point log2
//
void benchmark_log2f(struct rusage *_start,
                     struct rusage *_finish,
                     unsigned long int *_num_iterations,
                     unsigned int _n)
{
    // initialize arrays
    float x0 = randf();
    float x1 = randf();
    float x2 = randf();
    float x3 = randf();
    float y;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = log2f(x0);
        y = log2f(x1);
        y = log2f(x2);
        y = log2f(x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

