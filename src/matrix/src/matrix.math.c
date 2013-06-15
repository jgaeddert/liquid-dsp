/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2013 Joseph Gaeddert
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

// add elements of two matrices
//  _X      :   1st input matrix [size: _R x _C]
//  _Y      :   2nd input matrix [size: _R x _C]
//  _Z      :   output matrix [size: _R x _C]
//  _R      :   number of rows
//  _C      :   number of columns
void MATRIX(_add)(T * _X,
                  T * _Y,
                  T * _Z,
                  unsigned int _R,
                  unsigned int _C)
{
    unsigned int i;
    for (i=0; i<(_R*_C); i++) {
#if defined LIQUID_FIXED && T_COMPLEX==0
        _Z[i] = Q(_add)(_X[i], _Y[i]);
#elif defined LIQUID_FIXED && T_COMPLEX==1
        _Z[i] = CQ(_add)(_X[i], _Y[i]);
#else
        _Z[i] = _X[i] + _Y[i];
#endif
    }
}

// subtract elements of two matrices
//  _X      :   1st input matrix [size: _R x _C]
//  _Y      :   2nd input matrix [size: _R x _C]
//  _Z      :   output matrix [size: _R x _C]
//  _R      :   number of rows
//  _C      :   number of columns
void MATRIX(_sub)(T * _X,
                  T * _Y,
                  T * _Z,
                  unsigned int _R,
                  unsigned int _C)
{
    unsigned int i;
    for (i=0; i<(_R*_C); i++) {
#if defined LIQUID_FIXED && T_COMPLEX==0
        _Z[i] = Q(_sub)(_X[i], _Y[i]);
#elif defined LIQUID_FIXED && T_COMPLEX==1
        _Z[i] = CQ(_sub)(_X[i], _Y[i]);
#else
        _Z[i] = _X[i] - _Y[i];
#endif
    }
}

// point-wise multiplication
//  _X      :   1st input matrix [size: _R x _C]
//  _Y      :   2nd input matrix [size: _R x _C]
//  _Z      :   output matrix [size: _R x _C]
//  _R      :   number of rows
//  _C      :   number of columns
void MATRIX(_pmul)(T * _X,
                   T * _Y,
                   T * _Z,
                   unsigned int _R,
                   unsigned int _C)
{
    unsigned int i;
    for (i=0; i<(_R*_C); i++) {
#if defined LIQUID_FIXED && T_COMPLEX==0
        _Z[i] = Q(_mul)(_X[i], _Y[i]);
#elif defined LIQUID_FIXED && T_COMPLEX==1
        _Z[i] = CQ(_mul)(_X[i], _Y[i]);
#else
        _Z[i] = _X[i] * _Y[i];
#endif
    }
}

// point-wise division
//  _X      :   1st input matrix [size: _R x _C]
//  _Y      :   2nd input matrix [size: _R x _C]
//  _Z      :   output matrix [size: _R x _C]
//  _R      :   number of rows
//  _C      :   number of columns
void MATRIX(_pdiv)(T * _X,
                   T * _Y,
                   T * _Z,
                   unsigned int _R,
                   unsigned int _C)
{
    unsigned int i;
    for (i=0; i<(_R*_C); i++) {
#if defined LIQUID_FIXED && T_COMPLEX==0
        _Z[i] = Q(_div)(_X[i], _Y[i]);
#elif defined LIQUID_FIXED && T_COMPLEX==1
        _Z[i] = CQ(_div)(_X[i], _Y[i]);
#else
        _Z[i] = _X[i] / _Y[i];
#endif
    }
}


// multiply two matrices together
void MATRIX(_mul)(T * _X, unsigned int _XR, unsigned int _XC,
                  T * _Y, unsigned int _YR, unsigned int _YC,
                  T * _Z, unsigned int _ZR, unsigned int _ZC)
{
    // ensure lengths are valid
    if (_ZR != _XR || _ZC != _YC || _XC != _YR ) {
        fprintf(stderr,"error: matrix_mul(), invalid dimensions\n");
        exit(1);
    }

    unsigned int r, c, i;
    for (r=0; r<_ZR; r++) {
        for (c=0; c<_ZC; c++) {
            // z(i,j) = dotprod( x(i,:), y(:,j) )
#if defined LIQUID_FIXED && T_COMPLEX==0
            Q(_at) sum = 0; // accumulator (increased precision)
            for (i=0; i<_XC; i++) {
                sum += matrix_access(_X,_XR,_XC,r,i)*
                       matrix_access(_Y,_YR,_YC,i,c);
            }
            // shift result back by number of fractional bits
            matrix_access(_Z,_ZR,_ZC,r,c) = (sum >> Q(_fracbits));
#elif defined LIQUID_FIXED && T_COMPLEX==1
            // TODO: check this method
            Q(_at) sumi = 0;
            Q(_at) sumq = 0;
            for (i=0; i<_XC; i++) {
                // strip input values
                CQ(_t) a = matrix_access(_X,_XR,_XC,r,i);
                CQ(_t) b = matrix_access(_Y,_YR,_YC,i,c);

                // compute multiplication (only requires three arithmetic
                // multiplies) and accumulate into summing registers
                Q(_at) k1 = a.real * (b.real + b.imag);
                Q(_at) k2 = b.imag * (a.real + a.imag);
                Q(_at) k3 = b.real * (a.imag - a.real);

                sumi += (k1-k2);
                sumq += (k1+k3);
            }
            matrix_access(_Z,_ZR,_ZC,r,c).real = (sumi >> Q(_fracbits));
            matrix_access(_Z,_ZR,_ZC,r,c).imag = (sumq >> Q(_fracbits));
#else
            T sum=0.0f;
            for (i=0; i<_XC; i++) {
                sum += matrix_access(_X,_XR,_XC,r,i) *
                       matrix_access(_Y,_YR,_YC,i,c);
            }
            matrix_access(_Z,_ZR,_ZC,r,c) = sum;
#endif
            // DEBUG
            //printf("z(%u,%u) = ", r, c);
            //MATRIX_PRINT_ELEMENT(_Z,_ZR,_ZC,r,c);
            //printf("\n");
        }
    }
}

// augment matrices x and y:
//  z = [x | y]
void MATRIX(_aug)(T * _x, unsigned int _rx, unsigned int _cx,
                  T * _y, unsigned int _ry, unsigned int _cy,
                  T * _z, unsigned int _rz, unsigned int _cz)
{
    // ensure lengths are valid
    if (_rz != _rx || _rz != _ry || _rx != _ry || _cz != _cx + _cy) {
        fprintf(stderr,"error: matrix_aug(), invalid dimensions\n");
        exit(1);
    }

    // TODO: improve speed with memmove
    unsigned int r, c, n;
    for (r=0; r<_rz; r++) {
        n=0;
        for (c=0; c<_cx; c++)
            matrix_access(_z,_rz,_cz,r,n++) = matrix_access(_x,_rx,_cx,r,c);
        for (c=0; c<_cy; c++)
            matrix_access(_z,_rz,_cz,r,n++) = matrix_access(_y,_ry,_cy,r,c);
    }
}

// solve set of linear equations
void MATRIX(_div)(T * _X,
                  T * _Y,
                  T * _Z,
                  unsigned int _n)
{
#if defined LIQUID_FIXED
    fprintf(stderr,"warning: %s_div(), method not yet functional\n", MATRIX_NAME);
    exit(1);
#else
    // compute inv(_Y)
    T Y_inv[_n*_n];
    memmove(Y_inv, _Y, _n*_n*sizeof(T));
    MATRIX(_inv)(Y_inv,_n,_n);

    // _Z = _X * inv(_Y)
    MATRIX(_mul)(_X,    _n, _n,
                 Y_inv, _n, _n,
                 _Z,    _n, _n);
#endif
}

// matrix determinant (2 x 2)
T MATRIX(_det2x2)(T * _X,
                  unsigned int _r,
                  unsigned int _c)
{
    // validate input
    if (_r != 2 || _c != 2) {
        fprintf(stderr,"error: matrix_det2x2(), invalid dimensions\n");
        exit(1);
    }
#if defined LIQUID_FIXED && T_COMPLEX==0
    return Q(_sub)( Q(_mul)(_X[0],_X[3]),
                    Q(_mul)(_X[1],_X[2]) );
#elif defined LIQUID_FIXED && T_COMPLEX==1
    return CQ(_sub)( CQ(_mul)(_X[0],_X[3]),
                     CQ(_mul)(_X[1],_X[2]) );
#else
    return _X[0]*_X[3] - _X[1]*_X[2];
#endif
}

// matrix determinant (n x n)
T MATRIX(_det)(T * _X,
               unsigned int _r,
               unsigned int _c)
{
#if defined LIQUID_FIXED
    fprintf(stderr,"warning: %s_det(), method not yet functional\n", MATRIX_NAME);
    exit(1);
#else
    // validate input
    if (_r != _c) {
        fprintf(stderr,"error: matrix_det(), matrix must be square\n");
        exit(1);
    }
    unsigned int n = _r;
    if (n==2) return MATRIX(_det2x2)(_X,2,2);

    // compute L/U decomposition (Doolittle's method)
    T L[n*n]; // lower
    T U[n*n]; // upper
    T P[n*n]; // permutation
    MATRIX(_ludecomp_doolittle)(_X,n,n,L,U,P);

    // evaluate along the diagonal of U
    T det = 1.0;
    unsigned int i;
    for (i=0; i<n; i++)
        det *= matrix_access(U,n,n,i,i);

    return det;
#endif
}

// compute matrix transpose
void MATRIX(_trans)(T *          _X,
                    unsigned int _XR,
                    unsigned int _XC)
{
    // compute Hermitian transpose
    MATRIX(_hermitian)(_X,_XR,_XC);
    
#if defined LIQUID_FIXED && T_COMPLEX == 1
    // conjugate elements (fixed point)
    unsigned int i;
    for (i=0; i<_XR*_XC; i++)
        _X[i] = CQ(_conj)(_X[i]);
#else
    // conjugate elements (floating point)
    unsigned int i;
    for (i=0; i<_XR*_XC; i++)
        _X[i] = conj(_X[i]);
#endif
}

// compute matrix Hermitian transpose
void MATRIX(_hermitian)(T * _X,
                        unsigned int _XR,
                        unsigned int _XC)
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

// compute x*x' on m x n matrix, result: m x m
void MATRIX(_mul_transpose)(T *          _x,
                            unsigned int _m,
                            unsigned int _n,
                            T *          _xxT)
{
    unsigned int r;
    unsigned int c;
    unsigned int i;

    // permute rows
    for (r=0; r<_m; r++) {

        // permute columns
        for (c=0; c<_m; c++) {

#if defined LIQUID_FIXED && T_COMPLEX==0
            // TODO: check this
            Q(_at) sum = 0;
            for (i=0; i<_n; i++) {
                sum += matrix_access(_x,_m,_n,r,i) *
                       matrix_access(_x,_m,_n,c,i);
            }
            matrix_access(_xxT,_m,_m,r,c) = (sum >> Q(_fracbits));
#elif defined LIQUID_FIXED && T_COMPLEX==1
            // TODO: check this
            Q(_at) sumi = 0;
            Q(_at) sumq = 0;
            for (i=0; i<_n; i++) {
                // strip input values
                CQ(_t) a = matrix_access(_x,_m,_n,r,i);
                CQ(_t) b = matrix_access(_x,_m,_n,c,i);

                // compute multiplication (only requires three arithmetic
                // multiplies) and accumulate into summing registers
                Q(_at) k1 = a.real * (b.real + b.imag);
                Q(_at) k2 = b.imag * (a.real + a.imag);
                Q(_at) k3 = b.real * (a.imag - a.real);

                sumi += (k1-k2);
                sumq += (k1+k3);
            }
            matrix_access(_xxT,_m,_m,r,c).real = (sumi >> Q(_fracbits));
            matrix_access(_xxT,_m,_m,r,c).imag = (sumq >> Q(_fracbits));
#else
            // floating-point precision
            T sum = 0.0f;
            for (i=0; i<_n; i++) {
                sum +=        matrix_access(_x,_m,_n,r,i) *
                       conjf( matrix_access(_x,_m,_n,c,i) );
            }
            matrix_access(_xxT,_m,_m,r,c) = sum;
#endif
        }
    }
}


// compute x'*x on m x n matrix, result: n x n
void MATRIX(_transpose_mul)(T *          _x,
                            unsigned int _m,
                            unsigned int _n,
                            T *          _xTx)
{
    unsigned int r;
    unsigned int c;
    unsigned int i;

    // permute rows
    for (r=0; r<_n; r++) {

        // permute columns
        for (c=0; c<_n; c++) {
#if defined LIQUID_FIXED && T_COMPLEX==0
            // TODO: check this
            Q(_at) sum = 0;
            for (i=0; i<_n; i++) {
                sum += matrix_access(_x,_m,_n,i,r) *
                       matrix_access(_x,_m,_n,i,c);
            }
            matrix_access(_xTx,_n,_n,r,c) = (sum >> Q(_fracbits));
#elif defined LIQUID_FIXED && T_COMPLEX==1
            // TODO: check this
            Q(_at) sumi = 0;
            Q(_at) sumq = 0;
            for (i=0; i<_n; i++) {
                // strip input values
                CQ(_t) a = matrix_access(_x,_m,_n,i,r);
                CQ(_t) b = matrix_access(_x,_m,_n,i,c);

                // compute multiplication (only requires three arithmetic
                // multiplies) and accumulate into summing registers
                Q(_at) k1 = a.real * (b.real + b.imag);
                Q(_at) k2 = b.imag * (a.real + a.imag);
                Q(_at) k3 = b.real * (a.imag - a.real);

                sumi += (k1-k2);
                sumq += (k1+k3);
            }
            matrix_access(_xTx,_n,_n,r,c).real = (sumi >> Q(_fracbits));
            matrix_access(_xTx,_n,_n,r,c).imag = (sumq >> Q(_fracbits));
#else
            // floating-point precision
            T sum = 0.0f;
            for (i=0; i<_m; i++) {
                sum += conjf( matrix_access(_x,_m,_n,i,r) ) *
                              matrix_access(_x,_m,_n,i,c);
            }

            matrix_access(_xTx,_n,_n,r,c) = sum;
#endif
        }
    }
}


// compute x*x.' on m x n matrix, result: m x m
void MATRIX(_mul_hermitian)(T * _x,
                            unsigned int _m,
                            unsigned int _n,
                            T * _xxH)
{
#if defined LIQUID_FIXED
    fprintf(stderr,"warning: %s_mul_hermitian(), method not yet functional\n", MATRIX_NAME);
    exit(1);
#else
    unsigned int r;
    unsigned int c;
    unsigned int i;

    // clear _xxH
    for (i=0; i<_m*_m; i++)
        _xxH[i] = 0.0f;

    // 
    T sum = 0;
    for (r=0; r<_m; r++) {

        for (c=0; c<_m; c++) {
            sum = 0.0f;

            for (i=0; i<_n; i++) {
                sum += matrix_access(_x,_m,_n,r,i) *
                       matrix_access(_x,_m,_n,c,i);
            }

            matrix_access(_xxH,_m,_m,r,c) = sum;
        }
    }
#endif
}


// compute x.'*x on m x n matrix, result: n x n
void MATRIX(_hermitian_mul)(T * _x,
                            unsigned int _m,
                            unsigned int _n,
                            T * _xHx)
{
#if defined LIQUID_FIXED
    fprintf(stderr,"warning: %s_hermitian_mul(), method not yet functional\n", MATRIX_NAME);
    exit(1);
#else
    unsigned int r;
    unsigned int c;
    unsigned int i;

    // clear _xHx
    for (i=0; i<_n*_n; i++)
        _xHx[i] = 0.0f;

    // 
    T sum = 0;
    for (r=0; r<_n; r++) {

        for (c=0; c<_n; c++) {
            sum = 0.0f;

            for (i=0; i<_m; i++) {
                sum += matrix_access(_x,_m,_n,i,r) *
                       matrix_access(_x,_m,_n,i,c);
            }

            matrix_access(_xHx,_n,_n,r,c) = sum;
        }
    }
#endif
}

