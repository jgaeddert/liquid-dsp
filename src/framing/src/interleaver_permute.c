/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011  Virginia Polytechnic
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
// interleaver_permute.c
//
// interleaver permutation functions
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

// permute forward one iteration
//  _x      :   input/output data array, [size: _n x 1]
//  _n      :   array size
void interleaver_permute_forward(unsigned char * _x,
                                 unsigned int _n)
{
}

// permute reverse one iteration
//  _x      :   input/output data array, [size: _n x 1]
//  _n      :   array size
void interleaver_permute_reverse(unsigned char * _x,
                                 unsigned int _n)
{
}


// permute forward one iteration with byte mask
//  _x      :   input/output data array, [size: _n x 1]
//  _n      :   array size
//  _mask   :   byte mask
void interleaver_permute_forward_mask(unsigned char * _x,
                                      unsigned int _n,
                                      unsigned char _mask)
{
}

// permute reverse one iteration with byte mask
//  _x      :   input/output data array, [size: _n x 1]
//  _n      :   array size
//  _mask   :   byte mask
void interleaver_permute_reverse_mask(unsigned char * _x,
                                      unsigned int _n,
                                      unsigned char _mask)
{
}


// compute bit permutation for interleaver
//  _q      :   interleaver object
//  _p      :   output permutation index array, [size: 8*_n x 1]
void interleaver_compute_bit_permutation(interleaver _q,
                                         unsigned int * _p)
{
    unsigned int i, j;
    unsigned char x[_q->n], y[_q->n];

    for (i=0; i<_q->n; i++)
        x[i] = 0;

    for (i=0; i<_q->n; i++) {
        for (j=0; j<8; j++) {
            x[i] = 1<<j;
            interleaver_encode(_q, x, y);
            // find where the bit went!
            // look for byte containing bit
            unsigned int k;
            for (k=0; k<_q->n; k++) {
                if (y[k] > 0)
                    break;
            }
            // find bit position
            unsigned char v = y[k];
            unsigned int r;
            for (r=0; r<8; r++) {
                if (v & 1)
                    break;
                v >>= 1;
            }
            _p[8*i + j] = 8*k + r;
        }
        x[i] = 0;
    }
}
