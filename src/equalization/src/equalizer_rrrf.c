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
// Floating-point equalizers
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION_SHORT "f"
#define EXTENSION_FULL  "rrrf"

#define EQLMS(name)     LIQUID_CONCAT(eqlms_rrrf,name)
#define EQRLS(name)     LIQUID_CONCAT(eqrls_rrrf,name)

#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define WINDOW(name)    LIQUID_CONCAT(windowf,name)
#define MATRIX(name)    LIQUID_CONCAT(matrixf,name)

#define T               float

#define PRINTVAL(V)     printf("%5.2f ", V);

#include "eqlms.c"
#include "eqrls.c"
