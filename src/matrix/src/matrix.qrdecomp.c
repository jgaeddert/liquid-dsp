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
// Matrix Q/R decomposition method definitions
//

#include <math.h>
#include "liquid.internal.h"

#define DEBUG_MATRIX_QRDECOMP 1

// Q/R decomposition using the Gram-Schmidt algorithm
void MATRIX(_qrdecomp_gramschmidt)(T * _x,
                                   unsigned int _rx,
                                   unsigned int _cx,
                                   T * _Q,
                                   T * _R)
{
    // validate input
    if (_rx != _cx) {
        fprintf(stderr,"error: matrix_qrdecomp_gramschmidt(), input matrix not square\n");
        exit(-1);
    }
    unsigned int n = _rx;

    unsigned int i;
    unsigned int j;
    unsigned int k;

    // generate and initialize matrices
    T e[n*n];   // normalized...
    for (i=0; i<n*n; i++)
        e[i] = 0.0f;

    for (k=0; k<n; k++) {
        // e(i,k) <- _x(i,k)
        for (i=0; i<n; i++)
            matrix_access(e,n,n,i,k) = matrix_access(_x,n,n,i,k);

        // subtract...
        for (i=0; i<k; i++) {
            // compute dot product _x(:,k) * e(:,i)
            T g = 0;
            for (j=0; j<n; j++) {
                T prod = matrix_access(_x,n,n,j,k) * conj( matrix_access(e,n,n,j,i) );
                g += prod;
            }
            //printf("  i=%2u, g = %12.4e\n", i, crealf(g));
            for (j=0; j<n; j++)
                matrix_access(e,n,n,j,k) -= matrix_access(e,n,n,j,i) * g;
        }

        // compute e_k = e_k / |e_k|
        float ek = 0.0f;
        T ak;
        for (i=0; i<n; i++) {
            ak = matrix_access(e,n,n,i,k);
            ek += fabsf( ak*conjf(ak) );
        }
        ek = sqrtf(ek);

        // normalize e
        for (i=0; i<n; i++)
            matrix_access(e,n,n,i,k) /= ek;
    }

    // move Q
    memmove(_Q, e, n*n*sizeof(T));

    // compute R
    // j : row
    // k : column
    for (j=0; j<n; j++) {
        for (k=0; k<n; k++) {
            if (k < j) {
                matrix_access(_R,n,n,j,k) = 0.0f;
            } else {
                // compute dot product between and Q(:,j) and _x(:,k)
                T g = 0;
                for (i=0; i<n; i++) {
                    T prod = conj( matrix_access(_Q,n,n,i,j) ) * matrix_access(_x,n,n,i,k);
                    g += prod;
                }
                matrix_access(_R,n,n,j,k) = g;
            }
        }
    }
}

