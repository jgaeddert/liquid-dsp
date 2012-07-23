/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
 *                                      Institute & State University
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

