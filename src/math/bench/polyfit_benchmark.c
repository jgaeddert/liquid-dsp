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
float polyfit_bench(unsigned long int _num_iterations,
                      unsigned int      _Q,
                      unsigned int      _N)
{
    float p[_Q+1];

    float x[_N];
    float y[_N];
    unsigned int i;
    for (i=0; i<_N; i++) {
        x[i] = randnf();
        y[i] = randnf();
    }
    
    // start trials
    unsigned long int n = _num_iterations / 4;
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        polyf_fit(x,y,_N, p,_Q+1);
        polyf_fit(x,y,_N, p,_Q+1);
        polyf_fit(x,y,_N, p,_Q+1);
        polyf_fit(x,y,_N, p,_Q+1);
    }
    float extime = liquid_toc(timer);
    return extime;
}

LIQUID_BENCHMARK(polyfit_q3_n8,   "polyf_fit execute, q=3 n=8",   "math,polyfit")
    { return polyfit_bench(num_iterations, 3, 8); }
LIQUID_BENCHMARK(polyfit_q3_n16,  "polyf_fit execute, q=3 n=16",  "math,polyfit")
    { return polyfit_bench(num_iterations, 3, 16); }
LIQUID_BENCHMARK(polyfit_q3_n32,  "polyf_fit execute, q=3 n=32",  "math,polyfit")
    { return polyfit_bench(num_iterations, 3, 32); }
LIQUID_BENCHMARK(polyfit_q3_n64,  "polyf_fit execute, q=3 n=64",  "math,polyfit")
    { return polyfit_bench(num_iterations, 3, 64); }
LIQUID_BENCHMARK(polyfit_q3_n128, "polyf_fit execute, q=3 n=128", "math,polyfit")
    { return polyfit_bench(num_iterations, 3, 128); }

