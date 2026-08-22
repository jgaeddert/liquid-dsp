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
#include "liquid.internal.h"

// Helper function to keep code base small
float presync_cccf_bench(unsigned long int _num_iterations,
                         unsigned int      _n,
                         unsigned int      _m)
{
    // generate sequence (random)
    float complex h[_n];
    unsigned long int i;
    for (i=0; i<_n; i++) {
        h[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    // generate synchronizer
    presync_cccf q = presync_cccf_create(h, _n, 0.1f, _m);
    // input sequence (random)
    float complex x[7];
    for (i=0; i<7; i++) {
        x[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    float complex rxy;
    float dphi_hat;
    // start trials (7 push+execute per iteration; round down)
    unsigned long int n = _num_iterations / 7;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        // push input sequence through synchronizer
        presync_cccf_push(q, x[0]);  presync_cccf_execute(q, &rxy, &dphi_hat);
        presync_cccf_push(q, x[1]);  presync_cccf_execute(q, &rxy, &dphi_hat);
        presync_cccf_push(q, x[2]);  presync_cccf_execute(q, &rxy, &dphi_hat);
        presync_cccf_push(q, x[3]);  presync_cccf_execute(q, &rxy, &dphi_hat);
        presync_cccf_push(q, x[4]);  presync_cccf_execute(q, &rxy, &dphi_hat);
        presync_cccf_push(q, x[5]);  presync_cccf_execute(q, &rxy, &dphi_hat);
        presync_cccf_push(q, x[6]);  presync_cccf_execute(q, &rxy, &dphi_hat);
    }
    float extime = liquid_toc(timer);
    // clean up allocated objects
    presync_cccf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(presync_cccf_16,  "presync_cccf execute, n=16 m=6",  "framing,presync")
    { return presync_cccf_bench(num_iterations, 16,  6); }
LIQUID_BENCHMARK(presync_cccf_32,  "presync_cccf execute, n=32 m=6",  "framing,presync")
    { return presync_cccf_bench(num_iterations, 32,  6); }
LIQUID_BENCHMARK(presync_cccf_64,  "presync_cccf execute, n=64 m=6",  "framing,presync")
    { return presync_cccf_bench(num_iterations, 64,  6); }
LIQUID_BENCHMARK(presync_cccf_128, "presync_cccf execute, n=128 m=6", "framing,presync")
    { return presync_cccf_bench(num_iterations, 128, 6); }
LIQUID_BENCHMARK(presync_cccf_256, "presync_cccf execute, n=256 m=6", "framing,presync")
    { return presync_cccf_bench(num_iterations, 256, 6); }

