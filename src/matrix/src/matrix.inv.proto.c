/*
 * Copyright (c) 2007 - 2020 Joseph Gaeddert
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
// Matrix inverse method definitions
//

#include "liquid.internal.h"

int MATRIX(_inv)(T * _X, unsigned int _XR, unsigned int _XC)
{
    // ensure lengths are valid
    if (_XR != _XC )
        return liquid_error(LIQUID_EICONFIG,"%s_inv(), invalid dimensions", MATRIX_NAME);

    // X:
    //  x11 x12 ... x1n
    //  x21 x22 ... x2n
    //  ...
    //  xn1 xn2 ... xnn

    // allocate temporary memory
    T x[2*_XR*_XC];
    unsigned int xr = _XR;
    unsigned int xc = _XC*2;

    // x:
    //  x11 x12 ... x1n 1   0   ... 0
    //  x21 x22 ... x2n 0   1   ... 0
    //  ...
    //  xn1 xn2 ... xnn 0   0   ... 1
    unsigned int r,c;
    for (r=0; r<_XR; r++) {
        // copy matrix elements
        for (c=0; c<_XC; c++)
            matrix_access(x,xr,xc,r,c) = matrix_access(_X,_XR,_XC,r,c);

        // append identity matrix
        for (c=0; c<_XC; c++) {
#if defined LIQUID_FIXED && T_COMPLEX==1
            matrix_access(x,xr,xc,r,_XC+c).real = (r==c) ? Q(_one) : 0;
            matrix_access(x,xr,xc,r,_XC+c).imag = 0;
#elif defined LIQUID_FIXED && T_COMPLEX==0
            matrix_access(x,xr,xc,r,_XC+c) = (r==c) ? Q(_one) : 0;
#else
            matrix_access(x,xr,xc,r,_XC+c) = (r==c) ? 1 : 0;
#endif
        }
    }

    // perform Gauss-Jordan elimination on x
    // x:
    //  1   0   ... 0   y11 y12 ... y1n
    //  0   1   ... 0   y21 y22 ... y2n
    //  ...
    //  0   0   ... 1   yn1 yn2 ... ynn
    MATRIX(_gjelim)(x,xr,xc);

    // copy result from right half of x
    for (r=0; r<_XR; r++) {
        for (c=0; c<_XC; c++)
            matrix_access(_X,_XR,_XC,r,c) = matrix_access(x,xr,xc,r,_XC+c);
    }
    return LIQUID_OK;
}

// Gauss-Jordan elmination
int MATRIX(_gjelim)(T * _X, unsigned int _XR, unsigned int _XC)
{
    unsigned int r, c;

    // choose pivot rows based on maximum element along column
    TP v;
    TP v_max=0;
    unsigned int r_opt=0;
    unsigned int r_hat;
    for (r=0; r<_XR; r++) {

        // check values along this column and find the maximum
        for (r_hat=r; r_hat<_XR; r_hat++) {
#if defined LIQUID_FIXED && T_COMPLEX==0
            v = Q(_abs)( matrix_access(_X,_XR,_XC,r_hat,r) );
#elif defined LIQUID_FIXED && T_COMPLEX==1
            v = CQ(_cabs)( matrix_access(_X,_XR,_XC,r_hat,r) );
#else
            v = T_ABS( matrix_access(_X,_XR,_XC,r_hat,r) );
#endif
            // swap rows if necessary
            if (v > v_max || r_hat==r) {
                r_opt = r_hat;
                v_max = v;
            }
        }

        // if the maximum is zero, matrix is singular
        if (v_max == 0.0f)
            return liquid_error(LIQUID_EICONFIG,"%s_gjelim(), matrix singular to machine precision", MATRIX_NAME);

        // if row does not match column (e.g. maximum value does not
        // lie on the diagonal) swap the rows
        if (r != r_opt) {
            MATRIX(_swaprows)(_X,_XR,_XC,r,r_opt);
        }

        // pivot on the diagonal element
        MATRIX(_pivot)(_X,_XR,_XC,r,r);
    }

    // scale by diagonal
    T g;
    for (r=0; r<_XR; r++) {
#if defined LIQUID_FIXED && T_COMPLEX==0
        g = Q(_inv)( matrix_access(_X,_XR,_XC,r,r), 14 );
        for (c=0; c<_XC; c++)
            matrix_access(_X,_XR,_XC,r,c) = Q(_mul)( g, matrix_access(_X,_XR,_XC,r,c) );
#elif defined LIQUID_FIXED && T_COMPLEX==1
        g = CQ(_inv)( matrix_access(_X,_XR,_XC,r,r) );
        for (c=0; c<_XC; c++)
            matrix_access(_X,_XR,_XC,r,c) = CQ(_mul)( g, matrix_access(_X,_XR,_XC,r,c) );
#else
        g = 1 / matrix_access(_X,_XR,_XC,r,r);
        for (c=0; c<_XC; c++)
            matrix_access(_X,_XR,_XC,r,c) *= g;
#endif
    }
    return LIQUID_OK;
}

// pivot on element _r, _c
int MATRIX(_pivot)(T * _X, unsigned int _XR, unsigned int _XC, unsigned int _r, unsigned int _c)
{
    T v = matrix_access(_X,_XR,_XC,_r,_c);
#if defined LIQUID_FIXED && T_COMPLEX==1
    if (v.real==0 && v.imag==0) {
#elif defined LIQUID_FIXED && T_COMPLEX==0
    if (v==0) {
#else
    if (v==0) {
#endif
        return liquid_error(LIQUID_EICONFIG,"matrix_pivot(), pivoting on zero");
    }

    unsigned int r,c;

    // pivot using back-substitution
    T g;    // multiplier
    for (r=0; r<_XR; r++) {

        // skip over pivot row
        if (r == _r)
            continue;

        // compute multiplier
#if defined LIQUID_FIXED && T_COMPLEX==0
        g = Q(_div)( matrix_access(_X,_XR,_XC,r,_c), v );
#elif defined LIQUID_FIXED && T_COMPLEX==1
        g = CQ(_div)( matrix_access(_X,_XR,_XC,r,_c), v );
#else
        g = matrix_access(_X,_XR,_XC,r,_c) / v;
#endif

        // back-substitution
        for (c=0; c<_XC; c++) {
#if defined LIQUID_FIXED && T_COMPLEX==0
            T v = Q(_mul)( g, matrix_access(_X,_XR,_XC,_r,c) );
            matrix_access(_X,_XR,_XC,r,c) = Q(_sub)( v, matrix_access(_X,_XR,_XC, r,c) );
#elif defined LIQUID_FIXED && T_COMPLEX==1
            T v = CQ(_mul)( g, matrix_access(_X,_XR,_XC,_r,c) );
            matrix_access(_X,_XR,_XC,r,c) = CQ(_sub)( v, matrix_access(_X,_XR,_XC, r,c) );
#else
            matrix_access(_X,_XR,_XC,r,c) = g*matrix_access(_X,_XR,_XC,_r,c) -
                                              matrix_access(_X,_XR,_XC, r,c);
#endif
        }
    }
    return LIQUID_OK;
}

int MATRIX(_swaprows)(T * _X, unsigned int _XR, unsigned int _XC, unsigned int _r1, unsigned int _r2)
{
    if (_r1 == _r2)
        return LIQUID_OK;

    unsigned int c;
    T v_tmp;
    for (c=0; c<_XC; c++) {
        v_tmp = matrix_access(_X,_XR,_XC,_r1,c);
        matrix_access(_X,_XR,_XC,_r1,c) = matrix_access(_X,_XR,_XC,_r2,c);
        matrix_access(_X,_XR,_XC,_r2,c) = v_tmp;
    }
    return LIQUID_OK;
}
