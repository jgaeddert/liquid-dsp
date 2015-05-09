/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
// fftq16_benchmark.c : benchmark some fixed-point FFTs
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include "liquid.h"

#include "src/fft/bench/fftq16_runbench.h"

// fixed-point transforms
void benchmark_fftq16_2      FFTQ16_BENCHMARK_API( 2,     LIQUID_FFT_FORWARD )
void benchmark_fftq16_4      FFTQ16_BENCHMARK_API( 4,     LIQUID_FFT_FORWARD )
void benchmark_fftq16_8      FFTQ16_BENCHMARK_API( 8,     LIQUID_FFT_FORWARD )
void benchmark_fftq16_16     FFTQ16_BENCHMARK_API( 16,    LIQUID_FFT_FORWARD )
void benchmark_fftq16_32     FFTQ16_BENCHMARK_API( 32,    LIQUID_FFT_FORWARD )
void benchmark_fftq16_64     FFTQ16_BENCHMARK_API( 64,    LIQUID_FFT_FORWARD )
void benchmark_fftq16_128    FFTQ16_BENCHMARK_API( 128,   LIQUID_FFT_FORWARD )
void benchmark_fftq16_256    FFTQ16_BENCHMARK_API( 256,   LIQUID_FFT_FORWARD )
void benchmark_fftq16_512    FFTQ16_BENCHMARK_API( 512,   LIQUID_FFT_FORWARD )
void benchmark_fftq16_1024   FFTQ16_BENCHMARK_API( 1024,  LIQUID_FFT_FORWARD )
void benchmark_fftq16_2048   FFTQ16_BENCHMARK_API( 2048,  LIQUID_FFT_FORWARD )
void benchmark_fftq16_4096   FFTQ16_BENCHMARK_API( 4096,  LIQUID_FFT_FORWARD )
void benchmark_fftq16_8192   FFTQ16_BENCHMARK_API( 8192,  LIQUID_FFT_FORWARD )
void benchmark_fftq16_16384  FFTQ16_BENCHMARK_API( 16384, LIQUID_FFT_FORWARD )
void benchmark_fftq16_32768  FFTQ16_BENCHMARK_API( 32768, LIQUID_FFT_FORWARD )

