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
#include <string.h>

// Helper function to keep code base small
float window_push_bench(unsigned long int num_iterations, unsigned int _n)
{
    // initialize port
    windowcf w = windowcf_create(_n);

    // start trials (4 single-sample pushes per iteration; round down)
    unsigned long int i, n = num_iterations / 4;
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        windowcf_push(w, 1.0f);
        windowcf_push(w, 1.0f);
        windowcf_push(w, 1.0f);
        windowcf_push(w, 1.0f);
    }
    float extime = liquid_toc(timer);

    windowcf_destroy(w);
    return extime;
}

LIQUID_BENCHMARK(windowcf_push_n16,  "windowcf push, n=16",  "buffer,window")
    { return window_push_bench(num_iterations, 16); }

LIQUID_BENCHMARK(windowcf_push_n32,  "windowcf push, n=32",  "buffer,window")
    { return window_push_bench(num_iterations, 32); }

LIQUID_BENCHMARK(windowcf_push_n64,  "windowcf push, n=64",  "buffer,window")
    { return window_push_bench(num_iterations, 64); }

LIQUID_BENCHMARK(windowcf_push_n128, "windowcf push, n=128", "buffer,window")
    { return window_push_bench(num_iterations, 128); }

LIQUID_BENCHMARK(windowcf_push_n256, "windowcf push, n=256", "buffer,window")
    { return window_push_bench(num_iterations, 256); }


// Helper function to keep code base small
float window_write_bench(unsigned long int num_iterations, unsigned int _n)
{
    // initialize port
    windowcf w = windowcf_create(_n);

    // create buffer with random values for writing
    float complex * buf = (float complex*)malloc(960*sizeof(float complex));
    memset(buf, 0x00, 960*sizeof(float complex));

    // start trials (4 block-sized writes per iteration)
    unsigned long int i, n = 1 + (num_iterations / (521 + 761 + 331 + 960));
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        windowcf_write(w, buf, 521);
        windowcf_write(w, buf, 761);
        windowcf_write(w, buf, 331);
        windowcf_write(w, buf, 960);
    }
    float extime = liquid_toc(timer);

    // clean up allocations
    free(buf);
    windowcf_destroy(w);
    return extime;
}

LIQUID_BENCHMARK(windowcf_write_n16,  "windowcf write, n=16",  "buffer,window")
    { return window_write_bench(num_iterations, 16); }

LIQUID_BENCHMARK(windowcf_write_n64,  "windowcf write, n=64",  "buffer,window")
    { return window_write_bench(num_iterations, 64); }

LIQUID_BENCHMARK(windowcf_write_n256, "windowcf write, n=256", "buffer,window")
    { return window_write_bench(num_iterations, 256); }

LIQUID_BENCHMARK(windowcf_write_n1024, "windowcf write, n=1024", "buffer,window")
    { return window_write_bench(num_iterations, 1024); }

LIQUID_BENCHMARK(windowcf_write_n4096, "windowcf write, n=4096", "buffer,window")
    { return window_write_bench(num_iterations, 4096); }

