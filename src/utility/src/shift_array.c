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
// shift_array.c
//
// byte-wise array shifting
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"


// shift array to the left _b bytes, filling in zeros
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bytes to shift
void liquid_lshift(unsigned char * _src,
                   unsigned int _n,
                   unsigned int _b)
{
    // shift amount exceeds buffer size; fill with zeros
    if (_b >= _n) {
        memset(_src, 0x00, _n*sizeof(unsigned char));
        return;
    }

    // move memory
    memmove(_src, &_src[_b], (_n-_b)*sizeof(unsigned char));

    // fill remaining buffer with zeros
    memset(&_src[_n-_b], 0x00, _b*sizeof(unsigned char));
}
 
// shift array to the right _b bytes, filling in zeros
//  _src        :   source address [size: _n x 1]
//  _n          :   input data array size
//  _b          :   number of bytes to shift
void liquid_rshift(unsigned char * _src,
                   unsigned int _n,
                   unsigned int _b)
{
    // shift amount exceeds buffer size; fill with zeros
    if (_b >= _n) {
        memset(_src, 0x00, _n*sizeof(unsigned char));
        return;
    }

    // move memory
    memmove(&_src[_b], _src, (_n-_b)*sizeof(unsigned char));

    // fill remaining buffer with zeros
    memset(_src, 0x00, _b*sizeof(unsigned char));
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
 
