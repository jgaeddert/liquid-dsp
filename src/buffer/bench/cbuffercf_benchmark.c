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
float cbuffercf_bench(unsigned long int num_iterations,
                    unsigned int        _n,
                    unsigned int        _write_size,
                    unsigned int        _read_size)
{
    // create object
    cbuffercf q = cbuffercf_create(_n);

    // 
    float complex   v[_write_size]; // array for writing
    float complex * r;              // read pointer
    unsigned int num_requested;     // number of elements requested
    unsigned int num_read;          // number of elements read

    // initialize array for writing
    unsigned int i;
    for (i=0; i<_write_size; i++)
        v[i] = 0.0f;

    // accumulate total number of elements (target = num_iterations)
    unsigned long int num_total_elements = 0;

    // start trials; loop until target number of elements have passed through
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    while (num_total_elements < num_iterations) {
        // write elements to buffer if space is available
        if (_n - cbuffercf_size(q) > _write_size)
            cbuffercf_write(q, v, _write_size);

        // read up to '_read_size' elements
        num_requested = _read_size;
        cbuffercf_read(q, num_requested, &r, &num_read);

        // release elements that were read
        cbuffercf_release(q, num_read);

        // increment counter by number of elements passing through
        num_total_elements += num_read;
    }
    float extime = liquid_toc(timer);


    // clean up allocated memory
    cbuffercf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(cbuffercf_n16,    "cbuffercf read/write, n=16",    "buffer,circular")
    { return cbuffercf_bench(num_iterations,   16,  12,  11); }


LIQUID_BENCHMARK(cbuffercf_n32,    "cbuffercf read/write, n=32",    "buffer,circular")
    { return cbuffercf_bench(num_iterations,   32,  24,  23); }

LIQUID_BENCHMARK(cbuffercf_n64,    "cbuffercf read/write, n=64",    "buffer,circular")
    { return cbuffercf_bench(num_iterations,   64,  48,  47); }

LIQUID_BENCHMARK(cbuffercf_n128,   "cbuffercf read/write, n=128",   "buffer,circular")
    { return cbuffercf_bench(num_iterations,  128,  96,  95); }

LIQUID_BENCHMARK(cbuffercf_n256,   "cbuffercf read/write, n=256",   "buffer,circular")
    { return cbuffercf_bench(num_iterations,  256, 192, 191); }

LIQUID_BENCHMARK(cbuffercf_n512,   "cbuffercf read/write, n=512",   "buffer,circular")
    { return cbuffercf_bench(num_iterations,  512, 384, 383); }

LIQUID_BENCHMARK(cbuffercf_n1024,  "cbuffercf read/write, n=1024",  "buffer,circular")
    { return cbuffercf_bench(num_iterations, 1024, 768, 767); }

