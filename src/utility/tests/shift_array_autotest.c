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

#include <string.h>
#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST : lshift
//
void autotest_lshift() {
    // input        : 1000 0001 1110 1111 0101 1111 1010 1010
    // output [0]   : 1000 0001 1110 1111 0101 1111 1010 1010
    // output [1]   : 1110 1111 0101 1111 1010 1010 0000 0000
    // output [2]   : 0101 1111 1010 1010 0000 0000 0000 0000
    // output [3]   : 1010 1010 0000 0000 0000 0000 0000 0000
    // output [4]   : 0000 0000 0000 0000 0000 0000 0000 0000
    unsigned char input[4] = {0x81, 0xEF, 0x5F, 0xAA};
    
    unsigned char output_test_0[4] = {0x81, 0xEF, 0x5F, 0xAA};
    unsigned char output_test_1[4] = {0xEF, 0x5F, 0xAA, 0x00};
    unsigned char output_test_2[4] = {0x5F, 0xAA, 0x00, 0x00};
    unsigned char output_test_3[4] = {0xAA, 0x00, 0x00, 0x00};
    unsigned char output_test_4[4] = {0x00, 0x00, 0x00, 0x00};

    unsigned char output[4];

    // 
    // run tests
    //
    unsigned int i;
    for (i=0; i<5; i++) {
        memmove(output, input, 4);
        liquid_lshift( output, 4, i);
        switch (i) {
        case 0: CONTEND_SAME_DATA( output, output_test_0, 4 ); break;
        case 1: CONTEND_SAME_DATA( output, output_test_1, 4 ); break;
        case 2: CONTEND_SAME_DATA( output, output_test_2, 4 ); break;
        case 3: CONTEND_SAME_DATA( output, output_test_3, 4 ); break;
        case 4: CONTEND_SAME_DATA( output, output_test_4, 4 ); break;
        default:;
        }
    }
}


//
// AUTOTEST : rshift
//
void autotest_rshift() {
    // input        : 1000 0001 1110 1111 0101 1111 1010 1010
    // output [0]   : 1000 0001 1110 1111 0101 1111 1010 1010
    // output [1]   : 0000 0000 1000 0001 1110 1111 0101 1111
    // output [2]   : 0000 0000 0000 0000 1000 0001 1110 1111
    // output [3]   : 0000 0000 0000 0000 0000 0000 1000 0001
    // output [4]   : 0000 0000 0000 0000 0000 0000 0000 0000
    unsigned char input[4] = {0x81, 0xEF, 0x5F, 0xAA};
    
    unsigned char output_test_0[4] = {0x81, 0xEF, 0x5F, 0xAA};
    unsigned char output_test_1[4] = {0x00, 0x81, 0xEF, 0x5F};
    unsigned char output_test_2[4] = {0x00, 0x00, 0x81, 0xEF};
    unsigned char output_test_3[4] = {0x00, 0x00, 0x00, 0x81};
    unsigned char output_test_4[4] = {0x00, 0x00, 0x00, 0x00};

    unsigned char output[4];

    // 
    // run tests
    //
    unsigned int i;
    for (i=0; i<5; i++) {
        memmove(output, input, 4);
        liquid_rshift( output, 4, i);
        switch (i) {
        case 0: CONTEND_SAME_DATA( output, output_test_0, 4 ); break;
        case 1: CONTEND_SAME_DATA( output, output_test_1, 4 ); break;
        case 2: CONTEND_SAME_DATA( output, output_test_2, 4 ); break;
        case 3: CONTEND_SAME_DATA( output, output_test_3, 4 ); break;
        case 4: CONTEND_SAME_DATA( output, output_test_4, 4 ); break;
        default:;
        }
    }
}


//
// AUTOTEST : lcircshift
//
void autotest_lcircshift() {
    // input        : 1000 0001 1110 1111 0101 1111 1010 1010
    // output [0]   : 1000 0001 1110 1111 0101 1111 1010 1010
    // output [1]   : 1110 1111 0101 1111 1010 1010 1000 0001
    // output [2]   : 0101 1111 1010 1010 1000 0001 1110 1111
    // output [3]   : 1010 1010 1000 0001 1110 1111 0101 1111
    // output [4]   : 1000 0001 1110 1111 0101 1111 1010 1010
    unsigned char input[4] = {0x81, 0xEF, 0x5F, 0xAA};
    
    unsigned char output_test_0[4] = {0x81, 0xEF, 0x5F, 0xAA};
    unsigned char output_test_1[4] = {0xEF, 0x5F, 0xAA, 0x81};
    unsigned char output_test_2[4] = {0x5F, 0xAA, 0x81, 0xEF};
    unsigned char output_test_3[4] = {0xAA, 0x81, 0xEF, 0x5F};
    unsigned char output_test_4[4] = {0x81, 0xEF, 0x5F, 0xAA};

    unsigned char output[4];

    // 
    // run tests
    //
    unsigned int i;
    for (i=0; i<5; i++) {
        memmove(output, input, 4);
        liquid_lcircshift( output, 4, i);
        switch (i) {
        case 0: CONTEND_SAME_DATA( output, output_test_0, 4 ); break;
        case 1: CONTEND_SAME_DATA( output, output_test_1, 4 ); break;
        case 2: CONTEND_SAME_DATA( output, output_test_2, 4 ); break;
        case 3: CONTEND_SAME_DATA( output, output_test_3, 4 ); break;
        case 4: CONTEND_SAME_DATA( output, output_test_4, 4 ); break;
        default:;
        }
    }
}


//
// AUTOTEST : rcircshift
//
void autotest_rcircshift() {
    // input        : 1000 0001 1110 1111 0101 1111 1010 1010
    // output [0]   : 1000 0001 1110 1111 0101 1111 1010 1010
    // output [1]   : 1010 1010 1000 0001 1110 1111 0101 1111
    // output [2]   : 0101 1111 1010 1010 1000 0001 1110 1111
    // output [3]   : 1110 1111 0101 1111 1010 1010 1000 0001
    // output [4]   : 1000 0001 1110 1111 0101 1111 1010 1010
    unsigned char input[4] = {0x81, 0xEF, 0x5F, 0xAA};
    
    unsigned char output_test_0[4] = {0x81, 0xEF, 0x5F, 0xAA};
    unsigned char output_test_1[4] = {0xAA, 0x81, 0xEF, 0x5F};
    unsigned char output_test_2[4] = {0x5F, 0xAA, 0x81, 0xEF};
    unsigned char output_test_3[4] = {0xEF, 0x5F, 0xAA, 0x81};
    unsigned char output_test_4[4] = {0x81, 0xEF, 0x5F, 0xAA};

    unsigned char output[4];

    // 
    // run tests
    //
    unsigned int i;
    for (i=0; i<5; i++) {
        memmove(output, input, 4);
        liquid_rcircshift( output, 4, i);
        switch (i) {
        case 0: CONTEND_SAME_DATA( output, output_test_0, 4 ); break;
        case 1: CONTEND_SAME_DATA( output, output_test_1, 4 ); break;
        case 2: CONTEND_SAME_DATA( output, output_test_2, 4 ); break;
        case 3: CONTEND_SAME_DATA( output, output_test_3, 4 ); break;
        case 4: CONTEND_SAME_DATA( output, output_test_4, 4 ); break;
        default:;
        }
    }
}

