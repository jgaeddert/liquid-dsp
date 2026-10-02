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

//
// fft_r2r_benchmark.h
//
// Real even/odd FFT benchmarks (discrete cosine/sine transforms)
//

#include "liquid.benchmark.h"

// Helper function to keep code base small
float fft_r2r_bench(unsigned long int num_iterations,
                   unsigned int _n,
                   int _kind)
{
    // initialize arrays, plan
    float x[_n], y[_n];
    int _flags = 0;
    fftplan p = fft_create_plan_r2r_1d(_n, x, y, _kind, _flags);
    
    unsigned long int i;

    // initialize input with random values
    for (i=0; i<_n; i++)
        x[i] = randnf();

    // start trials (4 executes of _n samples each per iteration; round down)
    unsigned long int n = num_iterations / (4 * _n);
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        fft_execute(p);
        fft_execute(p);
        fft_execute(p);
        fft_execute(p);
    }
    float extime = liquid_toc(timer);

    fft_destroy_plan(p);
    return extime;
}

// Radix-2 (n=128)

LIQUID_BENCHMARK(fft_REDFT00_128, "fft r2r execute, n=128 REDFT00", "fft,r2r,redft00")
    { return fft_r2r_bench(num_iterations, 128, LIQUID_FFT_REDFT00); }
LIQUID_BENCHMARK(fft_REDFT01_128, "fft r2r execute, n=128 REDFT01", "fft,r2r,redft01")
    { return fft_r2r_bench(num_iterations, 128, LIQUID_FFT_REDFT01); }
LIQUID_BENCHMARK(fft_REDFT10_128, "fft r2r execute, n=128 REDFT10", "fft,r2r,redft10")
    { return fft_r2r_bench(num_iterations, 128, LIQUID_FFT_REDFT10); }
LIQUID_BENCHMARK(fft_REDFT11_128, "fft r2r execute, n=128 REDFT11", "fft,r2r,redft11")
    { return fft_r2r_bench(num_iterations, 128, LIQUID_FFT_REDFT11); }

LIQUID_BENCHMARK(fft_RODFT00_128, "fft r2r execute, n=128 RODFT00", "fft,r2r,rodft00")
    { return fft_r2r_bench(num_iterations, 128, LIQUID_FFT_RODFT00); }
LIQUID_BENCHMARK(fft_RODFT01_128, "fft r2r execute, n=128 RODFT01", "fft,r2r,rodft01")
    { return fft_r2r_bench(num_iterations, 128, LIQUID_FFT_RODFT01); }
LIQUID_BENCHMARK(fft_RODFT10_128, "fft r2r execute, n=128 RODFT10", "fft,r2r,rodft10")
    { return fft_r2r_bench(num_iterations, 128, LIQUID_FFT_RODFT10); }
LIQUID_BENCHMARK(fft_RODFT11_128, "fft r2r execute, n=128 RODFT11", "fft,r2r,rodft11")
    { return fft_r2r_bench(num_iterations, 128, LIQUID_FFT_RODFT11); }

// prime (n=127)

LIQUID_BENCHMARK(fft_REDFT00_127, "fft r2r execute, n=127 REDFT00", "fft,r2r,redft00")
    { return fft_r2r_bench(num_iterations, 127, LIQUID_FFT_REDFT00); }
LIQUID_BENCHMARK(fft_REDFT01_127, "fft r2r execute, n=127 REDFT01", "fft,r2r,redft01")
    { return fft_r2r_bench(num_iterations, 127, LIQUID_FFT_REDFT01); }
LIQUID_BENCHMARK(fft_REDFT10_127, "fft r2r execute, n=127 REDFT10", "fft,r2r,redft10")
    { return fft_r2r_bench(num_iterations, 127, LIQUID_FFT_REDFT10); }
LIQUID_BENCHMARK(fft_REDFT11_127, "fft r2r execute, n=127 REDFT11", "fft,r2r,redft11")
    { return fft_r2r_bench(num_iterations, 127, LIQUID_FFT_REDFT11); }

LIQUID_BENCHMARK(fft_RODFT00_127, "fft r2r execute, n=127 RODFT00", "fft,r2r,rodft00")
    { return fft_r2r_bench(num_iterations, 127, LIQUID_FFT_RODFT00); }
LIQUID_BENCHMARK(fft_RODFT01_127, "fft r2r execute, n=127 RODFT01", "fft,r2r,rodft01")
    { return fft_r2r_bench(num_iterations, 127, LIQUID_FFT_RODFT01); }
LIQUID_BENCHMARK(fft_RODFT10_127, "fft r2r execute, n=127 RODFT10", "fft,r2r,rodft10")
    { return fft_r2r_bench(num_iterations, 127, LIQUID_FFT_RODFT10); }
LIQUID_BENCHMARK(fft_RODFT11_127, "fft r2r execute, n=127 RODFT11", "fft,r2r,rodft11")
    { return fft_r2r_bench(num_iterations, 127, LIQUID_FFT_RODFT11); }

