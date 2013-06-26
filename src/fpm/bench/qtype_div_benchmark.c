/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
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
    // initialize values
    q16_t y;
    q16_t x = q16_float_to_fixed(1.1f);

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q16_div( q16_one, x );
        y = q16_div( y,       x );
        y = q16_div( y,       x );
        y = q16_div( y,       x );
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
    // initialize values
    q32_t y;
    q32_t x = q32_float_to_fixed(1.1f);

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = q32_div( q32_one, x );
        y = q32_div( y,       x );
        y = q32_div( y,       x );
        y = q32_div( y,       x );
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
    float x = 1.1;
    float y;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = 1. / x;
        y = y  / x;
        y = y  / x;
        y = y  / x;
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

