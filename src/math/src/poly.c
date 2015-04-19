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
// Floating-point polynomials (double precision)
// 

#include "liquid.internal.h"

#define MATRIX(name)    LIQUID_CONCAT(matrix, name)
#define POLY(name)      LIQUID_CONCAT(poly,   name)
#define POLY_NAME       "poly"
#define T               double
#define TC              double complex

#define T_COMPLEX       1
#define TI_COMPLEX      1

#define T_ABS(X)        fabs(X)
#define TC_ABS(X)       cabs(X)

#include "poly.common.c"
#include "poly.expand.c"
#include "poly.findroots.c"
#include "poly.lagrange.c"

