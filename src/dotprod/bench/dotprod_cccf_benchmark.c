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
float dotprod_cccf_bench(unsigned long int num_iterations, unsigned int _n)
{
    float complex x[_n];
    float complex h[_n];
    float complex y[4];
    unsigned int i;
    for (i=0; i<_n; i++) {
        x[i] = randnf() + _Complex_I*randnf();
        h[i] = randnf() + _Complex_I*randnf();
    }

    // create dotprod structure
    dotprod_cccf dp = dotprod_cccf_create(h,_n);

    // start trials (4 executes of _n samples each per iteration; round down)
    unsigned long int n = num_iterations / (4 * _n);
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        dotprod_cccf_execute(dp, x, &y[0]);
        dotprod_cccf_execute(dp, x, &y[1]);
        dotprod_cccf_execute(dp, x, &y[2]);
        dotprod_cccf_execute(dp, x, &y[3]);
    }
    float extime = liquid_toc(timer);

    // clean up objects
    dotprod_cccf_destroy(dp);
    return extime;
}

LIQUID_BENCHMARK(dotprod_cccf_4,   "dotprod_cccf execute, n=4",   "dotprod")
    { return dotprod_cccf_bench(num_iterations, 4); }

LIQUID_BENCHMARK(dotprod_cccf_16,  "dotprod_cccf execute, n=16",  "dotprod")
    { return dotprod_cccf_bench(num_iterations, 16); }

LIQUID_BENCHMARK(dotprod_cccf_64,  "dotprod_cccf execute, n=64",  "dotprod")
    { return dotprod_cccf_bench(num_iterations, 64); }

LIQUID_BENCHMARK(dotprod_cccf_256, "dotprod_cccf execute, n=256", "dotprod")
    { return dotprod_cccf_bench(num_iterations, 256); }

