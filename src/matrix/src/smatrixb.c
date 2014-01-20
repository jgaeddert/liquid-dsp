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
// sparse matrix API: boolean
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "liquid.internal.h"

// name-mangling macro
#define SMATRIX(name)       LIQUID_CONCAT(smatrixb,name)

// primitive type
#define T                   unsigned char

// category (float/int/bool)
#define SMATRIX_FLOAT       0
#define SMATRIX_INT         0
#define SMATRIX_BOOL        1

// print macros
#define PRINTVAL_ZERO()     printf(" .");
#define PRINTVAL(V)         printf(" %1u", V);

// source files
#include "smatrix.c"

// 
// smatrix cross methods
//

// multiply sparse binary matrix by floating-point matrix
//  _q  :   sparse matrix [size: A->M x A->N]
//  _x  :   input vector  [size:  mx  x  nx ]
//  _y  :   output vector [size:  my  x  ny ]
void smatrixb_mulf(smatrixb     _A,
                   float *      _x,
                   unsigned int _mx,
                   unsigned int _nx,
                   float *      _y,
                   unsigned int _my,
                   unsigned int _ny)
{
    // ensure lengths are valid
    if (_my != _A->M || _ny != _nx || _A->N != _mx ) {
        fprintf(stderr,"error: matrix_mul(), invalid dimensions\n");
        exit(1);
    }
    unsigned int i;
    unsigned int j;

    // clear output matrix
    for (i=0; i<_my*_ny; i++)
        _y[i] = 0.0f;

    //
    for (i=0; i<_A->M; i++) {
        // find non-zero column entries in this row
        unsigned int p;
        for (p=0; p<_A->num_mlist[i]; p++) {
            for (j=0; j<_ny; j++) {
                //_y(i,j) += _x( _A->mlist[i][p], j);
                _y[i*_ny + j] += _x[ _A->mlist[i][p]*_nx + j];
            }
        }
    }
}

// multiply sparse binary matrix by floating-point vector
//  _q  :   sparse matrix
//  _x  :   input vector [size: _N x 1]
//  _y  :   output vector [size: _M x 1]
void smatrixb_vmulf(smatrixb _q,
                    float *  _x,
                    float *  _y)
{
    unsigned int i;
    unsigned int j;
    
    for (i=0; i<_q->M; i++) {

        // reset total
        _y[i] = 0.0f;

        // only accumulate values on non-zero entries
        for (j=0; j<_q->num_mlist[i]; j++)
            _y[i] += _x[ _q->mlist[i][j] ];
    }
}

