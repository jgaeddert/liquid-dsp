/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// Fixed-point matrix (complex)
// 

#include "liquid.internal.h"

#define MATRIX(name)    LIQUID_CONCAT(matrixcq16, name)
#define MATRIX_NAME     "matrixcq16"

// declare fixed-point name-mangling macros
#define LIQUID_FIXED
#define Q(name)         LIQUID_CONCAT(q16,name)
#define CQ(name)        LIQUID_CONCAT(cq16,name)

#define T               cq16_t          // general type
#define TP              q16_t           // primitive type
#define T_COMPLEX       1               // is type complex?

#define MATRIX_PRINT_ELEMENT(X,R,C,r,c)                     \
    printf("%7.2f+j%6.2f ",                                 \
        q16_fixed_to_float(matrix_access(X,R,C,r,c).real),  \
        q16_fixed_to_float(matrix_access(X,R,C,r,c).imag)); \

#include "matrix.base.c"
//#include "matrix.cgsolve.c"
//#include "matrix.chol.c"
//#include "matrix.gramschmidt.c"
#include "matrix.inv.c"
//#include "matrix.linsolve.c"
//#include "matrix.ludecomp.c"
//#include "matrix.qrdecomp.c"
#include "matrix.math.c"

