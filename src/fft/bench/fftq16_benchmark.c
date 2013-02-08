/*
 * Copyright (c) 2012, 2013 Joseph Gaeddert
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

