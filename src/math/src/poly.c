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


