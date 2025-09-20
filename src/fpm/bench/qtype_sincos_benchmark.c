/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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
// BENCHMARK: q16 sin|cos (cordic)
//
void benchmark_q16_sincos_cordic(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 unsigned int _n)
{
    // initialize input
    q16_t x0 = rand() & 0xffff;
    q16_t x1 = rand() & 0xffff;
    q16_t x2 = rand() & 0xffff;
    q16_t x3 = rand() & 0xffff;
    q16_t s = 0, c = 0;
    unsigned int precision = 16;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q16_sincos_cordic(x0 + s - c, &s, &c, precision);
        q16_sincos_cordic(x1 + s - c, &s, &c, precision);
        q16_sincos_cordic(x2 + s - c, &s, &c, precision);
        q16_sincos_cordic(x3 + s - c, &s, &c, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += ((s^c)%2); // artificial use of output to avoid compiler warning
}

// 
// BENCHMARK: q16 sin|cos (look-up table)
//
void benchmark_q16_sincos_tab(struct rusage *_start,
                              struct rusage *_finish,
                              unsigned long int *_num_iterations,
                              unsigned int _n)
{
    // initialize input
    q16_t x0 = rand() & 0xffff;
    q16_t x1 = rand() & 0xffff;
    q16_t x2 = rand() & 0xffff;
    q16_t x3 = rand() & 0xffff;
    q16_t s = 0, c = 0;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q16_sincos_tab(x0 + s - c, &s, &c);
        q16_sincos_tab(x1 + s - c, &s, &c);
        q16_sincos_tab(x2 + s - c, &s, &c);
        q16_sincos_tab(x3 + s - c, &s, &c);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += ((s^c)%2); // artificial use of output to avoid compiler warning
}


// 
// BENCHMARK: q32 sin|cos (cordic)
//
void benchmark_q32_sincos_cordic(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 unsigned int _n)
{
    // initialize input
    q32_t x0 = rand() & 0xffff;
    q32_t x1 = rand() & 0xffff;
    q32_t x2 = rand() & 0xffff;
    q32_t x3 = rand() & 0xffff;
    q32_t s = 0, c = 0;
    unsigned int precision = 32;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q32_sincos_cordic(x0 + s - c, &s, &c, precision);
        q32_sincos_cordic(x1 + s - c, &s, &c, precision);
        q32_sincos_cordic(x2 + s - c, &s, &c, precision);
        q32_sincos_cordic(x3 + s - c, &s, &c, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += ((s^c)%2); // artificial use of output to avoid compiler warning
}


// 
// BENCHMARK: q32 sin|cos (look-up table)
//
void benchmark_q32_sincos_tab(struct rusage *_start,
                              struct rusage *_finish,
                              unsigned long int *_num_iterations,
                              unsigned int _n)
{
    // initialize input
    q32_t x0 = rand() & 0xffff;
    q32_t x1 = rand() & 0xffff;
    q32_t x2 = rand() & 0xffff;
    q32_t x3 = rand() & 0xffff;
    q32_t s = 0, c = 0;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q32_sincos_tab(x0 + s - c, &s, &c);
        q32_sincos_tab(x1 + s - c, &s, &c);
        q32_sincos_tab(x2 + s - c, &s, &c);
        q32_sincos_tab(x3 + s - c, &s, &c);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += ((s^c)%2); // artificial use of output to avoid compiler warning
}

// 
// BENCHMARK: floating-point sin|cos
//
void benchmark_float_sincos(struct rusage *_start,
                            struct rusage *_finish,
                            unsigned long int *_num_iterations,
                            unsigned int _n)
{
    // initialize input
    float x0 = 2 * M_PI * randf();
    float x1 = 2 * M_PI * randf();
    float x2 = 2 * M_PI * randf();
    float x3 = 2 * M_PI * randf();
    float s = 0, c = 0;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        s += sinf(x0);   c += cosf(x0);
        s += sinf(x1);   c += cosf(x1);
        s += sinf(x2);   c += cosf(x2);
        s += sinf(x3);   c += cosf(x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += ((s+c)>2); // artificial use of output to avoid compiler warning
}

