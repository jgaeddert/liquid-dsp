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
// Matrix L/U decomposition method definitions
//

#include "liquid.internal.h"

// L/U/P decomposition, Crout's method
void MATRIX(_ludecomp_crout)(T * _x,
                             unsigned int _rx,
                             unsigned int _cx,
                             T * _L,
                             T * _U,
                             T * _P)
{
    // validate input
    if (_rx != _cx) {
        fprintf(stderr,"error: matrix_ludecomp_crout(), input matrix not square\n");
        exit(-1);
    }
    unsigned int n = _rx;

    // reset L, U
    unsigned int i;
    for (i=0; i<n*n; i++) {
        _L[i] = 0.0;
        _U[i] = 0.0;
        _P[i] = 0.0;
    }

    unsigned int j,k,t;
    T L_ik, U_kj;
    for (k=0; k<n; k++) {
        for (i=k; i<n; i++) {
            L_ik = matrix_access(_x,n,n,i,k);
            for (t=0; t<k; t++) {
                L_ik -= matrix_access(_L,n,n,i,t)*
                        matrix_access(_U,n,n,t,k);
            }
            matrix_access(_L,n,n,i,k) = L_ik;
        }

        for (j=k; j<n; j++) {
            // set upper triangular matrix to unity on diagonal
            if (j==k) {
                matrix_access(_U,n,n,k,j) = 1.0f;
                continue;
            }

            U_kj = matrix_access(_x,n,n,k,j);
            for (t=0; t<k; t++) {
                U_kj -= matrix_access(_L,n,n,k,t)*
                        matrix_access(_U,n,n,t,j);
            }
            U_kj /= matrix_access(_L,n,n,k,k);
            matrix_access(_U,n,n,k,j) = U_kj;
        }
    }

    // set output permutation matrix to identity matrix
    MATRIX(_eye)(_P,n);
}

// L/U/P decomposition, Doolittle's method
void MATRIX(_ludecomp_doolittle)(T * _x,
                                 unsigned int _rx,
                                 unsigned int _cx,
                                 T * _L,
                                 T * _U,
                                 T * _P)
{
    // validate input
    if (_rx != _cx) {
        fprintf(stderr,"error: matrix_ludecomp_doolittle(), input matrix not square\n");
        exit(-1);
    }
    unsigned int n = _rx;

    // reset L, U
    unsigned int i;
    for (i=0; i<n*n; i++) {
        _L[i] = 0.0;
        _U[i] = 0.0;
        _P[i] = 0.0;
    }

    unsigned int j,k,t;
    T U_kj, L_ik;
    for (k=0; k<n; k++) {
        // compute upper triangular matrix
        for (j=k; j<n; j++) {
            U_kj = matrix_access(_x,n,n,k,j);
            for (t=0; t<k; t++) {
                U_kj -= matrix_access(_L,n,n,k,t)*
                        matrix_access(_U,n,n,t,j);
            }
            matrix_access(_U,n,n,k,j) = U_kj;
        }

        // compute lower triangular matrix
        for (i=k; i<n; i++) {
            // set lower triangular matrix to unity on diagonal
            if (i==k) {
                matrix_access(_L,n,n,i,k) = 1.0f;
                continue;
            }

            L_ik = matrix_access(_x,n,n,i,k);
            for (t=0; t<k; t++) {
                L_ik -= matrix_access(_L,n,n,i,t)*
                        matrix_access(_U,n,n,t,k);
            }
            L_ik /= matrix_access(_U,n,n,k,k);
            matrix_access(_L,n,n,i,k) = L_ik;
        }
    }

    // set output permutation matrix to identity matrix
    MATRIX(_eye)(_P,n);
}

