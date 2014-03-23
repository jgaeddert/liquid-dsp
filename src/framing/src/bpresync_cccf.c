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
// Binary pre-demod synchronizer
//

#include "liquid.internal.h"

// 
#define BPRESYNC(name)      LIQUID_CONCAT(bpresync_cccf,name)

// print and naming extensions
#define PRINTVAL(x)         printf("%12.4e + j%12.4e", crealf(x), cimagf(x))
#define EXTENSION_SHORT     "f"
#define EXTENSION_FULL      "cccf"

#define TO                  float complex
#define TC                  float complex
#define TI                  float complex

#define ABS(X)              cabsf(X)
#define REAL(X)             crealf(X)
#define IMAG(X)             cimagf(X)

#define BSYNC(name)         LIQUID_CONCAT(bsync_cccf,name)

#define TO_COMPLEX
#define TC_COMPLEX
#define TI_COMPLEX

// source files
#include "bpresync.c"

