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
    q16_t s, c;
    unsigned int precision = 16;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q16_sincos_cordic(x0, &s, &c, precision);
        q16_sincos_cordic(x1, &s, &c, precision);
        q16_sincos_cordic(x2, &s, &c, precision);
        q16_sincos_cordic(x3, &s, &c, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
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
    q16_t s, c;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q16_sincos_tab(x0, &s, &c);
        q16_sincos_tab(x1, &s, &c);
        q16_sincos_tab(x2, &s, &c);
        q16_sincos_tab(x3, &s, &c);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
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
    q32_t s, c;
    unsigned int precision = 32;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q32_sincos_cordic(x0, &s, &c, precision);
        q32_sincos_cordic(x1, &s, &c, precision);
        q32_sincos_cordic(x2, &s, &c, precision);
        q32_sincos_cordic(x3, &s, &c, precision);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
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
    q32_t s, c;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        q32_sincos_tab(x0, &s, &c);
        q32_sincos_tab(x1, &s, &c);
        q32_sincos_tab(x2, &s, &c);
        q32_sincos_tab(x3, &s, &c);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
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
    float s, c;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        s = sinf(x0);   c = cosf(x0);
        s = sinf(x1);   c = cosf(x1);
        s = sinf(x2);   c = cosf(x2);
        s = sinf(x3);   c = cosf(x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

