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
// spectral periodogram API: complex floating-point
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION           "cf"

// name-mangling macros
#define ASGRAM(name)        LIQUID_CONCAT(asgramcf,name)
#define SPGRAM(name)        LIQUID_CONCAT(spgramcf,name)
#define WINDOW(name)        LIQUID_CONCAT(windowcf,name)
#define FFT(name)           LIQUID_CONCAT(fft,name)

#define T                   float           // primitive type (real)
#define TC                  float complex   // primitive type (complex)
#define TI                  float complex   // input type

#define TI_COMPLEX          1

#define PRINTVAL_T(X,F)     PRINTVAL_FLOAT(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_CFLOAT(X,F)
#define PRINTVAL_TI(X,F)    PRINTVAL_CFLOAT(X,F)

// source files
#include "asgram.c"
#include "spgram.c"

