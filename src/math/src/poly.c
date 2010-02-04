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
// polynomial methods
//

#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"


float polyval(float * _p, unsigned int _k, float _x)
{
    unsigned int i;
    float xk = 1;
    float y = 0.0f;
    for (i=0; i<_k; i++) {
        y += _p[i]*xk;
        xk *= _x;
    }
    return y;
}

void polyfit(float * _x,
             float * _y,
             unsigned int _n,
             float * _p,
             unsigned int _k)
{

    // ...
    float X[_n*_k];
    unsigned int r,c;
    float v;
    for (r=0; r<_n; r++) {
        v = 1;
        for (c=0; c<_k; c++) {
            matrix_access(X,_n,_k,r,c) = v;
            v *= _x[r];
        }
    }

    // compute transpose of X
    float Xt[_k*_n];
    memmove(Xt,X,_k*_n*sizeof(float));
    fmatrix_trans(Xt,_n,_k);

    // compute [X']*y
    float Xty[_k];
    fmatrix_mul(Xt, _k, _n,
                _y, _n, 1,
                Xty,_k, 1);

    // compute [X']*X
    float X2[_k*_k];
    fmatrix_mul(Xt, _k, _n,
                X,  _n, _k,
                X2, _k, _k);

    // compute inv([X']*X)
    float G[_k*_k];
    memmove(G,X2,_k*_k*sizeof(float));
    fmatrix_inv(G,_k,_k);

    // compute coefficients
    fmatrix_mul(G,  _k, _k,
                Xty,_k, 1,
                _p, _k, 1);
}

// expands the polynomial:
//  (x+a[0]) * (x+a[1]) * ... * (x+a[n-1])
// as
//  c[0] + c[1]*x + c[2]*x^2 + ... + c[n]*x^n
void poly_expandroots(float * _a,
                      unsigned int _n,
                      float * _c)
{
    // no roots; return zero
    if (_n == 0) {
        _c[0] = 0.;
        return;
    }

    int i, j;
    // initialize coefficients array to [1,0,0,....0]
    for (i=0; i<=_n; i++)
        _c[i] = (i==0) ? 1 : 0;

    // iterative polynomial multiplication
    for (i=0; i<_n; i++) {
        for (j=i+1; j>0; j--)
            _c[j] = _a[i]*_c[j] + _c[j-1];

        _c[j] *= _a[i];
    }

    // assert(c[_n]==1.0f)
}

// expands the polynomial:
//  (x*b[0]-a[0]) * (x*b[1]-a[1]) * ... * (x*b[n-1]-a[n-1])
// as
//  c[0] + c[1]*x + c[2]*x^2 + ... + c[n]*x^n
//
// c has order _n (array is length _n+1)
void poly_expandroots2(float * _a,
                       float * _b,
                       unsigned int _n,
                       float * _c)
{
    unsigned int i;
    for (i=0; i<=_n; i++)
        _c[i] = 0.0f;
}

// expands the multiplication of two polynomials
//
//  (a[0] + a[1]*x + a[2]*x^2 + ...) * (b[0] + b[1]*x + b[]*x^2 + ...2 + ...)
// as
//  c[0] + c[1]*x + c[2]*x^2 + ... + c[n]*x^n
//
// where order(c)  = order(a)  + order(b) + 1
//    :: length(c) = length(a) + length(b) - 1
//
//  _a          :   1st polynomial coefficients (length is _order_a+1)
//  _order_a    :   1st polynomial order
//  _b          :   2nd polynomial coefficients (length is _order_b+1)
//  _order_b    :   2nd polynomial order
//  _c          :   output polynomial coefficients (length is _order_a + _order_b + 1)
void polymul(float * _a,
             unsigned int _order_a,
             float * _b,
             unsigned int _order_b,
             float * _c)
{
    unsigned int na = _order_a + 1;
    unsigned int nb = _order_b + 1;
    unsigned int nc = na + nb - 1;
    unsigned int i;
    for (i=0; i<nc; i++)
        _c[i] = 0.0f;

    unsigned int j;
    for (i=0; i<na; i++) {
        for (j=0; j<nb; j++) {
            _c[i+j] += _a[i]*_b[j];
        }
    }
}

