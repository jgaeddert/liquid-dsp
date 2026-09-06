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
float interleaver_bench(unsigned long int _num_iterations,
                        unsigned int      _n)
{
    // initialize interleaver
    interleaver q = interleaver_create(_n);
    interleaver_set_depth(q, 4);

    unsigned char x[_n];
    unsigned char y[_n];
    
    unsigned long int i;
    for (i=0; i<_n; i++)
        x[i] = rand() & 0xff;

    // start trials (4 interleaves per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        interleaver_encode(q, x, y);
        interleaver_encode(q, x, y);
        interleaver_encode(q, x, y);
        interleaver_encode(q, x, y);
    }
    float extime = liquid_toc(timer);

    // destroy interleaver object
    interleaver_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(interleaver_8,
    "interleaver_encode, n=8",
    "fec,interleaver")
{ return interleaver_bench(num_iterations, 8); }

LIQUID_BENCHMARK(interleaver_16,
    "interleaver_encode, n=16",
    "fec,interleaver")
{ return interleaver_bench(num_iterations, 16); }

LIQUID_BENCHMARK(interleaver_32,
    "interleaver_encode, n=32",
    "fec,interleaver")
{ return interleaver_bench(num_iterations, 32); }

LIQUID_BENCHMARK(interleaver_64,
    "interleaver_encode, n=64",
    "fec,interleaver")
{ return interleaver_bench(num_iterations, 64); }

LIQUID_BENCHMARK(interleaver_128,
    "interleaver_encode, n=128",
    "fec,interleaver")
{ return interleaver_bench(num_iterations, 128); }

LIQUID_BENCHMARK(interleaver_256,
    "interleaver_encode, n=256",
    "fec,interleaver")
{ return interleaver_bench(num_iterations, 256); }

LIQUID_BENCHMARK(interleaver_512,
    "interleaver_encode, n=512",
    "fec,interleaver")
{ return interleaver_bench(num_iterations, 512); }

LIQUID_BENCHMARK(interleaver_1024,
    "interleaver_encode, n=1024",
    "fec,interleaver")
{ return interleaver_bench(num_iterations, 1024); }

