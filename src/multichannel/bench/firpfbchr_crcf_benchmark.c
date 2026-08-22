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
float firpfbchr_crcf_execute_bench(unsigned long int _num_iterations,
                                   unsigned int      _M,
                                   unsigned int      _P,
                                   unsigned int      _m)
{
    // initialize channelizer
    float As         = 60.0f;
    firpfbchr_crcf q = firpfbchr_crcf_create_kaiser(_M,_P,_m,As);
    unsigned long int i;
    float complex x[_P];
    float complex y[_M];
    for (i=0; i<_P; i++)
        x[i] = randnf() + _Complex_I*randnf();
    // start trials (4 push+execute of _P samples each per iteration; round down)
    unsigned long int n = _num_iterations / (4 * _P);
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        firpfbchr_crcf_push(q, x);  firpfbchr_crcf_execute(q, y);
        firpfbchr_crcf_push(q, x);  firpfbchr_crcf_execute(q, y);
        firpfbchr_crcf_push(q, x);  firpfbchr_crcf_execute(q, y);
        firpfbchr_crcf_push(q, x);  firpfbchr_crcf_execute(q, y);
    }
    float extime = liquid_toc(timer);
    firpfbchr_crcf_destroy(q);
    return extime;
}

// analysis
LIQUID_BENCHMARK(firpfbchr_crcf_M0064_P0063, "firpfbchr_crcf analyzer, M=64 P=63",   "multichannel,firpfbchr,analyzer")
    { return firpfbchr_crcf_execute_bench(num_iterations,   64,   63, 4); }
LIQUID_BENCHMARK(firpfbchr_crcf_M0128_P0127, "firpfbchr_crcf analyzer, M=128 P=127",  "multichannel,firpfbchr,analyzer")
    { return firpfbchr_crcf_execute_bench(num_iterations,  128,  127, 4); }
LIQUID_BENCHMARK(firpfbchr_crcf_M0256_P0255, "firpfbchr_crcf analyzer, M=256 P=255",  "multichannel,firpfbchr,analyzer")
    { return firpfbchr_crcf_execute_bench(num_iterations,  256,  255, 4); }
LIQUID_BENCHMARK(firpfbchr_crcf_M0512_P0511, "firpfbchr_crcf analyzer, M=512 P=511",  "multichannel,firpfbchr,analyzer")
    { return firpfbchr_crcf_execute_bench(num_iterations,  512,  511, 4); }
LIQUID_BENCHMARK(firpfbchr_crcf_M1024_P1023, "firpfbchr_crcf analyzer, M=1024 P=1023","multichannel,firpfbchr,analyzer")
    { return firpfbchr_crcf_execute_bench(num_iterations, 1024, 1023, 4); }
LIQUID_BENCHMARK(firpfbchr_crcf_M2048_P2047, "firpfbchr_crcf analyzer, M=2048 P=2047","multichannel,firpfbchr,analyzer")
    { return firpfbchr_crcf_execute_bench(num_iterations, 2048, 2047, 4); }
LIQUID_BENCHMARK(firpfbchr_crcf_M4096_P4095, "firpfbchr_crcf analyzer, M=4096 P=4095","multichannel,firpfbchr,analyzer")
    { return firpfbchr_crcf_execute_bench(num_iterations, 4096, 4095, 4); }

