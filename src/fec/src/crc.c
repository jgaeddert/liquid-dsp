/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Cyclic redundancy check
// 

#include "liquid.internal.h"

// slow implementation
unsigned char reverse_byte(unsigned char _x)
{
    unsigned char y = 0x00;
    unsigned int i;
    for (i=0; i<8; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}



// 
// CRC-16
//

// reverse integer with 16 bits of data
unsigned int reverse_uint16(unsigned int _x)
{
    unsigned int i, y=0;
    for (i=0; i<16; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}

// generate 16-bit cyclic redundancy check key.
// slow method, operates one bit at a time
// algorithm from: http://www.hackersdelight.org/crc.pdf
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
unsigned int crc16_generate_key(unsigned char *_msg,
                                unsigned int _n)
{
    unsigned int i, j, b, mask, key16=~0;
    unsigned int poly = reverse_uint16(CRC16_POLY);
    for (i=0; i<_n; i++) {
        b = _msg[i];
        key16 ^= b;
        for (j=0; j<8; j++) {
            mask = -(key16 & 1);
            key16 = (key16>>1) ^ (poly & mask);
        }
    }
    return ~key16;
}

#if 0
void crc32_generate_key(unsigned char *_msg, unsigned int _n, unsigned char *_key)
{
    unsigned int key32 = crc32_generate_key32(_msg,_n);
    _key[0] = (key32 & 0xFF000000) >> 24;
    _key[1] = (key32 & 0x00FF0000) >> 16;
    _key[2] = (key32 & 0x0000FF00) >> 8;
    _key[3] = (key32 & 0x000000FF);
}
#endif

// validate message with 16-bit CRC
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
//  _key    :   16-bit CRC key
int crc16_validate_message(unsigned char *_msg,
                           unsigned int _n,
                           unsigned int _key)
{
    return crc16_generate_key(_msg,_n)==_key;
}


// 
// CRC-32
//

// reverse integer with 32 bits of data
unsigned int reverse_uint32(unsigned int _x)
{
    unsigned int i, y=0;
    for (i=0; i<32; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}

// generate 32-bit cyclic redundancy check key.
//
// slow method, operates one bit at a time
// algorithm from: http://www.hackersdelight.org/crc.pdf
//
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
unsigned int crc32_generate_key(unsigned char *_msg,
                                unsigned int _n)
{
    unsigned int i, j, b, mask, key32=~0;
    unsigned int poly = reverse_uint32(CRC32_POLY);
    for (i=0; i<_n; i++) {
        b = _msg[i];
        key32 ^= b;
        for (j=0; j<8; j++) {
            mask = -(key32 & 1);
            key32 = (key32>>1) ^ (poly & mask);
        }
    }
    return ~key32;
}

#if 0
void crc32_generate_key(unsigned char *_msg, unsigned int _n, unsigned char *_key)
{
    unsigned int key32 = crc32_generate_key32(_msg,_n);
    _key[0] = (key32 & 0xFF000000) >> 24;
    _key[1] = (key32 & 0x00FF0000) >> 16;
    _key[2] = (key32 & 0x0000FF00) >> 8;
    _key[3] = (key32 & 0x000000FF);
}
#endif

// validate message with 32-bit CRC
//  _msg    :   input data message [size: _n x 1]
//  _n      :   input data message size
//  _key    :   32-bit CRC key
int crc32_validate_message(unsigned char *_msg,
                           unsigned int _n,
                           unsigned int _key)
{
    return crc32_generate_key(_msg,_n)==_key;
}


