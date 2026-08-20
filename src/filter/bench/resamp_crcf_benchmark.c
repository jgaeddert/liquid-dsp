/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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
#include <math.h>

// Helper function to keep code base small
float resamp_crcf_bench(unsigned long int num_iterations,
                       unsigned int        _P,
                       unsigned int        _Q)
{
    // create resampling object; irrational rate is just less than Q/P
    float        rate = (float)_Q/(float)_P*sqrt(3301.0f/3302.0f);
    unsigned int m    = 12;     // filter semi-length
    float        bw   = 0.45f;  // filter bandwidth
    float        As   = 60.0f;  // stop-band attenuation [dB]
    unsigned int npfb = 64;     // number of polyphase filters
    resamp_crcf q = resamp_crcf_create(rate,m,bw,As,npfb);

    // buffering
    float complex buf_0[_P];
    float complex buf_1[_Q*4];
    unsigned int num_written;
    
    unsigned long int i;
    for (i=0; i<_P; i++)
        buf_0[i] = i % 7 ? 1 : -1;

    // start trials (4 execute_blocks of _P input samples each per iteration; round down)
    unsigned long int n = num_iterations / (4 * _P);
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        resamp_crcf_execute_block(q, buf_0, _P, buf_1, &num_written);
        resamp_crcf_execute_block(q, buf_0, _P, buf_1, &num_written);
        resamp_crcf_execute_block(q, buf_0, _P, buf_1, &num_written);
        resamp_crcf_execute_block(q, buf_0, _P, buf_1, &num_written);
    }
    float extime = liquid_toc(timer);

    // destroy object
    resamp_crcf_destroy(q);
    return extime;
}

//
// Resampler benchmark prototypes; compare to rational rate resampler
//
LIQUID_BENCHMARK(resamp_crcf_P17_Q1,   "resamp_crcf execute_block, P=17 Q=1",   "resampler")
    { return resamp_crcf_bench(num_iterations, 17,   1); }

LIQUID_BENCHMARK(resamp_crcf_P17_Q2,   "resamp_crcf execute_block, P=17 Q=2",   "resampler")
    { return resamp_crcf_bench(num_iterations, 17,   2); }

LIQUID_BENCHMARK(resamp_crcf_P17_Q4,   "resamp_crcf execute_block, P=17 Q=4",   "resampler")
    { return resamp_crcf_bench(num_iterations, 17,   4); }

LIQUID_BENCHMARK(resamp_crcf_P17_Q8,   "resamp_crcf execute_block, P=17 Q=8",   "resampler")
    { return resamp_crcf_bench(num_iterations, 17,   8); }

LIQUID_BENCHMARK(resamp_crcf_P17_Q16,  "resamp_crcf execute_block, P=17 Q=16",  "resampler")
    { return resamp_crcf_bench(num_iterations, 17,  16); }

LIQUID_BENCHMARK(resamp_crcf_P17_Q32,  "resamp_crcf execute_block, P=17 Q=32",  "resampler")
    { return resamp_crcf_bench(num_iterations, 17,  32); }

LIQUID_BENCHMARK(resamp_crcf_P17_Q64,  "resamp_crcf execute_block, P=17 Q=64",  "resampler")
    { return resamp_crcf_bench(num_iterations, 17,  64); }

LIQUID_BENCHMARK(resamp_crcf_P17_Q128, "resamp_crcf execute_block, P=17 Q=128", "resampler")
    { return resamp_crcf_bench(num_iterations, 17, 128); }

LIQUID_BENCHMARK(resamp_crcf_P17_Q256, "resamp_crcf execute_block, P=17 Q=256", "resampler")
    { return resamp_crcf_bench(num_iterations, 17, 256); }

