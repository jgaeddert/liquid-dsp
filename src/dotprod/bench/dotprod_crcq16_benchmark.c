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

// Helper function to keep code base small
void dotprod_crcq16_bench(struct rusage *_start,
                          struct rusage *_finish,
                          unsigned long int *_num_iterations,
                          unsigned int _n)
{
    // normalize number of iterations
    *_num_iterations *= 128;
    *_num_iterations /= _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    q16_t  h[_n];
    cq16_t x[_n];
    cq16_t y;
    unsigned int i;
    for (i=0; i<_n; i++) {
        h[i] = rand() % q16_one;
        
        x[i].real = rand() % q16_one;
        x[i].imag = rand() % q16_one;
    }

    // create dotprod structure;
    dotprod_crcq16 q = dotprod_crcq16_create(h,_n);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        dotprod_crcq16_execute(q, x, &y);
        dotprod_crcq16_execute(q, x, &y);
        dotprod_crcq16_execute(q, x, &y);
        dotprod_crcq16_execute(q, x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up objects
    dotprod_crcq16_destroy(q);
}

#define DOTPROD_CRCQ16_BENCHMARK_API(N) \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ dotprod_crcq16_bench(_start, _finish, _num_iterations, N); }

void benchmark_dotprod_crcq16_4       DOTPROD_CRCQ16_BENCHMARK_API(4)
void benchmark_dotprod_crcq16_16      DOTPROD_CRCQ16_BENCHMARK_API(16)
void benchmark_dotprod_crcq16_64      DOTPROD_CRCQ16_BENCHMARK_API(64)
void benchmark_dotprod_crcq16_256     DOTPROD_CRCQ16_BENCHMARK_API(256)

