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
float qdetector_cccf_bench(unsigned long int _num_iterations,
                           unsigned int      _n)
{
    // generate sequence (random)
    float complex h[_n];
    unsigned long int i;
    for (i=0; i<_n; i++) {
        h[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }

    // generate synchronizer
    int          ftype        = LIQUID_FIRFILT_ARKAISER;
    unsigned int k            =    2;   // samples/symbol
    unsigned int m            =    7;   // filter delay [symbols]
    float        beta         = 0.3f;   // excess bandwidth factor
    float        threshold    = 0.5f;   // threshold for detection
    int          range_index  =    5;   // carrier offset search range [index]
    qdetector_cccf q = qdetector_cccf_create_linear(h, _n, ftype, k, m, beta);
    qdetector_cccf_set_threshold(q,threshold);
    qdetector_cccf_set_range_index(q, range_index);
    //qdetector_cccf_print(q);

    // input sequence (random)
    float complex x[7];
    for (i=0; i<7; i++) {
        x[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f)*_Complex_I;
    }
    // start trials (7 executes per iteration; round down)
    unsigned long int n = _num_iterations / 7;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    int detected = 0;
    for (i=0; i<n; i++) {
        // push input sequence through synchronizer
        detected ^= qdetector_cccf_execute(q, x[0]) != NULL;
        detected ^= qdetector_cccf_execute(q, x[1]) != NULL;
        detected ^= qdetector_cccf_execute(q, x[2]) != NULL;
        detected ^= qdetector_cccf_execute(q, x[3]) != NULL;
        detected ^= qdetector_cccf_execute(q, x[4]) != NULL;
        detected ^= qdetector_cccf_execute(q, x[5]) != NULL;
        detected ^= qdetector_cccf_execute(q, x[6]) != NULL;
        // randomize input
        x[0] += detected > 2 ? -1e-3f : 1e-3f;
    }
    float extime = liquid_toc(timer);
    // clean up allocated objects
    qdetector_cccf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(qdetector_cccf_16,    "qdetector_cccf execute, n=16",    "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 16); }
LIQUID_BENCHMARK(qdetector_cccf_32,    "qdetector_cccf execute, n=32",    "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 32); }
LIQUID_BENCHMARK(qdetector_cccf_64,    "qdetector_cccf execute, n=64",    "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 64); }
LIQUID_BENCHMARK(qdetector_cccf_128,   "qdetector_cccf execute, n=128",   "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 128); }
LIQUID_BENCHMARK(qdetector_cccf_256,   "qdetector_cccf execute, n=256",   "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 256); }
LIQUID_BENCHMARK(qdetector_cccf_512,   "qdetector_cccf execute, n=512",   "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 512); }
LIQUID_BENCHMARK(qdetector_cccf_1024,  "qdetector_cccf execute, n=1024",  "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 1024); }
LIQUID_BENCHMARK(qdetector_cccf_2048,  "qdetector_cccf execute, n=2048",  "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 2048); }
LIQUID_BENCHMARK(qdetector_cccf_4096,  "qdetector_cccf execute, n=4096",  "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 4096); }
LIQUID_BENCHMARK(qdetector_cccf_8192,  "qdetector_cccf execute, n=8192",  "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 8192); }
LIQUID_BENCHMARK(qdetector_cccf_16384, "qdetector_cccf execute, n=16384", "framing,qdetector")
    { return qdetector_cccf_bench(num_iterations, 16384); }

