/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
#include "liquid.h"

// 
// AUTOTEST: Test matrixf add
//
void autotest_matrixf_add() {

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

    matrixf_add(x,y,z,2,3);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: Test matrixf ops
//
void autotest_matrixf_ops() {

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

    matrixf_mul(x,2,3, y,3,3, z,2,3);
    if (liquid_autotest_verbose)
        matrixf_print(z,2,3);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: 
//
void autotest_matrixf_mul() {
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

    matrixf_mul(x,5,3,y,3,5,z0,5,5);
    if (liquid_autotest_verbose)
        matrixf_print(z0,5,5);
    CONTEND_SAME_DATA(z0,z0_test,5*5*sizeof(float));

    matrixf_mul(y,3,5,x,5,3,z1,3,3);
    if (liquid_autotest_verbose)
        matrixf_print(z1,3,3);
    CONTEND_SAME_DATA(z1,z1_test,3*3*sizeof(float));

}

// 
// AUTOTEST: matrix_aug
//
void autotest_matrixf_aug() {
    float x[12]= {
      -17,   -3,   -7,
       -5,   -5,   -2,
        1,    8,    7,
       -1,   32,    2};

    float y[8] = {
      -30,   20,
      -13,    7,
       -4,    5,
       25,   -4};

    float z0[20];
    float z0_test[20] = {
      -17,   -3,   -7,  -30,   20,
       -5,   -5,   -2,  -13,    7,
        1,    8,    7,   -4,    5,
       -1,   32,    2,   25,   -4};

    matrixf_aug(x,4,3,y,4,2,z0,4,5);
    if (liquid_autotest_verbose)
        matrixf_print(z0,4,5);
    CONTEND_SAME_DATA(z0,z0_test,4*5*sizeof(float));
}


// 
// AUTOTEST: identity
//
void autotest_matrixf_eye() {
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
    matrixf_eye(y,4);
    CONTEND_SAME_DATA(y, I4_test, 16*sizeof(float));

    // multiply with input
    matrixf_mul(x, 4, 4,
                y, 4, 4,
                z, 4, 4);
    CONTEND_SAME_DATA(x, z, 16*sizeof(float));
}

// 
// AUTOTEST: L/U decomp (Crout)
//
void autotest_matrixf_ludecomp_crout()
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
    matrixf_ludecomp_crout(A,4,4,L,U,P);

    if (liquid_autotest_verbose) {
        printf("L :\n");
        matrixf_print(L,4,4);
        printf("U :\n");
        matrixf_print(U,4,4);
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
    matrixf_mul(L,       4, 4,
                U,       4, 4,
                LU_test, 4, 4);

    unsigned int i;
    for (i=0; i<16; i++)
        CONTEND_DELTA( LU_test[i], A[i], tol );
}

// 
// AUTOTEST: L/U decomp (Doolittle)
//
void autotest_matrixf_ludecomp_doolittle()
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
    matrixf_ludecomp_doolittle(A,4,4,L,U,P);

    if (liquid_autotest_verbose) {
        printf("L :\n");
        matrixf_print(L,4,4);
        printf("U :\n");
        matrixf_print(U,4,4);
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
    matrixf_mul(L,       4, 4,
                U,       4, 4,
                LU_test, 4, 4);

    unsigned int i;
    for (i=0; i<16; i++)
        CONTEND_DELTA( LU_test[i], A[i], tol );
}

// 
// AUTOTEST: inverse
//
void autotest_matrixf_inv()
{
    float tol = 1e-3f;

    float x[9] = {
       0.137864,  -0.444106,   0.394107,
      -0.571620,   0.106554,   0.058423,
       0.659264,   0.243594,  -0.122755};

    float x_inv[9];
    float x_inv_test[9] = {
       0.37791,  -0.57405,   0.94008,
       0.43799,   3.82934,   3.22867,
       2.89873,   4.51595,   3.30942};

    memmove(x_inv, x, sizeof(x));
    matrixf_inv(x_inv,3,3);

    unsigned int i;
    for (i=0; i<9; i++)
        CONTEND_DELTA(x_inv[i], x_inv_test[i], tol);
}

// 
// AUTOTEST: Q/R decomp (Gram-Schmidt)
//
void autotest_matrixf_qrdecomp()
{
    float tol = 1e-4f;  // error tolerance

    float A[16]= {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 5.0f, 7.0f, 8.0f,
        6.0f, 4.0f, 8.0f, 7.0f,
        1.0f, 0.0f, 3.0f, 1.0f};

    float Q[16];
    float R[16];

    float Q_test[16] = {
        0.125988157669742,   0.617707763884251,   0.571886263589597,   0.524890659167824,
        0.629940788348712,   0.494166211107401,  -0.137252703261503,  -0.583211843519804,
        0.755928946018454,  -0.444749589996661,  -0.114377252717920,   0.466569474815843,
        0.125988157669742,  -0.420041279441291,   0.800640769025436,  -0.408248290463863};

    float R_test[16] = {
        7.937253933193772,    6.425396041156863,   11.212946032607075,   10.960969717267590,
        0.000000000000000,    1.927248223318863,    0.494166211107400,    2.890872334978294,
        0.000000000000000,    0.000000000000000,    2.241794153271220,    1.189523428266362,
        0.000000000000000,    0.000000000000000,    0.000000000000000,    0.291605921759903};

    unsigned int i;

    // run decomposition
    matrixf_qrdecomp_gramschmidt(A,4,4,Q,R);

    if (liquid_autotest_verbose) {
        printf("Q :\n");
        matrixf_print(Q,4,4);
        printf("R :\n");
        matrixf_print(R,4,4);
    }

    for (i=0; i<16; i++) {
        CONTEND_DELTA( Q[i], Q_test[i], tol );
        CONTEND_DELTA( R[i], R_test[i], tol );
    }

    // test Q*R  == A
    float QR_test[16];
    matrixf_mul(Q,4,4, R,4,4, QR_test,4,4);
    for (i=0; i<16; i++)
        CONTEND_DELTA( A[i], QR_test[i], tol );

    // test Q*Q' == eye(4)
    float QQT_test[16];
    matrixf_mul_hermitian(Q,4,4, QQT_test);
    float I4[16];
    matrixf_eye(I4,4);
    for (i=0; i<16; i++)
        CONTEND_DELTA( QQT_test[i], I4[i], tol );
}


// 
// AUTOTEST: Gram-Schmidt Orthonormalization
//
void autotest_matrixf_gramschmidt()
{
    float tol = 1e-6f;  // error tolerance

    float X[12] = {
        1., 2., 1.,
        0., 2., 0.,
        2., 3., 1.,
        1., 1., 0.};

    float V[12];

    float V_test[12] = {
        sqrtf(6.)/6.,   sqrtf(2.)/6.,   2./3.,
        0.,             2.*sqrtf(2.)/3.,-1./3.,
        sqrtf(6.)/3.,   0.,             0.,
        sqrtf(6.)/6.,   -sqrtf(2.)/6.,  -2./3.};

    unsigned int i;
    matrixf_gramschmidt(X,4,3,V);

    if (liquid_autotest_verbose) {
        printf("X:\n");
        matrixf_print(X,4,3);
        printf("normalized X:\n");
        matrixf_print(V,4,3);
        printf("expected:\n");
        matrixf_print(V_test,4,3);
    }

    // run test
    for (i=0; i<12; i++)
        CONTEND_DELTA( V[i], V_test[i], tol );
}

// 
// test Cholesky decomposition
//
void autotest_matrixf_chol()
{
    float tol = 1e-4f;  // error tolerance

    // lower triangular matrix with positive values on diagonal
    float L[16]= {
        1.01f,  0,      0,      0,
       -1.42f,  0.50f,  0.00f,  0,
        0.32f,  2.01f,  0.30f,  0,
       -1.02f, -0.32f, -1.65f,  1.07f};

    float A[16];    // A = L * L^T
    float Lp[16];   // output Cholesky decomposition

    unsigned int i;

    // compute A
    matrixf_mul_transpose(L,4,4,A);

    // run decomposition
    matrixf_chol(A,4,Lp);

    if (liquid_autotest_verbose) {
        printf("L :\n");
        matrixf_print(L,4,4);
        printf("A :\n");
        matrixf_print(A,4,4);
        printf("Lp:\n");
        matrixf_print(Lp,4,4);
    }

    for (i=0; i<16; i++)
        CONTEND_DELTA( L[i], Lp[i], tol );
}


// 
// AUTOTEST: conjugate gradient solver
//
void autotest_matrixf_cgsolve_n3()
{
    float tol = 1e-6f;  // error tolerance

    // symmetric positive definite matrx
    float A[9] = {
         2.0,   -1.0,    0.0,
        -1.0,    2.0,   -1.0,
         0.0,   -1.0,    2.0};

    float x_test[3] = {
      -0.32889,
      -0.72798,
      -1.23532};

    float b[3];
    float x[3];

    // compute b = A*x_test
    matrixf_mul(A,      3, 3,
                x_test, 3, 1,
                b,      3, 1);

    matrixf_cgsolve(A,3,b,x,NULL);

    if (liquid_autotest_verbose) {
        printf("A:\n");
        matrixf_print(A,3,3);
        printf("b:\n");
        matrixf_print(b,3,1);
        printf("x:\n");
        matrixf_print(x_test,3,1);
        printf("x (computed):\n");
        matrixf_print(x,3,1);
    }

    // run test
    CONTEND_DELTA( x[0], x_test[0], tol );
    CONTEND_DELTA( x[1], x_test[1], tol );
    CONTEND_DELTA( x[2], x_test[2], tol );
}

// 
// AUTOTEST: conjugate gradient solver
//
void autotest_matrixf_cgsolve_n5()
{
    float tol = 1e-3f;  // error tolerance

    // symmetric positive definite matrx
    float A[25] = {
      0.20039785,  0.52039748,  0.12896973, -0.24593210, -0.11671682,
      0.52039748,  2.83168435, -0.02850759, -1.33722734, -0.86435664,
      0.12896973, -0.02850759,  0.20677096,  0.03188134,  0.00454172,
     -0.24593210, -1.33722734,  0.03188134,  0.77570134,  0.42975491,
     -0.11671682, -0.86435664,  0.00454171,  0.42975491,  0.45030650};

    float x_test[5] = {
     14.14453125,
     32.79626465,
    -32.57263184,
    -84.38003540,
     29.31457520};

    float b[5];
    float x[5];

    // compute b = A*x_test
    matrixf_mul(A,      5, 5,
                x_test, 5, 1,
                b,      5, 1);

    matrixf_cgsolve(A,5,b,x,NULL);

    if (liquid_autotest_verbose) {
        printf("A:\n");
        matrixf_print(A,5,5);
        printf("b:\n");
        matrixf_print(b,5,1);
        printf("x:\n");
        matrixf_print(x_test,5,1);
        printf("x (computed):\n");
        matrixf_print(x,5,1);
    }

    // run test
    CONTEND_DELTA( x[0], x_test[0], tol );
    CONTEND_DELTA( x[1], x_test[1], tol );
    CONTEND_DELTA( x[2], x_test[2], tol );
    CONTEND_DELTA( x[3], x_test[3], tol );
    CONTEND_DELTA( x[4], x_test[4], tol );
}

