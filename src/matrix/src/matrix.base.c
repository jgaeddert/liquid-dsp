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
// Matrix method base definitions
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void MATRIX(_print)(T * _X,
                    unsigned int _R,
                    unsigned int _C)
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

// initialize square matrix to the identity matrix
void MATRIX(_eye)(T * _x, unsigned int _n)
{
    unsigned int r,c,k=0;
    for (r=0; r<_n; r++) {
        for (c=0; c<_n; c++) {
            _x[k++] = r==c ? 1. : 0.;
        }
    }
}

// initialize matrix to ones
void MATRIX(_ones)(T * _x,
                   unsigned int _r,
                   unsigned int _c)
{
    unsigned int k;
    for (k=0; k<_r*_c; k++)
        _x[k] = 1.;
}

// initialize matrix to zeros
void MATRIX(_zeros)(T * _x,
                    unsigned int _r,
                    unsigned int _c)
{
    unsigned int k;
    for (k=0; k<_r*_c; k++)
        _x[k] = 0.;
}

