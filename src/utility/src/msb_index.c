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
// Index of most-significant bit
//
// Examples:
//  0x00000000  :   0
//  0x00000001  :   1
//  0x00000002  :   2
//  ...
//  0x00010000  :   17
//  0x00020000  :   17
//  0x00040000  :   17
//  ...
//  0x80000000  :   32
//

#include "liquid.internal.h"

unsigned int msb_index(unsigned int _x)
{
    unsigned int bits;

#if defined __i386__ || defined __amd64__ || defined __x86_64__
    if (!_x) return 0;
    __asm volatile("bsrl %1,%0\n"
        : "=r" (bits)
        : "c" (_x)
    );
    return bits + 1;
#elif 0
    // slow method; look one bit at a time
    for (bits = 0; _x != 0 && bits < 32; _x >>= 1, ++bits)
        ;
    return bits;
#else
    // look for first non-zero byte
    unsigned int i, b;
    bits = 8*SIZEOF_UNSIGNED_INT;
    for (i=SIZEOF_UNSIGNED_INT*8; i>0; i-=8) {
        b = (_x >> (i-8)) & 0xFF;
        if ( b )
            return bits - leading_zeros[b];
        else
            bits -= 8;
    }
    return 0;

#endif
}


