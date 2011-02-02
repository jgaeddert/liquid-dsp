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

 
// shift array to the left _b bits, filling in zeros
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bits to shift
void liquid_lbshift(unsigned char * _src,
                    unsigned int _n,
                    unsigned int _b)
{
    // validate input
    if (_b >= 8) {
        fprintf(stderr,"error: liquid_lbshift(), shift amount must be in [0,7]\n");
        exit(1);
    }

    // 
    unsigned int shift_0 = _b;              // shift amount: first byte
    unsigned int shift_1 = 8 - _b;          // shift amount: second byte
    unsigned char mask_0 = 0xff << shift_0; // bit mask: first byte
    unsigned char mask_1 = 0xff >> shift_1; // bit mask: second byte
#if 0
    printf("num_bits = %1u, shift_0 = %1u, shift_1 = %1u mask_0=%.2x, mask_1=%.2x\n",
            _b, shift_0, shift_1, mask_0, mask_1);
#endif
    // shift then mask
    unsigned int i;
    unsigned char byte;
    unsigned char byte_0;
    unsigned char byte_1;
    for (i=0; i<_n; i++) {
        // strip bytes
        byte_0 = _src[i];
        byte_1 = (i==_n-1) ? 0 : _src[i+1];

        // shift then mask
        byte = ((byte_0 << shift_0) & mask_0) |
               ((byte_1 >> shift_1) & mask_1);

        // store result
        _src[i] = byte;
    }
}

 
// shift array to the right _b bits filling in zeros
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bits to shift
void liquid_rbshift(unsigned char * _src,
                    unsigned int _n,
                    unsigned int _b)
{
    // validate input
    if (_b >= 8) {
        fprintf(stderr,"error: liquid_rbshift(), shift amount must be in [0,7]\n");
        exit(1);
    }

    // 
    unsigned int shift_0 = 8 - _b;              // shift amount: first byte
    unsigned int shift_1 = _b;          // shift amount: second byte
    unsigned char mask_0 = 0xff << shift_0; // bit mask: first byte
    unsigned char mask_1 = 0xff >> shift_1; // bit mask: second byte
#if 0
    printf("num_bits = %1u, shift_0 = %1u, shift_1 = %1u mask_0=%.2x, mask_1=%.2x\n",
            _b, shift_0, shift_1, mask_0, mask_1);
#endif
    // shift then mask
    int i;
    unsigned char byte;
    unsigned char byte_0;
    unsigned char byte_1;
    for (i=_n-1; i>=0; i--) {
        // strip bytes
        byte_0 = (i==0) ? 0 : _src[i-1];
        byte_1 = _src[i];

        // shift then mask
        byte = ((byte_0 << shift_0) & mask_0) |
               ((byte_1 >> shift_1) & mask_1);

        // store result
        _src[i] = byte;
    }
}

 
// circular shift array to the right _n bytes
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bytes to shift
void liquid_rcircshift(unsigned char * _src,
                       unsigned int _n,
                       unsigned int _b)
{
    // validate input
    if (_n == 0)
        return;

    // ensure 0 <= _b < _n
    _b = _b % _n;

    // check if less memory is used with lcircshift
    if (_b > (_n>>1)) {
        liquid_lcircshift(_src, _n, _n-_b);
        return;
    }

    // allocate memory for temporary array
    unsigned char * tmp = (unsigned char*) malloc(_b*sizeof(unsigned char));

    // copy to temporary array
    memmove(tmp, &_src[_n-_b], _b*sizeof(unsigned char));

    // shift right
    memmove(&_src[_b], _src, (_n-_b)*sizeof(unsigned char));

    // copy from temporary array
    memmove(_src, tmp, _b*sizeof(unsigned char));

    // free temporary array
    free(tmp);
}
 
// circular shift array to the left _n bytes
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bytes to shift
void liquid_lcircshift(unsigned char * _src,
                       unsigned int _n,
                       unsigned int _b)
{
    // validate input
    if (_n == 0)
        return;

    // ensure 0 <= _b < _n
    _b = _b % _n;

    // check if less memory is used with rcircshift
    if (_b > (_n>>1)) {
        liquid_rcircshift(_src, _n, _n-_b);
        return;
    }

    // allocate memory for temporary array
    unsigned char * tmp = (unsigned char*) malloc(_b*sizeof(unsigned char));

    // copy to temporary array
    memmove(tmp, _src, _b*sizeof(unsigned char));

    // shift left
    memmove(_src, &_src[_b], (_n-_b)*sizeof(unsigned char));

    // copy from temporary array
    memmove(&_src[_n-_b], tmp, _b*sizeof(unsigned char));

    // free temporary array
    free(tmp);
}


