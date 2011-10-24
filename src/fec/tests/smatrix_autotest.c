/*
 * Copyright (c) 2011, Joseph Gaeddert
 * Copyright (c) 2011, Virginia Polytechnic Institute & State University
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
#include "liquid.internal.h"

// 
// AUTOTESTS: basic encode/decode functionality
//

// test sparse binary matrix methods
void autotest_fec_smatrix()
{
    // A = [
    //  1   0   0   0   0   0   0   0   0   0   0   0
    //  0   0   0   1   0   0   0   0   0   0   0   0
    //  1   0   0   0   0   0   0   0   1   1   0   0
    //  0   0   1   0   0   0   1   1   0   0   0   0
    //  0   0   0   0   0   0   0   0   0   0   0   0
    //  0   0   0   0   0   0   1   0   1   0   1   0
    //  1   0   1   0   0   0   0   0   0   0   1   1
    //  0   0   1   0   0   1   1   0   0   0   0   0]
    //
    // x = [   1   1   0   0   1   0   0   1   1   1   0   1 ]
    // y = [   1   0   1   1   0   1   0   0 ]
    //
    
    // create sparse matrix and set values
    smatrix A = smatrix_create(8,12);
    smatrix_set(A,0,0);
    smatrix_set(A,2,0);
    smatrix_set(A,6,0);
    smatrix_set(A,3,2);
    smatrix_set(A,6,2);
    smatrix_set(A,7,2);
    smatrix_set(A,1,3);
    smatrix_set(A,7,5);
    smatrix_set(A,3,6);
    smatrix_set(A,5,6);
    smatrix_set(A,7,6);
    smatrix_set(A,3,7);
    smatrix_set(A,2,8);
    smatrix_set(A,5,8);
    smatrix_set(A,2,9);
    smatrix_set(A,5,10);
    smatrix_set(A,6,10);
    smatrix_set(A,6,11);

    // generate vectors
    unsigned char x[12]     = {1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1};
    unsigned char y_test[8] = {1, 0, 1, 1, 0, 1, 0, 0};
    unsigned char y[8];

    // multiply and run test
    smatrix_vmul(A,x,y);

    CONTEND_EQUALITY( y[0], y_test[0] );
    CONTEND_EQUALITY( y[1], y_test[1] );
    CONTEND_EQUALITY( y[2], y_test[2] );
    CONTEND_EQUALITY( y[3], y_test[3] );
    CONTEND_EQUALITY( y[4], y_test[4] );
    CONTEND_EQUALITY( y[5], y_test[5] );
    CONTEND_EQUALITY( y[6], y_test[6] );
    CONTEND_EQUALITY( y[7], y_test[7] );

    // print results (verbose)
    if (liquid_autotest_verbose) {
        printf("\ncompact form:\n");
        smatrix_print(A);

        printf("\nexpanded form:\n");
        smatrix_print_expanded(A);

        unsigned int i;
        unsigned int j;

        printf("x = [");
        for (j=0; j<12; j++) printf("%2u", x[j]);
        printf(" ];\n");

        printf("y      = [");
        for (i=0; i<8; i++) printf("%2u", y[i]);
        printf(" ];\n");

        printf("y_test = [");
        for (i=0; i<8; i++) printf("%2u", y_test[i]);
        printf(" ];\n");
    }

    // destroy matrix object
    smatrix_destroy(A);
}

// test sparse binary matrix multiplication
void autotest_fec_smatrix_mul()
{
    // a: [8 x 12]
    unsigned char a_test[96] = {
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0};

    // b: [12 x 5]
    unsigned char b_test[60] = {
        1, 1, 0, 0, 0,
        0, 0, 0, 0, 1,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 1,
        0, 0, 0, 1, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 0, 0,
        0, 1, 0, 0, 1,
        1, 0, 0, 1, 0,
        0, 1, 0, 0, 0};

    // output: [8 x 5]
    unsigned char c_test[40] = {
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 1,
        0, 0, 0, 1, 0};

    smatrix a = smatrix_create_array(a_test,  8,12);
    smatrix b = smatrix_create_array(b_test, 12, 5);
    smatrix c = smatrix_create(8, 5);

    // compute output
    smatrix_mul(a,b,c);

    // print results (verbose)
    if (liquid_autotest_verbose) {
        printf("a:\n"); smatrix_print_expanded(a);
        printf("b:\n"); smatrix_print_expanded(b);
        printf("c:\n"); smatrix_print_expanded(c);
    }

    unsigned int i;
    unsigned int j;
    for (i=0; i<8; i++) {
        for (j=0; j<5; j++) {
            CONTEND_EQUALITY( smatrix_get(c,i,j), c_test[i*5+j]);
        }
    }
    
    // destroy objects
    smatrix_destroy(a);
    smatrix_destroy(b);
    smatrix_destroy(c);
}

