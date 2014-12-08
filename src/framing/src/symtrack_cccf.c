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
// Framing API: floating-point
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION_SHORT     "f"
#define EXTENSION_FULL      "cccf"

#define PRINTVAL(x)         printf("%12.4e + j%12.4e", crealf(x), cimagf(x))

#define T                   float
#define TO                  float complex
#define TC                  float complex
#define TI                  float complex
#define ABS(X)              cabsf(X)

// object references
#define SYMTRACK(name)      LIQUID_CONCAT(symtrack_cccf,name)
#define AGC(name)           LIQUID_CONCAT(agc_crcf,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync_crcf,name)
#define EQLMS(name)         LIQUID_CONCAT(eqlms_cccf,name)
#define NCO(name)           LIQUID_CONCAT(nco_crcf,name)
#define MODEM(name)         LIQUID_CONCAT(modem,name)

#define TO_COMPLEX          1
#define TC_COMPLEX          1
#define TI_COMPLEX          1

// source files
#include "symtrack.c"

