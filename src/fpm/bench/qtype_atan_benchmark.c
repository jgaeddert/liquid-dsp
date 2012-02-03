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
    q16_t r, theta;
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
    q16_t theta;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        theta = q16_atan2_frac(x0, y0);
        theta = q16_atan2_frac(x1, y1);
        theta = q16_atan2_frac(x2, y2);
        theta = q16_atan2_frac(x3, y3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
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
    q32_t r, theta;
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
    q32_t theta;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        theta = q32_atan2_frac(x0, y0);
        theta = q32_atan2_frac(x1, y1);
        theta = q32_atan2_frac(x2, y2);
        theta = q32_atan2_frac(x3, y3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
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
    float theta;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        theta = atan2f(y0, x0);
        theta = atan2f(y1, x1);
        theta = atan2f(y2, x2);
        theta = atan2f(y3, x3);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

