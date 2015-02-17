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

