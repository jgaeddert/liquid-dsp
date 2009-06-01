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

#ifndef __PACK_BYTES_AUTOTEST_H__
#define __PACK_BYTES_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.internal.h"

void autotest_pack_bytes_01() {
    unsigned char output[8];
    unsigned int N;
    
    unsigned char input[36] = {
        0, 0, 0, 0, 0, 0, 0, 0, // 0:   0000 0000
        1, 1, 1, 1, 1, 1, 1, 1, // 255: 1111 1111
        0, 0, 0, 0, 1, 1, 1, 1, // 15:  0000 1111
        1, 0, 1, 0, 1, 0, 1, 0  // 170: 1010 1010
    };

    // Test packing entire array
    unsigned char output_test_01[4] = {0x00, 0xFF, 0x0F, 0xAA};
    pack_bytes( input, 32, output, 8, &N );
    CONTEND_EQUALITY( N, 4 );
    CONTEND_SAME_DATA( output, output_test_01, 4 );

    // Test packing only 28 elements
    unsigned char output_test_02[4] = {0x00, 0xFF, 0x0F, 0x0A};
    pack_bytes( input, 28, output, 8, &N );
    CONTEND_EQUALITY( N, 4 );
    CONTEND_SAME_DATA( output, output_test_02, 4 );
    
    // Test packing only 25 elements
    unsigned char output_test_03[4] = {0x00, 0xFF, 0x0F, 0x01};
    pack_bytes( input, 25, output, 8, &N );
    CONTEND_EQUALITY( N, 4 );
    CONTEND_SAME_DATA( output, output_test_03, 4 );

    // Test packing only 24 elements (3 bytes)
    unsigned char output_test_04[3] = {0x00, 0xFF, 0x0F};
    pack_bytes( input, 24, output, 8, &N );
    CONTEND_EQUALITY( N, 3 );
    CONTEND_SAME_DATA( output, output_test_04, 3 );
}


//
// unpack_bytes
//

void autotest_unpack_bytes_01() {
    unsigned char input[5] = {0x00, 0x01, 0xFF, 0x0F, 0xAA};
    
    unsigned char output[64];
    unsigned int N;

    unsigned char output_test[40] = {
        0, 0, 0, 0, 0, 0, 0, 0, // 0:   0000 0000
        0, 0, 0, 0, 0, 0, 0, 1, // 1:   0000 0001
        1, 1, 1, 1, 1, 1, 1, 1, // 255: 1111 1111
        0, 0, 0, 0, 1, 1, 1, 1, // 15:  0000 1111
        1, 0, 1, 0, 1, 0, 1, 0  // 170: 1010 1010
    };
    
    // Test packing entire array
    unpack_bytes( input, 4, output, 40, &N );
    CONTEND_EQUALITY( N, 32 );
    CONTEND_SAME_DATA( output, output_test, 32 );
}


//
// repack_bytes
//

void autotest_repack_bytes_01() {
    unsigned char input[] = {
        0x07,   // 111
        0x00,   // 000
        0x06,   // 110
        0x07    // 111
    };

    unsigned char output_test[] = {
        0x03,   // 11
        0x02,   // 10
        0x00,   // 00
        0x03,   // 11
        0x01,   // 01
        0x03    // 11
    };

    unsigned char output[8];
    unsigned int N;

    repack_bytes( input, 3, 4, output, 2, 8, &N );

    CONTEND_EQUALITY( N, 6 );
    CONTEND_SAME_DATA( output, output_test, 6 );
}

void autotest_repack_bytes_02() {
    unsigned char input[] = {
        0x01,   // 00001
        0x02,   // 00010
        0x04    // 00100
    };

    unsigned char output_test[] = {
        0x00,   // 000
        0x02,   // 010
        0x01,   // 001
        0x00,   // 000
        0x04    // 100
    };

    unsigned char output[8];
    unsigned int N;

    repack_bytes( input, 5, 3, output, 3, 8, &N );

    CONTEND_EQUALITY( N, 5 );
    CONTEND_SAME_DATA( output, output_test, 5 );
}

void autotest_repack_bytes_03() {
    unsigned char input[] = {
        0x00,   // 000
        0x02,   // 010
        0x01,   // 001
        0x00,   // 000
        0x04    // 100
    };

    unsigned char output_test[] = {
        0x01,   // 00001
        0x02,   // 00010
        0x04    // 00100
    };

    unsigned char output[8];
    unsigned int N;

    repack_bytes( input, 3, 5, output, 5, 8, &N );

    CONTEND_EQUALITY( N, 3 );
    CONTEND_SAME_DATA( output, output_test, 3 );
}


#endif // __PACK_BYTES_AUTOTEST_H__

