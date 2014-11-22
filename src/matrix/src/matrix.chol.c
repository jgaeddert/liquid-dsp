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
// Matrix Cholesky decomposition method definitions
//

#include <math.h>
#include "liquid.internal.h"

#define DEBUG_MATRIX_CHOL 0

// Compute Cholesky decomposition of a symmetric/Hermitian positive-
// definite matrix as A = L * L^T
//  _A      :   input square matrix [size: _n x _n]
//  _n      :   input matrix dimension
//  _L      :   output lower-triangular matrix
void MATRIX(_chol)(T * _A,
                   unsigned int _n,
                   T * _L)
{
    // reset L
    unsigned int i;
    for (i=0; i<_n*_n; i++)
        _L[i] = 0.0;

    unsigned int j;
    unsigned int k;
    T  A_jj;
    T  L_jj;
    T  L_ik;
    T  L_jk;
    TP t0;
    T  t1;
    for (j=0; j<_n; j++) {
        // assert that A_jj is real, positive
        A_jj = matrix_access(_A,_n,_n,j,j);
        if ( creal(A_jj) < 0.0 ) {
            fprintf(stderr,"warning: matrix_chol(), matrix is not positive definite (real{A[%u,%u]} = %12.4e < 0)\n",j,j,creal(A_jj));
            return;
        }
#if T_COMPLEX
        if ( fabs(cimag(A_jj)) > 0.0 ) {
            fprintf(stderr,"warning: matrix_chol(), matrix is not positive definite (|imag{A[%u,%u]}| = %12.4e > 0)\n",j,j,fabs(cimag(A_jj)));
            return;
        }
#endif

        // compute L_jj and store it in output matrix
        t0 = 0.0;
        for (k=0; k<j; k++) {
            L_jk = matrix_access(_L,_n,_n,j,k);
#if T_COMPLEX
            t0 += creal( L_jk * conj(L_jk) );
#else
            t0 += L_jk * L_jk;
#endif
        }
        // test to ensure A_jj > t0
        if ( creal(A_jj) < t0 ) {
            fprintf(stderr,"warning: matrix_chol(), matrix is not positive definite (real{A[%u,%u]} = %12.4e < %12.4e)\n",j,j,creal(A_jj),t0);
            return;
        }
        L_jj = sqrt( A_jj - t0 );
        matrix_access(_L,_n,_n,j,j) = L_jj;

        for (i=j+1; i<_n; i++) {
            t1 = matrix_access(_A,_n,_n,i,j);
            for (k=0; k<j; k++) {
                L_ik = matrix_access(_L,_n,_n,i,k);
                L_jk = matrix_access(_L,_n,_n,j,k);
#if T_COMPLEX
                t1 -= L_ik * conj(L_jk);
#else
                t1 -= L_ik * L_jk;
#endif
            }
            // TODO : store inverse of L_jj to reduce number of divisions
            matrix_access(_L,_n,_n,i,j) = t1 / L_jj;
        }
    }
}

