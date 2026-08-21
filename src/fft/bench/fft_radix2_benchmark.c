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

// benchmark FFTs of length 2^m

#include "src/fft/bench/fft_runbench.h"

// power-of-two transforms
LIQUID_BENCHMARK(fft_2,     "fft execute, nfft=2"    , "fft,radix2") {return fft_runbench(num_iterations, 2,     LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_4,     "fft execute, nfft=4"    , "fft,radix2") {return fft_runbench(num_iterations, 4,     LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_8,     "fft execute, nfft=8"    , "fft,radix2") {return fft_runbench(num_iterations, 8,     LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_16,    "fft execute, nfft=16"   , "fft,radix2") {return fft_runbench(num_iterations, 16,    LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_32,    "fft execute, nfft=32"   , "fft,radix2") {return fft_runbench(num_iterations, 32,    LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_64,    "fft execute, nfft=64"   , "fft,radix2") {return fft_runbench(num_iterations, 64,    LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_128,   "fft execute, nfft=128"  , "fft,radix2") {return fft_runbench(num_iterations, 128,   LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_256,   "fft execute, nfft=256"  , "fft,radix2") {return fft_runbench(num_iterations, 256,   LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_512,   "fft execute, nfft=512"  , "fft,radix2") {return fft_runbench(num_iterations, 512,   LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_1024,  "fft execute, nfft=1024" , "fft,radix2") {return fft_runbench(num_iterations, 1024,  LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_2048,  "fft execute, nfft=2048" , "fft,radix2") {return fft_runbench(num_iterations, 2048,  LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_4096,  "fft execute, nfft=4096" , "fft,radix2") {return fft_runbench(num_iterations, 4096,  LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_8192,  "fft execute, nfft=8192" , "fft,radix2") {return fft_runbench(num_iterations, 8192,  LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_16384, "fft execute, nfft=16384", "fft,radix2") {return fft_runbench(num_iterations, 16384, LIQUID_FFT_FORWARD); }
LIQUID_BENCHMARK(fft_32768, "fft execute, nfft=32768", "fft,radix2") {return fft_runbench(num_iterations, 32768, LIQUID_FFT_FORWARD); }

