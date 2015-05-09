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
// FFT API: fixed-point, 16-bit precision
//

#include "liquid.internal.h"

// Macro definitions
#define FFT(name)           LIQUID_CONCAT(fftq16,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_cccq16,name)

// fixed-point macros
#define LIQUID_FPM          1
#define Q(name)             LIQUID_CONCAT(q16,name)
#define CQ(name)            LIQUID_CONCAT(cq16,name)

#define T                   q16_t           /* primitive type */
#define TC                  cq16_t          /* primitive type (complex) */

// include main files
#include "fft_common.c"         // common source must come first (object definition)
#include "fft_dft.c"            // FFT definitions for DFT
#include "fft_radix2.c"         // FFT definitions for radix-2 transforms
#include "fft_mixed_radix.c"    // FFT definitions for mixed-radix transforms (Cooley-Tukey)
#include "fft_rader.c"          // FFT definitions for transforms of prime length (Rader's algorithm)
#include "fft_rader2.c"         // FFT definitions for transforms of prime length (Rader's alternate algorithm)
#include "fft_r2r_1d.c"         // real-to-real definitions (DCT/DST)

