/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Complex floating-point matrix
// 

#include "liquid.internal.h"

#define MATRIX(name)    LIQUID_CONCAT(cfmatrix, name)
#define MATRIX_NAME     "cfmatrix"
#define T               float complex
#define MATRIX_PRINT_ELEMENT(X,R,C,r,c) \
    printf("%4.2f+j%4.2f\t", \
        crealf(matrix_access(X,R,C,r,c)), \
        cimagf(matrix_access(X,R,C,r,c)));

#include "matrix.c"
#include "matrix_inv.c"

