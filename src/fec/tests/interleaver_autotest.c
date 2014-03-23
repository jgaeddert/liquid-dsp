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

#include <stdlib.h>

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTESTS: interleave/deinterleave
//
void interleaver_test_hard(unsigned int _n)
{
    unsigned int i;
    unsigned char x[_n];
    unsigned char y[_n];
    unsigned char z[_n];

    for (i=0; i<_n; i++)
        x[i] = rand() & 0xFF;

    // create interleaver object
    interleaver q = interleaver_create(_n);

    interleaver_encode(q,x,y);
    interleaver_decode(q,y,z);

    CONTEND_SAME_DATA(x, z, _n);

    // destroy interleaver object
    interleaver_destroy(q);
}

// 
// AUTOTESTS: interleave/deinterleave (soft)
//
void interleaver_test_soft(unsigned int _n)
{
    unsigned int i;
    unsigned char x[8*_n];
    unsigned char y[8*_n];
    unsigned char z[8*_n];

    for (i=0; i<8*_n; i++)
        x[i] = rand() & 0xFF;

    // create interleaver object
    interleaver q = interleaver_create(_n);

    interleaver_encode_soft(q,x,y);
    interleaver_decode_soft(q,y,z);

    CONTEND_SAME_DATA(x, z, 8*_n);
    
    // destroy interleaver object
    interleaver_destroy(q);
}

void autotest_interleaver_hard_8()      { interleaver_test_hard(8   ); }
void autotest_interleaver_hard_16()     { interleaver_test_hard(16  ); }
void autotest_interleaver_hard_64()     { interleaver_test_hard(64  ); }
void autotest_interleaver_hard_256()    { interleaver_test_hard(256 ); }

void autotest_interleaver_soft_8()      { interleaver_test_soft(8   ); }
void autotest_interleaver_soft_16()     { interleaver_test_soft(16  ); }
void autotest_interleaver_soft_64()     { interleaver_test_soft(64  ); }
void autotest_interleaver_soft_256()    { interleaver_test_soft(256 ); }

