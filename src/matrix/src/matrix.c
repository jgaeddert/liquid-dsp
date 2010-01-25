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
// Matrix method definitions
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void MATRIX(_print)(T * _X, unsigned int _R, unsigned int _C)
{
    printf("matrix [%u x %u] : \n", _R, _C);
    unsigned int r,c;
    for (r=0; r<_R; r++) {
        for (c=0; c<_C; c++) {
            MATRIX_PRINT_ELEMENT(_X,_R,_C,r,c);
        }
        printf("\n");
    }
}

void MATRIX(_add)(unsigned int _R, unsigned int _C,
                  T * _X, T * _Y, T * _Z)
{
    unsigned int i;
    for (i=0; i<(_R*_C); i++)
        _Z[i] = _X[i] + _Y[i];
}

void MATRIX(_sub)(unsigned int _R, unsigned int _C,
                  T * _X, T * _Y, T * _Z)
{
    unsigned int i;
    for (i=0; i<(_R*_C); i++)
        _Z[i] = _X[i] - _Y[i];
}

void MATRIX(_mul)(T * _X, unsigned int _XR, unsigned int _XC,
                  T * _Y, unsigned int _YR, unsigned int _YC,
                  T * _Z, unsigned int _ZR, unsigned int _ZC)
{
    // ensure lengths are valid
    if (_ZR != _XR || _ZC != _YC || _XC != _YR ) {
        fprintf(stderr,"error: matrix_multiply(), invalid dimensions\n");
        exit(0);
    }

    unsigned int r, c, i;
    for (r=0; r<_ZR; r++) {
        for (c=0; c<_ZC; c++) {
            // z(i,j) = dotprod( x(i,:), y(:,j) )
            T sum=0.0f;
            for (i=0; i<_XC; i++) {
                sum += matrix_access(_X,_XR,_XC,r,i) *
                       matrix_access(_Y,_YR,_YC,i,c);
            }
            matrix_access(_Z,_ZR,_ZC,r,c) = sum;
#ifdef DEBUG
            printf("z(%u,%u) = ", r, c);
            MATRIX_PRINT_ELEMENT(_Z,_ZR,_ZC,r,c);
            printf("\n");
#endif
        }
    }
}

void MATRIX(_div)(T * _X, unsigned int _XR, unsigned int _XC,
                  T * _Y, unsigned int _YR, unsigned int _YC,
                  T * _Z, unsigned int _ZR, unsigned int _ZC)
{
    // ensure all matrices are square and the same size
    unsigned int n = _XR;
    if (_ZR != _XR || _ZC != _YC || _XC != _YR ) {
        fprintf(stderr,"error: matrix_div(), invalid dimensions\n");
        exit(-1);
    } else if ( _XC != n || _YR != n || _YC != n || _ZR != n || _ZC != n) {
        fprintf(stderr,"error: matrix_div(), invalid dimensions\n");
        exit(-1);
    }

    // compute inv(_Y)
    T Y_inv[n*n];
    memmove(Y_inv, _Y, n*n*sizeof(T));
    MATRIX(_inv)(Y_inv,n,n);

    // _Z = _X * inv(_Y)
    MATRIX(_mul)(_X,    n, n,
                 Y_inv, n, n,
                 _Z,    n, n);
}


// compute matrix transpose
void MATRIX(_trans)(T * _X, unsigned int _XR, unsigned int _XC)
{
    T y[_XR*_XC];
    memmove(y,_X,_XR*_XC*sizeof(T));

    unsigned int r,c;
    for (r=0; r<_XR; r++) {
        for (c=0; c<_XC; c++) {
            matrix_access(_X,_XC,_XR,c,r) = matrix_access(y,_XR,_XC,r,c);
        }
    }
}

// initialize square matrix to the identity matrix
void MATRIX(_eye)(T * _X, unsigned int _n)
{
    unsigned int r,c,k=0;
    for (r=0; r<_n; r++) {
        for (c=0; c<_n; c++) {
            _X[k++] = r==c ? 1. : 0.;
        }
    }
}


