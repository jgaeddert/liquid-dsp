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

#include "liquid.benchmark.h"

// Helper function to keep code base small
float firhilbf_decim_bench(unsigned long int num_iterations, unsigned int _m)
{
    // create hilbert transform object
    firhilbf q = firhilbf_create(_m,60.0f);

    float x[] = {1.0f, -1.0f};
    float complex y;
    unsigned long int i;

    // start trials (4 executes of 2 input samples each per iteration; round down)
    unsigned long int n = num_iterations / 8;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        firhilbf_decim_execute(q,x,&y);
        firhilbf_decim_execute(q,x,&y);
        firhilbf_decim_execute(q,x,&y);
        firhilbf_decim_execute(q,x,&y);
    }
    float extime = liquid_toc(timer);

    firhilbf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(firhilbf_decim_m3,  "firhilbf_decim execute, m=3",  "filter,firhilb,halfband,decimator")
    { return firhilbf_decim_bench(num_iterations, 3); }

LIQUID_BENCHMARK(firhilbf_decim_m5,  "firhilbf_decim execute, m=5",  "filter,firhilb,halfband,decimator")
    { return firhilbf_decim_bench(num_iterations, 5); }

LIQUID_BENCHMARK(firhilbf_decim_m9,  "firhilbf_decim execute, m=9",  "filter,firhilb,halfband,decimator")
    { return firhilbf_decim_bench(num_iterations, 9); }

LIQUID_BENCHMARK(firhilbf_decim_m13, "firhilbf_decim execute, m=13", "filter,firhilb,halfband,decimator")
    { return firhilbf_decim_bench(num_iterations, 13); }

