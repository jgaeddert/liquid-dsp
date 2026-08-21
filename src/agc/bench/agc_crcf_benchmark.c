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

// benchmark: automatic gain control (agc_crcf)
LIQUID_BENCHMARK(agc_crcf, "agc_crcf execute", "AGC")
{
    // initialize AGC object
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, 0.05f);

    float complex x = 1e-6f;    // input sample
    float complex y;            // output sample

    // start trials (8 single-sample executes per iteration; round down)
    unsigned long int i, n = num_iterations / 8;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i = 0; i < n; i++) {
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
        agc_crcf_execute(q, x, &y);
    }
    float extime = liquid_toc(timer);

    // destroy object
    agc_crcf_destroy(q);
    return extime;
}
