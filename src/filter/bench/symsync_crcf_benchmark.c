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
float symsync_crcf_bench(unsigned long int num_iterations,
                        unsigned int        _k,
                        unsigned int        _m)
{
    unsigned long int i;
    unsigned int npfb = 16;     // number of filters in bank
    unsigned int k    = _k;     // samples/symbol
    unsigned int m    = _m;     // filter delay [symbols]
    float beta        = 0.3f;   // filter excess bandwidth factor

    // create symbol synchronizer
    symsync_crcf q = symsync_crcf_create_rnyquist(LIQUID_FIRFILT_RRC,
                                                  k, m, beta, npfb);

    //
    unsigned int num_samples = 64;
    unsigned int num_written;
    float complex x[num_samples];
    float complex y[num_samples];

    // generate pseudo-random data
    msequence ms = msequence_create_default(6);
    for (i=0; i<num_samples; i++)
        x[i] = ((float)msequence_generate_symbol(ms, 6) - 31.5) / 24.0f;
    msequence_destroy(ms);

    // start trials (4 executes of num_samples input samples each per iteration; round down)
    unsigned long int n = num_iterations / (4 * num_samples);
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        symsync_crcf_execute(q, x, num_samples, y, &num_written);
        symsync_crcf_execute(q, x, num_samples, y, &num_written);
        symsync_crcf_execute(q, x, num_samples, y, &num_written);
        symsync_crcf_execute(q, x, num_samples, y, &num_written);
    }
    float extime = liquid_toc(timer);
    symsync_crcf_destroy(q);
    return extime;
}

//
// BENCHMARKS
//
LIQUID_BENCHMARK(symsync_crcf_k2_m2,  "symsync_crcf execute, k=2 m=2",  "filter,symsync")
    { return symsync_crcf_bench(num_iterations, 2, 2); }

LIQUID_BENCHMARK(symsync_crcf_k2_m4,  "symsync_crcf execute, k=2 m=4",  "filter,symsync")
    { return symsync_crcf_bench(num_iterations, 2, 4); }

LIQUID_BENCHMARK(symsync_crcf_k2_m8,  "symsync_crcf execute, k=2 m=8",  "filter,symsync")
    { return symsync_crcf_bench(num_iterations, 2, 8); }

LIQUID_BENCHMARK(symsync_crcf_k2_m16, "symsync_crcf execute, k=2 m=16", "filter,symsync")
    { return symsync_crcf_bench(num_iterations, 2, 16); }

