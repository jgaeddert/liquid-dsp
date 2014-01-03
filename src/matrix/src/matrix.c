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
// Floating-point matrix (double precision)
// 

#include "liquid.internal.h"

#define MATRIX(name)    LIQUID_CONCAT(matrix, name)
#define MATRIX_NAME     "matrix"

#define T               double          // general type
#define TP              double          // primitive type
#define T_COMPLEX       0               // is type complex?

#define MATRIX_PRINT_ELEMENT(X,R,C,r,c) \
    printf("%12.8f", matrix_access(X,R,C,r,c));

#include "matrix.base.c"
#include "matrix.cgsolve.c"
#include "matrix.chol.c"
#include "matrix.gramschmidt.c"
#include "matrix.inv.c"
#include "matrix.linsolve.c"
#include "matrix.ludecomp.c"
#include "matrix.qrdecomp.c"
#include "matrix.math.c"

