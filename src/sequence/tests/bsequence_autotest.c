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

#include "autotest/autotest.h"
#include "liquid.h"

// 
// test initialization of binary sequence
//
void autotest_bsequence_init()
{
    // 1111 0000 1100 1010
    unsigned char v[2] = {0xf0, 0xca};

    // create and initialize sequence
    bsequence q = bsequence_create(16);
    bsequence_init(q,v);

    // run tests
    CONTEND_EQUALITY( bsequence_index(q,15), 1 );
    CONTEND_EQUALITY( bsequence_index(q,14), 1 );
    CONTEND_EQUALITY( bsequence_index(q,13), 1 );
    CONTEND_EQUALITY( bsequence_index(q,12), 1 );
    
    CONTEND_EQUALITY( bsequence_index(q,11), 0 );
    CONTEND_EQUALITY( bsequence_index(q,10), 0 );
    CONTEND_EQUALITY( bsequence_index(q, 9), 0 );
    CONTEND_EQUALITY( bsequence_index(q, 8), 0 );
    
    CONTEND_EQUALITY( bsequence_index(q, 7), 1 );
    CONTEND_EQUALITY( bsequence_index(q, 6), 1 );
    CONTEND_EQUALITY( bsequence_index(q, 5), 0 );
    CONTEND_EQUALITY( bsequence_index(q, 4), 0 );
    
    CONTEND_EQUALITY( bsequence_index(q, 3), 1 );
    CONTEND_EQUALITY( bsequence_index(q, 2), 0 );
    CONTEND_EQUALITY( bsequence_index(q, 1), 1 );
    CONTEND_EQUALITY( bsequence_index(q, 0), 0 );
    
    // clean up memory
    bsequence_destroy(q);
}

// 
// test correlation between to sequences
//
void autotest_bsequence_correlate()
{
    // v0   :   1111 0000 1100 1010
    // v1   :   1100 1011 0001 1110
    // sim  :   1100 0100 0010 1011 (7 similar bits)
    unsigned char v0[2] = {0xf0, 0xca};
    unsigned char v1[2] = {0xcb, 0x1e};

    // create and initialize sequences
    bsequence q0 = bsequence_create(16);
    bsequence q1 = bsequence_create(16);
    bsequence_init(q0,v0);
    bsequence_init(q1,v1);

    // run tests
    CONTEND_EQUALITY( bsequence_correlate(q0,q1), 7 );
    
    // clean up memory
    bsequence_destroy(q0);
    bsequence_destroy(q1);
}


// 
// test add operations on two sequences
//
void autotest_bsequence_add()
{
    // v0   :   1111 0000 1100 1010
    // v1   :   1100 1011 0001 1110
    // sum  :   0011 1011 1101 0100
    unsigned char v0[2] = {0xf0, 0xca};
    unsigned char v1[2] = {0xcb, 0x1e};

    // create and initialize sequences
    bsequence q0 = bsequence_create(16);
    bsequence q1 = bsequence_create(16);
    bsequence_init(q0,v0);
    bsequence_init(q1,v1);

    // create result sequence
    bsequence r = bsequence_create(16);
    bsequence_add(q0, q1, r);

    // run tests
    CONTEND_EQUALITY( bsequence_index(r,15), 0 );
    CONTEND_EQUALITY( bsequence_index(r,14), 0 );
    CONTEND_EQUALITY( bsequence_index(r,13), 1 );
    CONTEND_EQUALITY( bsequence_index(r,12), 1 );
    
    CONTEND_EQUALITY( bsequence_index(r,11), 1 );
    CONTEND_EQUALITY( bsequence_index(r,10), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 9), 1 );
    CONTEND_EQUALITY( bsequence_index(r, 8), 1 );
    
    CONTEND_EQUALITY( bsequence_index(r, 7), 1 );
    CONTEND_EQUALITY( bsequence_index(r, 6), 1 );
    CONTEND_EQUALITY( bsequence_index(r, 5), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 4), 1 );
    
    CONTEND_EQUALITY( bsequence_index(r, 3), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 2), 1 );
    CONTEND_EQUALITY( bsequence_index(r, 1), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 0), 0 );
    
    // clean up memory
    bsequence_destroy(q0);
    bsequence_destroy(q1);
    bsequence_destroy(r);
}

// 
// test multiply operations on two sequences
//
void autotest_bsequence_mul()
{
    // v0   :   1111 0000 1100 1010
    // v1   :   1100 1011 0001 1110
    // prod :   1100 0000 0000 1010
    unsigned char v0[2] = {0xf0, 0xca};
    unsigned char v1[2] = {0xcb, 0x1e};

    // create and initialize sequences
    bsequence q0 = bsequence_create(16);
    bsequence q1 = bsequence_create(16);
    bsequence_init(q0,v0);
    bsequence_init(q1,v1);

    // create result sequence
    bsequence r = bsequence_create(16);
    bsequence_mul(q0, q1, r);

    // run tests
    CONTEND_EQUALITY( bsequence_index(r,15), 1 );
    CONTEND_EQUALITY( bsequence_index(r,14), 1 );
    CONTEND_EQUALITY( bsequence_index(r,13), 0 );
    CONTEND_EQUALITY( bsequence_index(r,12), 0 );
    
    CONTEND_EQUALITY( bsequence_index(r,11), 0 );
    CONTEND_EQUALITY( bsequence_index(r,10), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 9), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 8), 0 );
    
    CONTEND_EQUALITY( bsequence_index(r, 7), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 6), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 5), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 4), 0 );
    
    CONTEND_EQUALITY( bsequence_index(r, 3), 1 );
    CONTEND_EQUALITY( bsequence_index(r, 2), 0 );
    CONTEND_EQUALITY( bsequence_index(r, 1), 1 );
    CONTEND_EQUALITY( bsequence_index(r, 0), 0 );
    
    // clean up memory
    bsequence_destroy(q0);
    bsequence_destroy(q1);
    bsequence_destroy(r);
}

// 
// test accumulation of binary sequence
//
void autotest_bsequence_accumulate()
{
    // 1111 0000 1100 1010 (8 total bits)
    unsigned char v[2] = {0xf0, 0xca};

    // create and initialize sequence
    bsequence q = bsequence_create(16);
    bsequence_init(q,v);

    // run tests
    CONTEND_EQUALITY( bsequence_accumulate(q), 8 );
    
    // clean up memory
    bsequence_destroy(q);
}


