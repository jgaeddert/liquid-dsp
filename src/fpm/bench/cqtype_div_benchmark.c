/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// BENCHMARK: cq16 division
//
void benchmark_cq16_div(struct rusage *_start,
                        struct rusage *_finish,
                        unsigned long int *_num_iterations,
                        unsigned int _n)
{
    // initialize values
    cq16_t x  = {q16_float_to_fixed(1.0f), q16_float_to_fixed(-0.9f)};
    cq16_t y0 = x;
    cq16_t y;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = cq16_div( y0, x );
        y = cq16_div( y,  x );
        y = cq16_div( y,  x );
        y = cq16_div( y,  x );
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: cq32 division
//
void benchmark_cq32_div(struct rusage *_start,
                        struct rusage *_finish,
                        unsigned long int *_num_iterations,
                        unsigned int _n)
{
    // initialize values
    cq32_t x  = {q32_float_to_fixed(1.0f), q32_float_to_fixed(-0.9f)};
    cq32_t y0 = x;
    cq32_t y;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = cq32_div( y0, x );
        y = cq32_div( y,  x );
        y = cq32_div( y,  x );
        y = cq32_div( y,  x );
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

// 
// BENCHMARK: complex floating-point division
//
void benchmark_cfloat_div(struct rusage *_start,
                          struct rusage *_finish,
                          unsigned long int *_num_iterations,
                          unsigned int _n)
{
    // initialize arrays
    float complex x  = 1.1 - _Complex_I*0.9f;
    float complex y0 = x;
    float complex y;

    // start trials
    unsigned long int i;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        y = y0 / x;
        y = y  / x;
        y = y  / x;
        y = y  / x;
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

