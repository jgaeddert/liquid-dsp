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

// 
#define BSYNC(name)         LIQUID_CONCAT(bsync_rrrf,name)

#define PRINTVAL(x)         printf("%12.4e", x)

#define TO                  float
#define TC                  float
#define TI                  float
#define ABS(X)              fabsf(X)
#define WINDOW(name)        LIQUID_CONCAT(windowf,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_rrrf,name)

#undef  TO_COMPLEX
#undef  TC_COMPLEX
#undef  TI_COMPLEX

// source files
#include "bsync.c"

