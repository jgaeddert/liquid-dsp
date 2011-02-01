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
// pack_bytes.c
//
// Useful byte-packing utilities
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// move array to the right, filling in zeros appropriately
//  _dst        :   destination address [size: _n+1 x 1]
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _num_bits   :   number of bits to shift
void liquid_rmemmove(unsigned char * _dst,
                     unsigned char * _src,
                     unsigned int _n,
                     unsigned int _num_bits)
{
    // validate input
    if (_num_bits >= 8) {
        fprintf(stderr,"error: liquid_rmemmove(), bit shift must be in [0,7]\n");
        exit(1);
    }

    // check condition where bit shift is zero
    if (_num_bits == 0) {
        // move source data to destination
        memmove(_dst, _src, _n);

        // set last byte to zero
        _dst[_n] = 0x00;

        // return
        return;
    }

    // move left by reverse amount
    liquid_lmemmove(_dst, _src, _n, 8-_num_bits);
}
 
// move array to the right, filling in zeros appropriately
//  _dst        :   destination address [size: _n+1 x 1]
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _num_bits   :   number of bits to shift
void liquid_lmemmove(unsigned char * _dst,
                     unsigned char * _src,
                     unsigned int _n,
                     unsigned int _num_bits)
{
    // validate input
    if (_num_bits >= 8) {
        fprintf(stderr,"error: liquid_lmemmove(), bit shift must be in [0,7]\n");
        exit(1);
    }

    // copy memory to destination; this handles case when the
    // _src and _dst addresses overlap
    memmove(_dst+1, _src, _n);
    _dst[0] = 0x00;

    // 
    unsigned int shift_0 = _num_bits;       // shift amount: first byte
    unsigned int shift_1 = 8 - _num_bits;   // shift amount: second byte
    unsigned char mask_0 = 0xff << shift_0; // bit mask: first byte
    unsigned char mask_1 = 0xff >> shift_1; // bit mask: second byte
#if 0
    printf("num_bits = %1u, shift_0 = %1u, shift_1 = %1u mask_0=%.2x, mask_1=%.2x\n",
            _num_bits, shift_0, shift_1, mask_0, mask_1);
#endif
    // shift then mask
    unsigned int i;
    unsigned char byte;
    unsigned char byte_0;
    unsigned char byte_1;
    for (i=0; i<=_n; i++) {
        // strip bytes
        byte_0 = (i==0)  ? 0 : _dst[i+0];
        byte_1 = (i==_n) ? 0 : _dst[i+1];

        // shift then mask
        byte = ((byte_0 << shift_0) & mask_0) |
               ((byte_1 >> shift_1) & mask_1);

        // store result
        _dst[i] = byte;
    }
}
 
