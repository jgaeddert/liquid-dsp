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

// test sparse floating-point vector multiplication
void autotest_smatrixf_vmul()
{
    float tol = 1e-6f;

    // A = [
    //  0 0 0 0 4
    //  0 0 0 0 0
    //  0 0 0 3 0
    //  2 0 0 0 1

    // create sparse matrix and set values
    smatrixf A = smatrixf_create(4, 5);
    smatrixf_set(A, 0,4, 4);
    smatrixf_set(A, 2,3, 3);
    smatrixf_set(A, 3,0, 2);
    smatrixf_set(A, 3,4, 0);
    smatrixf_set(A, 3,4, 1);

    // initialize input vector
    float x[5] = {7, 1, 5, 2, 2};

    float y_test[4] = {8, 0, 6, 16};
    float y[4];

    // multiply and run test
    smatrixf_vmul(A,x,y);

    // check values
    CONTEND_DELTA( y[0], y_test[0], tol );
    CONTEND_DELTA( y[1], y_test[1], tol );
    CONTEND_DELTA( y[2], y_test[2], tol );
    CONTEND_DELTA( y[3], y_test[3], tol );

    smatrixf_destroy(A);
}
// test sparse floating-point matrix multiplication
void autotest_smatrixf_mul()
{
    float tol = 1e-6f;

    // intialize matrices
    smatrixf a = smatrixf_create(4, 5);
    smatrixf b = smatrixf_create(5, 3);
    smatrixf c = smatrixf_create(4, 3);

    // initialize 'a'
    // 0 0 0 0 4
    // 0 0 0 0 0
    // 0 0 0 3 0
    // 2 0 0 0 1
    smatrixf_set(a, 0,4, 4);
    smatrixf_set(a, 2,3, 3);
    smatrixf_set(a, 3,0, 2);
    smatrixf_set(a, 3,4, 0);
    smatrixf_set(a, 3,4, 1);

    // initialize 'b'
    // 7 6 0
    // 0 0 0
    // 0 0 0
    // 0 5 0
    // 2 0 0
    smatrixf_set(b, 0,0, 7);
    smatrixf_set(b, 0,1, 6);
    smatrixf_set(b, 3,1, 5);
    smatrixf_set(b, 4,0, 2);

    // compute 'c'
    //  8   0   0
    //  0   0   0
    //  0  15   0
    // 16  12   0
    smatrixf_mul(a,b,c);

    float c_test[12] = {
         8,   0,   0,
         0,   0,   0,
         0,  15,   0,
        16,  12,   0};

    // check values
    unsigned int i;
    unsigned int j;
    for (i=0; i<4; i++) {
        for (j=0; j<3; j++) {
            CONTEND_DELTA(smatrixf_get(c,i,j),
                          matrixf_access(c_test,4,3,i,j),
                          tol);
        }
    }

    smatrixf_destroy(a);
    smatrixf_destroy(b);
    smatrixf_destroy(c);
}
