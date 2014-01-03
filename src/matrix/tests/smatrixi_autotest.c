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
#include "liquid.internal.h"

// 
// AUTOTESTS: basic sparse matrix functionality
//

// test sparse integer vector multiplication
void autotest_smatrixi_vmul()
{
    // A = [
    //  0 0 0 0 4
    //  0 0 0 0 0
    //  0 0 0 3 0
    //  2 0 0 0 1

    // create sparse matrix and set values
    smatrixi A = smatrixi_create(4, 5);
    smatrixi_set(A, 0,4, 4);
    smatrixi_set(A, 2,3, 3);
    smatrixi_set(A, 3,0, 2);
    smatrixi_set(A, 3,4, 0);
    smatrixi_set(A, 3,4, 1);

    // initialize input vector
    short int x[5] = {7, 1, 5, 2, 2};

    short int y_test[4] = {8, 0, 6, 16};
    short int y[4];

    // multiply and run test
    smatrixi_vmul(A,x,y);

    // check values
    CONTEND_EQUALITY( y[0], y_test[0] );
    CONTEND_EQUALITY( y[1], y_test[1] );
    CONTEND_EQUALITY( y[2], y_test[2] );
    CONTEND_EQUALITY( y[3], y_test[3] );

    smatrixi_destroy(A);
}

// test sparse integer matrix multiplication
void autotest_smatrixi_mul()
{
    // intialize matrices
    smatrixi a = smatrixi_create(4, 5);
    smatrixi b = smatrixi_create(5, 3);
    smatrixi c = smatrixi_create(4, 3);

    // initialize 'a'
    // 0 0 0 0 4
    // 0 0 0 0 0
    // 0 0 0 3 0
    // 2 0 0 0 1
    smatrixi_set(a, 0,4, 4);
    smatrixi_set(a, 2,3, 3);
    smatrixi_set(a, 3,0, 2);
    smatrixi_set(a, 3,4, 0);
    smatrixi_set(a, 3,4, 1);

    // initialize 'b'
    // 7 6 0
    // 0 0 0
    // 0 0 0
    // 0 5 0
    // 2 0 0
    smatrixi_set(b, 0,0, 7);
    smatrixi_set(b, 0,1, 6);
    smatrixi_set(b, 3,1, 5);
    smatrixi_set(b, 4,0, 2);

    // compute 'c'
    //  8   0   0
    //  0   0   0
    //  0  15   0
    // 16  12   0
    smatrixi_mul(a,b,c);

    short int c_test[12] = {
         8,   0,   0,
         0,   0,   0,
         0,  15,   0,
        16,  12,   0};

    // check values
    unsigned int i;
    unsigned int j;
    for (i=0; i<4; i++) {
        for (j=0; j<3; j++) {
            CONTEND_EQUALITY(smatrixi_get(c,i,j),
                             matrixf_access(c_test,4,3,i,j))
        }
    }

    smatrixi_destroy(a);
    smatrixi_destroy(b);
    smatrixi_destroy(c);
}
