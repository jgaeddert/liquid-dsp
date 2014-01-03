/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// FFT API: floating-point (single precision)
//

#include "liquid.internal.h"

// Macro definitions
#define FFT(name)           LIQUID_CONCAT(fft,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_cccf,name)

#define T                   float           /* primitive type */
#define TC                  float complex   /* primitive type (complex) */

#define PRINTVAL_T(X,F)     PRINTVAL_FLOAT(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_CFLOAT(X,F)

// include main files
#include "fft_common.c"         // common source must come first (object definition)
#include "fft_dft.c"            // FFT definitions for DFT
#include "fft_radix2.c"         // FFT definitions for radix-2 transforms
#include "fft_mixed_radix.c"    // FFT definitions for mixed-radix transforms (Cooley-Tukey)
#include "fft_rader.c"          // FFT definitions for transforms of prime length (Rader's algorithm)
#include "fft_rader2.c"         // FFT definitions for transforms of prime length (Rader's alternate algorithm)
#include "fft_r2r_1d.c"         // real-to-real definitions (DCT/DST)

