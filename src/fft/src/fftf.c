/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
#include "fft_create_plan.c"
#include "fct_execute.c"
#include "dct_execute.c"
#include "dst_execute.c"
#include "mdct_execute.c"
#include "imdct_execute.c"
#include "fft_execute.c"
#include "fft.common.c"
