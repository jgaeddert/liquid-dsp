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
// Bit-counting utilities
//
// NOTE: the following arrays are defined externally in the gentab
//       directory:
// const unsigned char liquid_c_ones[256];
// const unsigned char liquid_c_ones_mod2[256];
//

#include "liquid.internal.h"

// count the number of ones in an integer
unsigned int liquid_count_ones(unsigned int _x) {
#if SIZEOF_INT == 2
    return liquid_count_ones_uint16(_x);
#elif SIZEOF_INT == 4
    return liquid_count_ones_uint32(_x);
#else
    unsigned int i;
    unsigned int c=0;
    for (i=0; i<SIZEOF_INT; i++) {
        c += liquid_c_ones[ _x & 0xff ];
        _x >>= 8;
    }   
    return c;
#endif
}

// count the number of ones in an integer, modulo 2
unsigned int liquid_count_ones_mod2(unsigned int _x)
{
#if SIZEOF_INT == 2
    return liquid_count_ones_mod2_uint16(_x);
#elif SIZEOF_INT == 4
    return liquid_count_ones_mod2_uint32(_x);
#else
    unsigned int i;
    unsigned int c=0;
    for (i=0; i<SIZEOF_INT; i++) {
        c += liquid_c_ones_mod2[ _x & 0xff ];
        _x >>= 8;
    }   
    return c & 1;
#endif
}

// count the binary dot-product between two integers
unsigned int liquid_bdotprod(unsigned int _x,
                             unsigned int _y)
{
    unsigned int t = _x & _y;
#if SIZEOF_INT == 2
    return liquid_count_ones_mod2_uint16(t);
#elif SIZEOF_INT == 4
    return liquid_count_ones_mod2_uint32(t);
#else
    unsigned int c=0;
    for (i=0; i<SIZEOF_INT; i++) {
        c += liquid_c_ones_mod2[ t & 0xff ];
        t >>= 8;
    }   
    return c & 1;
#endif
}


// counts the number of different bits between two symbols
unsigned int count_bit_errors(unsigned int _s1,
                              unsigned int _s2)
{
    return liquid_count_ones(_s1^_s2);
}

// counts the number of different bits between two arrays of symbols
//  _msg0   :   original message [size: _n x 1]
//  _msg1   :   copy of original message [size: _n x 1]
//  _n      :   message size
unsigned int count_bit_errors_array(unsigned char * _msg0,
                                    unsigned char * _msg1,
                                    unsigned int _n)
{
    unsigned int num_bit_errors = 0;
    unsigned int i;
    for (i=0; i<_n; i++)
        num_bit_errors += liquid_c_ones[_msg0[i] ^ _msg1[i]];

    return num_bit_errors;
}


void liquid_print_bitstring(unsigned int _x,
                            unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        printf("%1u", (_x >> (_n-i-1)) & 1);
}

