/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void vectorf_bench(struct rusage *_start,
                   struct rusage *_finish,
                   unsigned long int *_num_iterations,
                   unsigned int _n)
{
    // normalize number of iterations
    *_num_iterations = *_num_iterations * 20 / _n;
    if (*_num_iterations < 1) *_num_iterations = 1;

    // allocate buffers
    LIQUID_VLA(float, buf_0, _n);
    LIQUID_VLA(float, buf_1, _n);
    LIQUID_VLA(float, buf_2, _n);
    unsigned int i;
    for (i=0; i<_n; i++) {
        buf_0[i] = randnf();
        buf_1[i] = randnf();
    }

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // run vector multiplication
        liquid_vectorf_mul(buf_0, buf_1, _n, buf_2);

        // ensure the compiler doesn't optimize this out
        buf_0[i % _n] += buf_2[0];
    }
    *_num_iterations *= _n;
    getrusage(RUSAGE_SELF, _finish);
}

#define VECTORF_BENCHMARK_API(N)        \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ vectorf_bench(_start, _finish, _num_iterations, N); }

void benchmark_vectorf_4       VECTORF_BENCHMARK_API(4)
void benchmark_vectorf_16      VECTORF_BENCHMARK_API(16)
void benchmark_vectorf_64      VECTORF_BENCHMARK_API(64)
void benchmark_vectorf_256     VECTORF_BENCHMARK_API(256)
void benchmark_vectorf_1024    VECTORF_BENCHMARK_API(1024)

