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
float vectorf_bench(unsigned long int _num_iterations,
                    unsigned int      _n)
{
    // allocate buffers
    float buf_0[_n];
    float buf_1[_n];
    float buf_2[_n];
    unsigned int i;
    for (i=0; i<_n; i++) {
        buf_0[i] = randnf();
        buf_1[i] = randnf();
    }

    // start trials (1 mul of _n samples per iteration; round down)
    unsigned long int n = _num_iterations / _n;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        // run vector multiplication
        liquid_vectorf_mul(buf_0, buf_1, _n, buf_2);

        // ensure the compiler doesn't optimize this out
        buf_0[i % _n] += buf_2[0];
    }
    float extime = liquid_toc(timer);
    return extime;
}

LIQUID_BENCHMARK(vectorf_4,    "liquid_vectorf_mul, n=4",    "vector")
    { return vectorf_bench(num_iterations, 4); }
LIQUID_BENCHMARK(vectorf_16,   "liquid_vectorf_mul, n=16",   "vector")
    { return vectorf_bench(num_iterations, 16); }
LIQUID_BENCHMARK(vectorf_64,   "liquid_vectorf_mul, n=64",   "vector")
    { return vectorf_bench(num_iterations, 64); }
LIQUID_BENCHMARK(vectorf_256,  "liquid_vectorf_mul, n=256",  "vector")
    { return vectorf_bench(num_iterations, 256); }
LIQUID_BENCHMARK(vectorf_1024, "liquid_vectorf_mul, n=1024", "vector")
    { return vectorf_bench(num_iterations, 1024); }

