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

