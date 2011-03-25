/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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

// L/U/P decomposition, Crout's method
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

    T u[n*n];   // normal...
    T e[n*n];   // normalized...

    unsigned int i;
    unsigned int j;
    unsigned int k;
    for (k=0; k<n; k++) {
        for (i=0; i<n; i++)
            matrix_access(u,n,n,i,k) = matrix_access(_x,n,n,i,k);

        // subtract...
        for (i=0; i<k; i++) {
            T g = 0.0f;
            for (j=0; j<n; j++)
                g += matrix_access(_x,n,n,j,k) * matrix_access(e,n,n,j,i);
            matrix_access(u,n,n,i,k) -= matrix_access(e,n,n,i,k) * g;
        }

        // compute e_k = e_k / |e_k|
        float ek = 0.0f;
        T ak;
        for (i=0; i<n; i++) {
            ak = matrix_access(u,n,n,i,k);
            ek += fabsf( ak*conjf(ak) );
        }
        ek = sqrtf(ek);

        // compute normalized...
        for (i=0; i<n; i++)
            matrix_access(e,n,n,i,k) = matrix_access(u,n,n,i,k) / ek;

    }

    memmove(_Q, e, n*n*sizeof(T));

}

