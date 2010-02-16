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


T POLY(val)(T * _p, unsigned int _k, T _x)
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

void POLY(fit)(T * _x,
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

// Lagrange polynomial exact fit (order _n-1)
void POLY(fit_lagrange)(T * _x,
                        T * _y,
                        unsigned int _n,
                        T * _p)
{
    unsigned int k=_n-1;    // polynomial order

    // clear polynomial coefficients array
    unsigned int i;
    for (i=0; i<k; i++)
        _p[i] = 0.;

    // compute roots, gain
    T roots[k];     // polynomial roots
    T c[_n];        // expanded polynomial
    T g;            // gain
    unsigned int j;
    unsigned int n;
    for (i=0; i<_n; i++) {
        n=0;
        g=1.0f;
        for (j=0; j<_n; j++) {
            if (j!=i) {
                roots[n++] = - _x[j];
                g *= (_x[i] - _x[j]);
            }
        }
        g = _y[i] / g;

        // expand roots
        POLY(_expandroots)(roots, k, c);

        for (j=0; j<_n; j++) {
            _p[j] += g * c[j];
        }

#if 0
        // debug/print
        printf("****** %3u : **********************\n", i);
        printf("  g : %12.8f\n", g);
        printf("  roots:\n");
        for (j=0; j<k; j++)
            printf("  r[%3u] = %12.8f\n", j, crealf(roots[j]));
        printf("  expanded roots:\n");
        for (j=0; j<_n; j++)
            printf("  c[%3u] = %16.6f > %16.6f\n", j, crealf(c[j]), crealf(g*c[j]));
#endif
    }

}

// Lagrange polynomial interpolation
void POLY(_interp_lagrange)(T * _x,
                            T * _y,
                            unsigned int _n,
                            T   _x0,
                            T * _y0)
{
    *_y0 = 0.0;     // set output to zero
    T g;            // accumulator
    unsigned int i, j;
    for (i=0; i<_n; i++) {
        g=1.0f;
        for (j=0; j<_n; j++) {
            if (j!=i) {
                g *= (_x0 - _x[j])/(_x[i] - _x[j]);
            }
        }
        *_y0 += _y[i] * g;
    }
}

