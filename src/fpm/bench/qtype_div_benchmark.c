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

#include "liquid.h"
#include "liquidfpm.h"

// 
// BENCHMARK: q16 division
//
void benchmark_q16_div(struct rusage *_start,
                       struct rusage *_finish,
                       unsigned long int *_num_iterations,
                       unsigned int _n)
{
    // initialize arrays
    q16_t x = rand() % 0xffff;
    q16_t y = rand() % 0xffff;
    q16_t z;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        z = q16_div( x, y );
        z = q16_div( x, y );
        z = q16_div( x, y );
        z = q16_div( x, y );
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: q32 division
//
void benchmark_q32_div(struct rusage *_start,
                       struct rusage *_finish,
                       unsigned long int *_num_iterations,
                       unsigned int _n)
{
    // initialize arrays
    q32_t x = rand() % 0xffffffff;
    q32_t y = rand() % 0xffffffff;
    q32_t z;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        z = q32_div( x, y );
        z = q32_div( x, y );
        z = q32_div( x, y );
        z = q32_div( x, y );
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: floating-point division
//
void benchmark_float_div(struct rusage *_start,
                         struct rusage *_finish,
                         unsigned long int *_num_iterations,
                         unsigned int _n)
{
    // initialize arrays
    float x = randf();
    float y = randf();
    float z;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        z = x * y;
        z = x * y;
        z = x * y;
        z = x * y;
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

