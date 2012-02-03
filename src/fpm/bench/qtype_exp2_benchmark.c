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

#include <sys/resource.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquidfpm.internal.h"

// 
// BENCHMARK: q16 exp2 (shift/add)
//
void benchmark_q16_exp2_shiftadd(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 unsigned int _n)
{
    // initialize arrays
    q16_t x0 = rand() % q16_one;
    q16_t x1 = rand() % q16_one;
    q16_t x2 = rand() % q16_one;
    q16_t x3 = rand() % q16_one;
    q16_t y;
    unsigned int precision = 16;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q16_exp2_shiftadd(x0, precision);
        y = q16_exp2_shiftadd(x1, precision);
        y = q16_exp2_shiftadd(x2, precision);
        y = q16_exp2_shiftadd(x3, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: q16 exp2 (look-up table)
//
void benchmark_q16_exp2_frac(struct rusage *_start,
                             struct rusage *_finish,
                             unsigned long int *_num_iterations,
                             unsigned int _n)
{
    // initialize arrays
    q16_t x0 = rand() % q16_one;
    q16_t x1 = rand() % q16_one;
    q16_t x2 = rand() % q16_one;
    q16_t x3 = rand() % q16_one;
    q16_t y;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q16_exp2_frac(x0);
        y = q16_exp2_frac(x1);
        y = q16_exp2_frac(x2);
        y = q16_exp2_frac(x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: q32 exp2 (shift/add)
//
void benchmark_q32_exp2_shiftadd(struct rusage *_start,
                                 struct rusage *_finish,
                                 unsigned long int *_num_iterations,
                                 unsigned int _n)
{
    // initialize arrays
    q32_t x0 = rand() % q32_one;
    q32_t x1 = rand() % q32_one;
    q32_t x2 = rand() % q32_one;
    q32_t x3 = rand() % q32_one;
    q32_t y;
    unsigned int precision = 32;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q32_exp2_shiftadd(x0, precision);
        y = q32_exp2_shiftadd(x1, precision);
        y = q32_exp2_shiftadd(x2, precision);
        y = q32_exp2_shiftadd(x3, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: q32 exp2
//
void benchmark_q32_exp2_frac(struct rusage *_start,
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
        y = q32_exp2_frac(x0);
        y = q32_exp2_frac(x1);
        y = q32_exp2_frac(x2);
        y = q32_exp2_frac(x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}


// 
// BENCHMARK: floating-point exp2
//
void benchmark_exp2f(struct rusage *_start,
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
        y = exp2f(x0);
        y = exp2f(x1);
        y = exp2f(x2);
        y = exp2f(x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

