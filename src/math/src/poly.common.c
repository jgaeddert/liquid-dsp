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
// common polynomial routines
//  polyval
//  polyfit
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

T POLY(_val)(T * _p, unsigned int _k, T _x)
{
    unsigned int i;
    T xk = 1;
    T y = 0.0f;
    for (i=0; i<_k; i++) {
        y += _p[i]*xk;
        xk *= _x;
    }
    return y;
}

void POLY(_fit)(T * _x,
                T * _y,
                unsigned int _n,
                T * _p,
                unsigned int _k)
{

    // ...
    T X[_n*_k];
    unsigned int r,c;
    T v;
    for (r=0; r<_n; r++) {
        v = 1;
        for (c=0; c<_k; c++) {
            matrix_access(X,_n,_k,r,c) = v;
            v *= _x[r];
        }
    }

    // compute transpose of X
    T Xt[_k*_n];
    memmove(Xt,X,_k*_n*sizeof(T));
    MATRIX(_trans)(Xt,_n,_k);

    // compute [X']*y
    T Xty[_k];
    MATRIX(_mul)(Xt, _k, _n,
                 _y, _n, 1,
                 Xty,_k, 1);

    // compute [X']*X
    T X2[_k*_k];
    MATRIX(_mul)(Xt, _k, _n,
                 X,  _n, _k,
                 X2, _k, _k);

    // compute inv([X']*X)
    T G[_k*_k];
    memmove(G,X2,_k*_k*sizeof(T));
    MATRIX(_inv)(G,_k,_k);

    // compute coefficients
    MATRIX(_mul)(G,  _k, _k,
                 Xty,_k, 1,
                 _p, _k, 1);
}

