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
float detector_cccf_bench(unsigned long int num_iterations, unsigned int _n)
{
    // generate sequence (random)
    float complex h[_n];
    unsigned long int i;
    for (i=0; i<_n; i++) {
        h[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    // generate synchronizer
    float threshold = 0.5f;
    float dphi_max  = 0.07f;
    detector_cccf q = detector_cccf_create(h, _n, threshold, dphi_max);
    // input sequence (random)
    float complex x[7];
    for (i=0; i<7; i++) {
        x[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    float tau_hat;
    float dphi_hat;
    float gamma_hat;
    // start trials (7 correlates per iteration; round down)
    unsigned long int n = num_iterations / 7;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    int detected = 0;
    for (i=0; i<n; i++) {
        // push input sequence through synchronizer
        detected ^= detector_cccf_correlate(q, x[0], &tau_hat, & dphi_hat, &gamma_hat);
        detected ^= detector_cccf_correlate(q, x[1], &tau_hat, & dphi_hat, &gamma_hat);
        detected ^= detector_cccf_correlate(q, x[2], &tau_hat, & dphi_hat, &gamma_hat);
        detected ^= detector_cccf_correlate(q, x[3], &tau_hat, & dphi_hat, &gamma_hat);
        detected ^= detector_cccf_correlate(q, x[4], &tau_hat, & dphi_hat, &gamma_hat);
        detected ^= detector_cccf_correlate(q, x[5], &tau_hat, & dphi_hat, &gamma_hat);
        detected ^= detector_cccf_correlate(q, x[6], &tau_hat, & dphi_hat, &gamma_hat);

        // randomize input
        x[0] += detected > 2 ? -1e-3f : 1e-3f;
    }
    float extime = liquid_toc(timer);
    // clean up allocated objects
    detector_cccf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(detector_cccf_16,  "detector_cccf correlate, n=16",  "framing,detector")
    { return detector_cccf_bench(num_iterations, 16); }
LIQUID_BENCHMARK(detector_cccf_32,  "detector_cccf correlate, n=32",  "framing,detector")
    { return detector_cccf_bench(num_iterations, 32); }
LIQUID_BENCHMARK(detector_cccf_64,  "detector_cccf correlate, n=64",  "framing,detector")
    { return detector_cccf_bench(num_iterations, 64); }
LIQUID_BENCHMARK(detector_cccf_128, "detector_cccf correlate, n=128", "framing,detector")
    { return detector_cccf_bench(num_iterations, 128); }
LIQUID_BENCHMARK(detector_cccf_256, "detector_cccf correlate, n=256", "framing,detector")
    { return detector_cccf_bench(num_iterations, 256); }

