/*
 * Copyright (c) 2007 - 2021 Joseph Gaeddert
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
float fftfilt_crcf_bench(unsigned long int num_iterations, unsigned int _n)
{
    // generate coefficients
    unsigned int h_len = _n+1;
    float h[h_len];
    unsigned long int i;
    for (i=0; i<h_len; i++)
        h[i] = randnf();

    // create filter object
    fftfilt_crcf q = fftfilt_crcf_create(h,h_len,_n);

    // generate input vector
    float complex x[_n + 4];
    for (i=0; i<_n+4; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // output vector
    float complex y[_n];

    // start trials (4 blocks of _n samples per iteration; round down)
    unsigned long int n = num_iterations / (4 * _n);
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        fftfilt_crcf_execute(q, &x[0], y);
        fftfilt_crcf_execute(q, &x[1], y);
        fftfilt_crcf_execute(q, &x[2], y);
        fftfilt_crcf_execute(q, &x[3], y);
    }
    float extime = liquid_toc(timer);

    // destroy filter object
    fftfilt_crcf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(fftfilt_crcf_4, "fftfilt_crcf execute, n=4", "filter,fftfilt")
    { return fftfilt_crcf_bench(num_iterations, 4); }

LIQUID_BENCHMARK(fftfilt_crcf_8, "fftfilt_crcf execute, n=8", "filter,fftfilt")
    { return fftfilt_crcf_bench(num_iterations, 8); }

LIQUID_BENCHMARK(fftfilt_crcf_16, "fftfilt_crcf execute, n=16", "filter,fftfilt")
    { return fftfilt_crcf_bench(num_iterations, 16); }

LIQUID_BENCHMARK(fftfilt_crcf_32, "fftfilt_crcf execute, n=32", "filter,fftfilt")
    { return fftfilt_crcf_bench(num_iterations, 32); }

LIQUID_BENCHMARK(fftfilt_crcf_64, "fftfilt_crcf execute, n=64", "filter,fftfilt")
    { return fftfilt_crcf_bench(num_iterations, 64); }

