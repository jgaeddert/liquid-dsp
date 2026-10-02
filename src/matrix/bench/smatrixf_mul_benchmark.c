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
#include <stdlib.h>
#include "liquid.h"

// Helper function to keep code base small
float smatrixf_mul_bench(unsigned long int num_iterations, unsigned int _n)
{
    unsigned long int i;

    // generate random matrices
    smatrixf a = smatrixf_create(_n, _n);
    smatrixf b = smatrixf_create(_n, _n);
    smatrixf c = smatrixf_create(_n, _n);

    // number of random non-zero entries
    unsigned int nnz = _n / 20 < 4 ? 4 : _n / 20;

    // initialize _a
    for (i=0; i<nnz; i++) {
        unsigned int row = rand() % _n;
        unsigned int col = rand() % _n;
        float value      = randf();
        smatrixf_set(a, row, col, value);
    }
    
    // initialize _b
    for (i=0; i<nnz; i++) {
        unsigned int row = rand() % _n;
        unsigned int col = rand() % _n;
        float value      = randf();
        smatrixf_set(b, row, col, value);
    }
    // initialize c with first multiplication
    smatrixf_mul(a,b,c);
    // start trials (4 multiplications per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        smatrixf_mul(a,b,c);
        smatrixf_mul(a,b,c);
        smatrixf_mul(a,b,c);
        smatrixf_mul(a,b,c);
    }
    float extime = liquid_toc(timer);
    // free smatrix objects
    smatrixf_destroy(a);
    smatrixf_destroy(b);
    smatrixf_destroy(c);
    return extime;
}

LIQUID_BENCHMARK(smatrixf_mul_n32,  "smatrixf_mul execute, n=32",  "matrix,smatrix,mul")
    { return smatrixf_mul_bench(num_iterations, 32); }
LIQUID_BENCHMARK(smatrixf_mul_n64,  "smatrixf_mul execute, n=64",  "matrix,smatrix,mul")
    { return smatrixf_mul_bench(num_iterations, 64); }
LIQUID_BENCHMARK(smatrixf_mul_n128, "smatrixf_mul execute, n=128", "matrix,smatrix,mul")
    { return smatrixf_mul_bench(num_iterations, 128); }
LIQUID_BENCHMARK(smatrixf_mul_n256, "smatrixf_mul execute, n=256", "matrix,smatrix,mul")
    { return smatrixf_mul_bench(num_iterations, 256); }
LIQUID_BENCHMARK(smatrixf_mul_n512, "smatrixf_mul execute, n=512", "matrix,smatrix,mul")
    { return smatrixf_mul_bench(num_iterations, 512); }

