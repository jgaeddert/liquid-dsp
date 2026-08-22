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
float matrixf_linsolve_bench(unsigned long int num_iterations, unsigned int _n)
{
    unsigned long int i;

    float A[_n*_n];
    float b[_n];
    float x[_n];
    for (i=0; i<_n*_n; i++)
        A[i] = randnf();
    for (i=0; i<_n; i++)
        b[i] = randnf();
    
    // start trials (4 solves per iteration; round down)
    unsigned long int n = num_iterations / 4;
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        matrixf_linsolve(A,_n,b,x,NULL);
        matrixf_linsolve(A,_n,b,x,NULL);
        matrixf_linsolve(A,_n,b,x,NULL);
        matrixf_linsolve(A,_n,b,x,NULL);
    }
    float extime = liquid_toc(timer);
    return extime;
}

LIQUID_BENCHMARK(matrixf_linsolve_n2,  "matrixf_linsolve execute, n=2",  "matrix,linsolve")
    { return matrixf_linsolve_bench(num_iterations, 2); }
LIQUID_BENCHMARK(matrixf_linsolve_n4,  "matrixf_linsolve execute, n=4",  "matrix,linsolve")
    { return matrixf_linsolve_bench(num_iterations, 4); }
LIQUID_BENCHMARK(matrixf_linsolve_n8,  "matrixf_linsolve execute, n=8",  "matrix,linsolve")
    { return matrixf_linsolve_bench(num_iterations, 8); }
LIQUID_BENCHMARK(matrixf_linsolve_n16, "matrixf_linsolve execute, n=16", "matrix,linsolve")
    { return matrixf_linsolve_bench(num_iterations, 16); }
LIQUID_BENCHMARK(matrixf_linsolve_n32, "matrixf_linsolve execute, n=32", "matrix,linsolve")
    { return matrixf_linsolve_bench(num_iterations, 32); }
LIQUID_BENCHMARK(matrixf_linsolve_n64, "matrixf_linsolve execute, n=64", "matrix,linsolve")
    { return matrixf_linsolve_bench(num_iterations, 64); }

