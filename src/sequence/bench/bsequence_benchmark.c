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
float bsequence_correlate_bench(unsigned long int _num_iterations,
                                unsigned int      _n)
{
    // create and initialize binary sequences
    bsequence bs1 = bsequence_create(_n);
    bsequence bs2 = bsequence_create(_n);

    unsigned long int i;
    int rxy = 0;

    // start trials (4 correlates per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        rxy += bsequence_correlate(bs1, bs2);
        rxy -= bsequence_correlate(bs1, bs2);
        rxy += bsequence_correlate(bs1, bs2);
        rxy -= bsequence_correlate(bs1, bs2);

        bsequence_push(rxy > 0 ? bs1 : bs2, 1);
    }
    float extime = liquid_toc(timer);

    // clean up memory
    bsequence_destroy(bs1);
    bsequence_destroy(bs2);
    return extime;
}

LIQUID_BENCHMARK(bsequence_xcorr_n16,   "bsequence_correlate, n=16",   "sequence")
    { return bsequence_correlate_bench(num_iterations, 16); }
LIQUID_BENCHMARK(bsequence_xcorr_n64,   "bsequence_correlate, n=64",   "sequence")
    { return bsequence_correlate_bench(num_iterations, 64); }
LIQUID_BENCHMARK(bsequence_xcorr_n256,  "bsequence_correlate, n=256",  "sequence")
    { return bsequence_correlate_bench(num_iterations, 256); }
LIQUID_BENCHMARK(bsequence_xcorr_n1024, "bsequence_correlate, n=1024", "sequence")
    { return bsequence_correlate_bench(num_iterations, 1024); }

