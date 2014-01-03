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
// Matrix Gram-Schmidt Orthonormalization
//

#include <math.h>
#include "liquid.internal.h"

#define DEBUG_MATRIX_GRAMSCHMIDT 0

// compute projection of _u onto _v, store in _e
void MATRIX(_proj)(T * _u,
                   T * _v,
                   unsigned int _n,
                   T * _e)
{
    // compute dot-product between _u and _v
    unsigned int i;
    T uv = 0.;
    T uu = 0.;
    for (i=0; i<_n; i++) {
        uv += _u[i] * _v[i];
        uu += _u[i] * _u[i];
    }

    // TODO : check magnitude of _uu
    T g = uv / uu;
    for (i=0; i<_n; i++)
        _e[i] = _u[i] * g;
}

// Orthnormalization using the Gram-Schmidt algorithm
void MATRIX(_gramschmidt)(T * _x,
                          unsigned int _rx,
                          unsigned int _cx,
                          T * _v)
{
    // validate input
    if (_rx == 0 || _cx == 0) {
        fprintf(stderr,"error: matrix_gramschmidt(), input matrix cannot have zero-length dimensions\n");
        exit(1);
    }

    unsigned int i;
    unsigned int j;
    unsigned int k;

    // copy _x to _u
    memmove(_v, _x, _rx * _cx * sizeof(T));

    unsigned int n = _rx;   // dimensionality of each vector
    T proj_ij[n];
    for (j=0; j<_cx; j++) {
        for (i=0; i<j; i++) {
            // v_j  <-  v_j - proj(v_i, v_j)

#if DEBUG_MATRIX_GRAMSCHMIDT
            printf("computing proj(v_%u, v_%u)\n", i, j);
#endif

            // compute proj(v_i, v_j)
            T vij = 0.;     // dotprod(v_i, v_j)
            T vii = 0.;     // dotprod(v_i, v_i)
            T ti;
            T tj;
            for (k=0; k<n; k++) {
                ti = matrix_access(_v, _rx, _cx, k, i);
                tj = matrix_access(_v, _rx, _cx, k, j);

                T prodij = ti * conj(tj);
                vij += prodij;

                T prodii = ti * conj(ti);
                vii += prodii;
            }
            // TODO : vii should be 1.0 from normalization step below
            T g = vij / vii;

            // complete projection
            for (k=0; k<n; k++)
                proj_ij[k] = matrix_access(_v, _rx, _cx, k, i) * g;

            // subtract projection from v_j
            for (k=0; k<n; k++)
                matrix_access(_v, _rx, _cx, k, j) -= proj_ij[k];
        }

        // normalize v_j
        T vjj = 0.;     // dotprod(v_j, v_j)
        T tj  = 0.;
        for (k=0; k<n; k++) {
            tj = matrix_access(_v, _rx, _cx, k, j);
            T prodjj = tj * conj(tj);
            vjj += prodjj;
        }
        // TODO : check magnitude of vjj
        T g = 1. / sqrt( creal(vjj) );
        for (k=0; k<n; k++)
            matrix_access(_v, _rx, _cx, k, j) *= g;

#if DEBUG_MATRIX_GRAMSCHMIDT
        MATRIX(_print)(_v, _rx, _cx);
#endif
    }
}

