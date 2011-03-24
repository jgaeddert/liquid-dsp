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

#ifndef __INTERLEAVER_AUTOTEST_H__
#define __INTERLEAVER_AUTOTEST_H__

#include <stdlib.h>

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTESTS: interleave/deinterleave
//
void intlv_test(unsigned int _n, int _type)
{
    unsigned int i;
    unsigned char x[_n];
    unsigned char y[_n];
    unsigned char z[_n];

    for (i=0; i<_n; i++)
        x[i] = rand() & 0xFF;

    interleaver q = interleaver_create(_n, _type);

    interleaver_encode(q,x,y);
    interleaver_decode(q,y,z);

    CONTEND_SAME_DATA(x, z, _n);
}

void autotest_interleaver_block_8()         { intlv_test(8,     LIQUID_INTERLEAVER_BLOCK); }
void autotest_interleaver_block_16()        { intlv_test(16,    LIQUID_INTERLEAVER_BLOCK); }
void autotest_interleaver_block_64()        { intlv_test(64,    LIQUID_INTERLEAVER_BLOCK); }
void autotest_interleaver_block_256()       { intlv_test(256,   LIQUID_INTERLEAVER_BLOCK); }

void autotest_interleaver_sequence_8()      { intlv_test(8,     LIQUID_INTERLEAVER_SEQUENCE); }
void autotest_interleaver_sequence_16()     { intlv_test(16,    LIQUID_INTERLEAVER_SEQUENCE); }
void autotest_interleaver_sequence_64()     { intlv_test(64,    LIQUID_INTERLEAVER_SEQUENCE); }
void autotest_interleaver_sequence_256()    { intlv_test(256,   LIQUID_INTERLEAVER_SEQUENCE); }

#endif 

