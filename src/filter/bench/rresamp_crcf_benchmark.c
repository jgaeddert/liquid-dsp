/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
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
#include <stdlib.h>

// Helper function to keep code base small
float rresamp_crcf_bench(unsigned long int num_iterations,
                        unsigned int        _P,
                        unsigned int        _Q)
{
    // create resampling object
    unsigned int m  = 12;
    float        bw = 0.45f;
    float        As = 60.0f;
    rresamp_crcf q = rresamp_crcf_create_kaiser(_P,_Q,m,bw,As);

    // input/output buffers
    unsigned int buf_len = _P > _Q ? _P : _Q; // max(_P,_Q)
    float complex * buf = (float complex*) malloc(buf_len*sizeof(float complex));

    // initialize buffer
    unsigned long int i;
    for (i=0; i<buf_len; i++)
        buf[i] = i==0 ? 1.0 : 0.0;

    // start trials (4 executes of buf_len input samples each per iteration; round down)
    unsigned long int n = num_iterations / (4 * buf_len);
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        rresamp_crcf_execute(q, buf, buf);
        rresamp_crcf_execute(q, buf, buf);
        rresamp_crcf_execute(q, buf, buf);
        rresamp_crcf_execute(q, buf, buf);
        buf[0] = 1.0f;
    }
    float extime = liquid_toc(timer);

    free(buf);
    rresamp_crcf_destroy(q);
    return extime;
}

//
// Rational-rate resampler benchmark prototypes; compare to arbitrary rate resampler
//
LIQUID_BENCHMARK(rresamp_crcf_P17_Q1,   "rresamp_crcf execute, P=17 Q=1",   "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17,   1); }

LIQUID_BENCHMARK(rresamp_crcf_P17_Q2,   "rresamp_crcf execute, P=17 Q=2",   "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17,   2); }

LIQUID_BENCHMARK(rresamp_crcf_P17_Q4,   "rresamp_crcf execute, P=17 Q=4",   "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17,   4); }

LIQUID_BENCHMARK(rresamp_crcf_P17_Q8,   "rresamp_crcf execute, P=17 Q=8",   "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17,   8); }

LIQUID_BENCHMARK(rresamp_crcf_P17_Q16,  "rresamp_crcf execute, P=17 Q=16",  "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17,  16); }

LIQUID_BENCHMARK(rresamp_crcf_P17_Q32,  "rresamp_crcf execute, P=17 Q=32",  "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17,  32); }

LIQUID_BENCHMARK(rresamp_crcf_P17_Q64,  "rresamp_crcf execute, P=17 Q=64",  "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17,  64); }

LIQUID_BENCHMARK(rresamp_crcf_P17_Q128, "rresamp_crcf execute, P=17 Q=128", "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17, 128); }

LIQUID_BENCHMARK(rresamp_crcf_P17_Q256, "rresamp_crcf execute, P=17 Q=256", "filter,rresamp")
    { return rresamp_crcf_bench(num_iterations, 17, 256); }

