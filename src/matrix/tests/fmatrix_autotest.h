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

#ifndef __LIQUID_FMATRIX_AUTOTEST_H__
#define __LIQUID_FMATRIX_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: Test fmatrix add
//
void autotest_fmatrix_add() {

    float x[6] = {
        1, 2, 3,
        4, 5, 6  };

    float y[6] = {
        0, 1, 2,
        3, 4, 5  };

    float z[6];
    float ztest[6] = {
        1, 3, 5,
        7, 9, 11 };

    fmatrix_add(2,3,x,y,z);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: Test fmatrix ops
//
void autotest_fmatrix_ops() {

    float x[6] = {
        1, 2, 3,
        4, 5, 6  };

    float y[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9  };

    float z[6];
    float ztest[6] = {
        30, 36, 42,
        66, 81, 96   };

    fmatrix_mul(x,2,3, y,3,3, z,2,3);
    if (liquid_autotest_verbose)
        fmatrix_print(z,2,3);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: 
//
void autotest_fmatrix_mul() {
    float x[15]= {
       4,  -3,  -1,
       1,   1,   0,
       2,  -1,   0,
       1,   1,  -2,
      -5,   1,  -3};

    float y[15] = {
       0,  -3,   1,  -5,   2,
       5,  -1,   4,   3,  -3,
       2,  -6,  -1,   1,  -3};

    float z0[25];
    float z0_test[25] = {
      -17,   -3,   -7,  -30,   20,
        5,   -4,    5,   -2,   -1,
       -5,   -5,   -2,  -13,    7,
        1,    8,    7,   -4,    5,
       -1,   32,    2,   25,   -4};

    float z1[9];
    float z1_test[9] = {
      -16,   -7,    4,
       45,  -20,   -2,
       16,  -13,    5};

    fmatrix_mul(x,5,3,y,3,5,z0,5,5);
    if (liquid_autotest_verbose)
        fmatrix_print(z0,5,5);
    CONTEND_SAME_DATA(z0,z0_test,5*5*sizeof(float));

    fmatrix_mul(y,3,5,x,5,3,z1,3,3);
    if (liquid_autotest_verbose)
        fmatrix_print(z1,3,3);
    CONTEND_SAME_DATA(z1,z1_test,3*3*sizeof(float));

}


// 
// AUTOTEST: identity
//
void autotest_fmatrix_eye() {
    float x[16]= {
       4,  -3,  -1,  3,
       1,   1,   0,  2,
      -1,   0,   1,  1,
      -2,  -5,   1,  -3};

    float I4_test[16] = {
        1,  0,  0,  0,
        0,  1,  0,  0,
        0,  0,  1,  0,
        0,  0,  0,  1};

    float y[16];
    float z[16];

    // generate identity matrix
    fmatrix_eye(y,4);
    CONTEND_SAME_DATA(y, I4_test, 16*sizeof(float));

    // multiply with input
    fmatrix_mul(x, 4, 4,
                y, 4, 4,
                z, 4, 4);
    CONTEND_SAME_DATA(x, z, 16*sizeof(float));
}

// 
// AUTOTEST: L/U decomp (Crout)
//
void autotest_fmatrix_ludecomp_crout()
{
    float tol = 1e-6f;  // error tolerance

    float A[16]= {
       4,  -3,  -1,  3,
       1,   1,   0,  2,
      -1,   0,   1,  1,
      -2,  -5,   1,  -3};

    float L[16];
    float U[16];
    float P[16];

    float LU_test[16];

    // run decomposition
    fmatrix_ludecomp_crout(A,4,4,L,U,P);

    if (liquid_autotest_verbose) {
        printf("L :\n");
        fmatrix_print(L,4,4);
        printf("U :\n");
        fmatrix_print(U,4,4);
    }

    unsigned int r,c;
    for (r=0; r<4; r++) {
        for (c=0; c<4; c++) {
            if (r < c) {
                CONTEND_DELTA( matrix_access(L,4,4,r,c), 0.0f, tol );
            } else if (r==c) {
                CONTEND_DELTA( matrix_access(U,4,4,r,c), 1.0f, tol );
            } else {
                CONTEND_DELTA( matrix_access(U,4,4,r,c), 0.0f, tol );
            }
        }
    }

    // multiply LU
    fmatrix_mul(L,       4, 4,
                U,       4, 4,
                LU_test, 4, 4);

    unsigned int i;
    for (i=0; i<16; i++)
        CONTEND_DELTA( LU_test[i], A[i], tol );
}

// 
// AUTOTEST: L/U decomp (Doolittle)
//
void autotest_fmatrix_ludecomp_doolittle()
{
    float tol = 1e-6f;  // error tolerance

    float A[16]= {
       4,  -3,  -1,  3,
       1,   1,   0,  2,
      -1,   0,   1,  1,
      -2,  -5,   1,  -3};

    float L[16];
    float U[16];
    float P[16];

    float LU_test[16];

    // run decomposition
    fmatrix_ludecomp_doolittle(A,4,4,L,U,P);

    if (liquid_autotest_verbose) {
        printf("L :\n");
        fmatrix_print(L,4,4);
        printf("U :\n");
        fmatrix_print(U,4,4);
    }

    unsigned int r,c;
    for (r=0; r<4; r++) {
        for (c=0; c<4; c++) {
            if (r < c) {
                CONTEND_DELTA( matrix_access(L,4,4,r,c), 0.0f, tol );
            } else if (r==c) {
                CONTEND_DELTA( matrix_access(L,4,4,r,c), 1.0f, tol );
            } else {
                CONTEND_DELTA( matrix_access(U,4,4,r,c), 0.0f, tol );
            }
        }
    }

    // multiply LU
    fmatrix_mul(L,       4, 4,
                U,       4, 4,
                LU_test, 4, 4);

    unsigned int i;
    for (i=0; i<16; i++)
        CONTEND_DELTA( LU_test[i], A[i], tol );
}

#endif // __LIQUID_FMATRIX_AUTOTEST_H__
