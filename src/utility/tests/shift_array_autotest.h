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

#include <string.h>
#include "autotest/autotest.h"
#include "liquid.internal.h"


//
// AUTOTEST : rmemmove
//
void autotest_rmemmove() {
    // input        : 1000 0001 1110 1111 0101 1111 1010 1010 .... ....
    // output [0]   : 1000 0001 1110 1111 0101 1111 1010 1010 0000 0000
    // output [1]   : 0100 0000 1111 0111 1010 1111 1101 0101 0000 0000
    // output [2]   : 0010 0000 0111 1011 1101 0111 1110 1010 1000 0000
    // output [3]   : 0001 0000 0011 1101 1110 1011 1111 0101 0100 0000
    // output [4]   : 0000 1000 0001 1110 1111 0101 1111 1010 1010 0000
    // output [5]   : 0000 0100 0000 1111 0111 1010 1111 1101 0101 0000
    // output [6]   : 0000 0010 0000 0111 1011 1101 0111 1110 1010 1000
    // output [7]   : 0000 0001 0000 0011 1101 1110 1011 1111 0101 0100
    unsigned char input[4] = {0x81, 0xEF, 0x5F, 0xAA};
    
    unsigned char output_test_0[5] = {0x81, 0xEF, 0x5F, 0xAA, 0x00};
    unsigned char output_test_1[5] = {0x40, 0xF7, 0xAF, 0xD5, 0x00};
    unsigned char output_test_2[5] = {0x20, 0x7B, 0xD7, 0xEA, 0x80};
    unsigned char output_test_3[5] = {0x10, 0x3D, 0xEB, 0xF5, 0x40};
    unsigned char output_test_4[5] = {0x08, 0x1E, 0xF5, 0xFA, 0xA0};
    unsigned char output_test_5[5] = {0x04, 0x0F, 0x7A, 0xFD, 0x50};
    unsigned char output_test_6[5] = {0x02, 0x07, 0xBD, 0x7E, 0xA8};
    unsigned char output_test_7[5] = {0x01, 0x03, 0xDE, 0xBF, 0x54};

    unsigned char output[5];
    
    // 
    // run tests
    //

    liquid_rmemmove(output, input, 4, 0);
    CONTEND_SAME_DATA( output, output_test_0, 5 );

    liquid_rmemmove(output, input, 4, 1);
    CONTEND_SAME_DATA( output, output_test_1, 5 );

    liquid_rmemmove(output, input, 4, 2);
    CONTEND_SAME_DATA( output, output_test_2, 5 );

    liquid_rmemmove(output, input, 4, 3);
    CONTEND_SAME_DATA( output, output_test_3, 5 );

    liquid_rmemmove(output, input, 4, 4);
    CONTEND_SAME_DATA( output, output_test_4, 5 );

    liquid_rmemmove(output, input, 4, 5);
    CONTEND_SAME_DATA( output, output_test_5, 5 );

    liquid_rmemmove(output, input, 4, 6);
    CONTEND_SAME_DATA( output, output_test_6, 5 );

    liquid_rmemmove(output, input, 4, 7);
    CONTEND_SAME_DATA( output, output_test_7, 5 );
}

//
// AUTOTEST : lmemmove
//
void autotest_lmemmove() {
    // input        : .... .... 1000 0001 1110 1111 0101 1111 1010 1010
    // output [0]   : 0000 0000 1000 0001 1110 1111 0101 1111 1010 1010
    // output [1]   : 0000 0001 0000 0011 1101 1110 1011 1111 0101 0100
    // output [2]   : 0000 0010 0000 0111 1011 1101 0111 1110 1010 1000
    // output [3]   : 0000 0100 0000 1111 0111 1010 1111 1101 0101 0000
    // output [4]   : 0000 1000 0001 1110 1111 0101 1111 1010 1010 0000
    // output [5]   : 0001 0000 0011 1101 1110 1011 1111 0101 0100 0000
    // output [6]   : 0010 0000 0111 1011 1101 0111 1110 1010 1000 0000
    // output [7]   : 0100 0000 1111 0111 1010 1111 1101 0101 0000 0000
    unsigned char input[4] = {0x81, 0xEF, 0x5F, 0xAA};
    
    unsigned char output_test_0[5] = {0x00, 0x81, 0xEF, 0x5F, 0xAA};
    unsigned char output_test_1[5] = {0x01, 0x03, 0xDE, 0xBF, 0x54};
    unsigned char output_test_2[5] = {0x02, 0x07, 0xBD, 0x7E, 0xA8};
    unsigned char output_test_3[5] = {0x04, 0x0F, 0x7A, 0xFD, 0x50};
    unsigned char output_test_4[5] = {0x08, 0x1E, 0xF5, 0xFA, 0xA0};
    unsigned char output_test_5[5] = {0x10, 0x3D, 0xEB, 0xF5, 0x40};
    unsigned char output_test_6[5] = {0x20, 0x7B, 0xD7, 0xEA, 0x80};
    unsigned char output_test_7[5] = {0x40, 0xF7, 0xAF, 0xD5, 0x00};

    unsigned char output[5];
    
    // 
    // run tests
    //

    liquid_lmemmove(output, input, 4, 0);
    CONTEND_SAME_DATA( output, output_test_0, 5 );

    liquid_lmemmove(output, input, 4, 1);
    CONTEND_SAME_DATA( output, output_test_1, 5 );

    liquid_lmemmove(output, input, 4, 2);
    CONTEND_SAME_DATA( output, output_test_2, 5 );

    liquid_lmemmove(output, input, 4, 3);
    CONTEND_SAME_DATA( output, output_test_3, 5 );

    liquid_lmemmove(output, input, 4, 4);
    CONTEND_SAME_DATA( output, output_test_4, 5 );

    liquid_lmemmove(output, input, 4, 5);
    CONTEND_SAME_DATA( output, output_test_5, 5 );

    liquid_lmemmove(output, input, 4, 6);
    CONTEND_SAME_DATA( output, output_test_6, 5 );

    liquid_lmemmove(output, input, 4, 7);
    CONTEND_SAME_DATA( output, output_test_7, 5 );
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
    // output3[2]   : 1110 1111 0101 1111 1010 1010 1000 0001
    // output4[2]   : 1000 0001 1110 1111 0101 1111 1010 1010
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

