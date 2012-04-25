/*
 * Copyright (c) 2007, 2009, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2012 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// fft_radix2_benchmark.c : benchmark FFTs of length 2^m
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include "liquid.h"

#include "src/fft/bench/fft_runbench.h"

// power-of-two transforms
void benchmark_fft_2      FFT_BENCHMARK_API(2,       FFT_FORWARD)
void benchmark_fft_4      FFT_BENCHMARK_API(4,       FFT_FORWARD)
void benchmark_fft_8      FFT_BENCHMARK_API(8,       FFT_FORWARD)
void benchmark_fft_16     FFT_BENCHMARK_API(16,      FFT_FORWARD)
void benchmark_fft_32     FFT_BENCHMARK_API(32,      FFT_FORWARD)
void benchmark_fft_64     FFT_BENCHMARK_API(64,      FFT_FORWARD)
void benchmark_fft_128    FFT_BENCHMARK_API(128,     FFT_FORWARD)
void benchmark_fft_256    FFT_BENCHMARK_API(256,     FFT_FORWARD)
void benchmark_fft_512    FFT_BENCHMARK_API(512,     FFT_FORWARD)
void benchmark_fft_1024   FFT_BENCHMARK_API(1024,    FFT_FORWARD)
void benchmark_fft_2048   FFT_BENCHMARK_API(2048,    FFT_FORWARD)
void benchmark_fft_4096   FFT_BENCHMARK_API(4096,    FFT_FORWARD)
void benchmark_fft_8192   FFT_BENCHMARK_API(8192,    FFT_FORWARD)
void benchmark_fft_16384  FFT_BENCHMARK_API(16384,   FFT_FORWARD)
void benchmark_fft_32768  FFT_BENCHMARK_API(32768,   FFT_FORWARD)

