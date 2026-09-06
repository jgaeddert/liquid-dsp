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
float firfilt_crcf_bench(unsigned long int num_iterations, unsigned int _n)
{
    // generate coefficients
    float h[_n];
    unsigned long int i;
    for (i=0; i<_n; i++)
        h[i] = randnf();

    // create filter object
    firfilt_crcf f = firfilt_crcf_create(h,_n);

    // generate input vector
    float complex x[4];
    for (i=0; i<4; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // output vector
    float complex y[4];

    // start trials (4 work units per iteration; round down to multiple of 4)
    unsigned long int n = num_iterations / 4;
    liquid_timer q = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        firfilt_crcf_push(f, x[0]); firfilt_crcf_execute(f, &y[0]);
        firfilt_crcf_push(f, x[1]); firfilt_crcf_execute(f, &y[1]);
        firfilt_crcf_push(f, x[2]); firfilt_crcf_execute(f, &y[2]);
        firfilt_crcf_push(f, x[3]); firfilt_crcf_execute(f, &y[3]);
    }
    float extime = liquid_toc(q);

    firfilt_crcf_destroy(f);
    return extime;
}

LIQUID_BENCHMARK(firfilt_crcf_4, "firfilt_crcf execute, n=4", "filter,firfilt")
    { return firfilt_crcf_bench(num_iterations, 4); }

LIQUID_BENCHMARK(firfilt_crcf_8, "firfilt_crcf execute, n=8", "filter,firfilt")
    { return firfilt_crcf_bench(num_iterations, 8); }

LIQUID_BENCHMARK(firfilt_crcf_16, "firfilt_crcf execute, n=16", "filter,firfilt")
    { return firfilt_crcf_bench(num_iterations, 16); }

LIQUID_BENCHMARK(firfilt_crcf_32, "firfilt_crcf execute, n=32", "filter,firfilt")
    { return firfilt_crcf_bench(num_iterations, 32); }

LIQUID_BENCHMARK(firfilt_crcf_64, "firfilt_crcf execute, n=64", "filter,firfilt")
    { return firfilt_crcf_bench(num_iterations, 64); }

