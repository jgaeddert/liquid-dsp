/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include "liquid.benchmark.h"

// Helper function to keep code base small
float sumsqcf_bench(unsigned long int num_iterations, unsigned int _n)
{
    float complex x[_n];
    float complex y = 0.0f;
    unsigned long int i;
    for (i=0; i<_n; i++)
        x[i] = 0.2f + 0.2f*_Complex_I;

    // start trials (4 calls of _n samples each per iteration; round down)
    unsigned long int n = num_iterations / (4 * _n);
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        y += liquid_sumsqcf(x, _n);
        y -= liquid_sumsqcf(x, _n);
        y += liquid_sumsqcf(x, _n);
        y -= liquid_sumsqcf(x, _n);

        // change input
        x[i%_n] = y;
    }
    float extime = liquid_toc(timer);
    return extime;
}

LIQUID_BENCHMARK(sumsqcf_4,   "liquid_sumsqcf execute, n=4",   "dotprod,sumsq")
    { return sumsqcf_bench(num_iterations, 4); }

LIQUID_BENCHMARK(sumsqcf_16,  "liquid_sumsqcf execute, n=16",  "dotprod,sumsq")
    { return sumsqcf_bench(num_iterations, 16); }

LIQUID_BENCHMARK(sumsqcf_64,  "liquid_sumsqcf execute, n=64",  "dotprod,sumsq")
    { return sumsqcf_bench(num_iterations, 64); }

LIQUID_BENCHMARK(sumsqcf_256, "liquid_sumsqcf execute, n=256", "dotprod,sumsq")
    { return sumsqcf_bench(num_iterations, 256); }

