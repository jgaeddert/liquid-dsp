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
float firinterp_crcf_bench(unsigned long int num_iterations,
                          unsigned int _M,
                          unsigned int _h_len)
{
    float h[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++)
        h[i] = 1.0f;

    firinterp_crcf q = firinterp_crcf_create(_M,h,_h_len);

    float complex y[_M];
    // start trials (4 executes of 1 input sample each per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        firinterp_crcf_execute(q,1.0f,y);
        firinterp_crcf_execute(q,1.0f,y);
        firinterp_crcf_execute(q,1.0f,y);
        firinterp_crcf_execute(q,1.0f,y);
    }
    float extime = liquid_toc(timer);

    firinterp_crcf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(firinterp_crcf_m2_h8,    "firinterp_crcf execute, M=2 h_len=8",   "filter,firinterp,interpolator")
    { return firinterp_crcf_bench(num_iterations, 2,   8); }

LIQUID_BENCHMARK(firinterp_crcf_m4_h16,   "firinterp_crcf execute, M=4 h_len=16",  "filter,firinterp,interpolator")
    { return firinterp_crcf_bench(num_iterations, 4,  16); }

LIQUID_BENCHMARK(firinterp_crcf_m8_h32,   "firinterp_crcf execute, M=8 h_len=32",  "filter,firinterp,interpolator")
    { return firinterp_crcf_bench(num_iterations, 8,  32); }

LIQUID_BENCHMARK(firinterp_crcf_m16_h64,  "firinterp_crcf execute, M=16 h_len=64", "filter,firinterp,interpolator")
    { return firinterp_crcf_bench(num_iterations, 16, 64); }

LIQUID_BENCHMARK(firinterp_crcf_m32_h128, "firinterp_crcf execute, M=32 h_len=128","filter,firinterp,interpolator")
    { return firinterp_crcf_bench(num_iterations, 32, 128); }

