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
// BENCHMARK: q16 atan2 (cordic)
//
void benchmark_q16_atan2_cordic(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 unsigned int _n)
{
    // initialize input
    q16_t x0 = rand() & 0xffff, y0 = rand() & 0xffff;
    q16_t x1 = rand() & 0xffff, y1 = rand() & 0xffff;
    q16_t x2 = rand() & 0xffff, y2 = rand() & 0xffff;
    q16_t x3 = rand() & 0xffff, y3 = rand() & 0xffff;
    q16_t r, theta = 0;
    unsigned int precision = 16;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q16_atan2_cordic(x0, y0, &r, &theta, precision);
        q16_atan2_cordic(x1, y1, &r, &theta, precision);
        q16_atan2_cordic(x2, y2, &r, &theta, precision);
        q16_atan2_cordic(x3, y3, &r, &theta, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += (theta % 2); // artificial use of output to avoid compiler warning
}


// 
// BENCHMARK: q16 atan2 (look-up table)
//
void benchmark_q16_atan2_frac(struct rusage *_start,
                              struct rusage *_finish,
                              unsigned long int *_num_iterations,
                              unsigned int _n)
{
    // initialize input
    q16_t x0 = rand() & 0xffff, y0 = rand() & 0xffff;
    q16_t x1 = rand() & 0xffff, y1 = rand() & 0xffff;
    q16_t x2 = rand() & 0xffff, y2 = rand() & 0xffff;
    q16_t x3 = rand() & 0xffff, y3 = rand() & 0xffff;
    q16_t theta = 0;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        theta += q16_atan2_frac(x0, y0);
        theta += q16_atan2_frac(x1, y1);
        theta += q16_atan2_frac(x2, y2);
        theta += q16_atan2_frac(x3, y3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += (theta % 2); // artificial use of output to avoid compiler warning
}


// 
// BENCHMARK: q32 atan2 (cordic)
//
void benchmark_q32_atan2_cordic(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 unsigned int _n)
{
    // initialize input
    q32_t x0 = rand() & 0xffff, y0 = rand() & 0xffff;
    q32_t x1 = rand() & 0xffff, y1 = rand() & 0xffff;
    q32_t x2 = rand() & 0xffff, y2 = rand() & 0xffff;
    q32_t x3 = rand() & 0xffff, y3 = rand() & 0xffff;
    q32_t r, theta = 0;
    unsigned int precision = 32;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q32_atan2_cordic(x0, y0, &r, &theta, precision);
        q32_atan2_cordic(x1, y1, &r, &theta, precision);
        q32_atan2_cordic(x2, y2, &r, &theta, precision);
        q32_atan2_cordic(x3, y3, &r, &theta, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += (theta % 2); // artificial use of output to avoid compiler warning
}


// 
// BENCHMARK: q32 atan2 (look-up table)
//
void benchmark_q32_atan2_frac(struct rusage *_start,
                              struct rusage *_finish,
                              unsigned long int *_num_iterations,
                              unsigned int _n)
{
    // initialize input
    q32_t x0 = rand() & 0xffff, y0 = rand() & 0xffff;
    q32_t x1 = rand() & 0xffff, y1 = rand() & 0xffff;
    q32_t x2 = rand() & 0xffff, y2 = rand() & 0xffff;
    q32_t x3 = rand() & 0xffff, y3 = rand() & 0xffff;
    q32_t theta = 0;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        theta += q32_atan2_frac(x0, y0);
        theta += q32_atan2_frac(x1, y1);
        theta += q32_atan2_frac(x2, y2);
        theta += q32_atan2_frac(x3, y3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += (theta % 2); // artificial use of output to avoid compiler warning
}



// 
// BENCHMARK: floating-point atan2
//
void benchmark_atan2f(struct rusage *_start,
                      struct rusage *_finish,
                      unsigned long int *_num_iterations,
                      unsigned int _n)
{
    // initialize input
    float x0 = randnf(), y0 = randnf();
    float x1 = randnf(), y1 = randnf();
    float x2 = randnf(), y2 = randnf();
    float x3 = randnf(), y3 = randnf();
    float theta = 0;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        theta += atan2f(y0, x0);
        theta += atan2f(y1, x1);
        theta += atan2f(y2, x2);
        theta += atan2f(y3, x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
    *_num_iterations += (theta > 2); // artificial use of output to avoid compiler warning
}

