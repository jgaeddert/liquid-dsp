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

#ifdef _I
#  undef _I
#endif

#define _I _Complex_I

// 
// AUTOTEST: Test matrixcf add
//
void autotest_matrixcf_add() {

    float complex x[6] = {
      2+  3*_I,  10+  9*_I,   1+  3*_I, 
      7+  2*_I,   8+  6*_I,   3+  1*_I
    };

    float complex y[6] = {
      7+  3*_I,   4+  8*_I,   6+  6*_I, 
      1+  9*_I,   7+ 10*_I,   5+  1*_I
    };

    float complex z[6];
    float complex ztest[6] = {
      9+  6*_I,  14+ 17*_I,   7+  9*_I, 
      8+ 11*_I,  15+ 16*_I,   8+  2*_I
    };

    matrixcf_add(x,y,z,2,3);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: Test matrixcf ops
//
void autotest_matrixcf_mul() {

    float complex x[6] = {
      9+  9*_I,   3+  4*_I,   8+  3*_I, 
      0+  3*_I,   6+ 10*_I,   6+  1*_I
    };

    float complex y[9] = {
      8+  7*_I,   4+  9*_I,   4+  1*_I, 
      5+  6*_I,  10+  2*_I,   6+  3*_I, 
      6+  9*_I,   2+  8*_I,   5+  5*_I
    };

    float complex z[6];
    float complex ztest[6] = {
     21+263*_I, -31+233*_I,  58+133*_I, 
    -24+170*_I,  17+174*_I,  28+125*_I
    };
    
    matrixcf_mul(x,2,3, y,3,3, z,2,3);
    if (liquid_autotest_verbose)
        matrixcf_print(z,2,3);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: identity
//
void autotest_matrixcf_eye() {
    float complex x[16]= {
      9+  4*_I,   4+  9*_I,   5+  4*_I,   4+  5*_I, 
      3+  9*_I,   9+  2*_I,   9+  7*_I,   9+  2*_I, 
      9+  4*_I,   1+  9*_I,   8+  6*_I,   0+  2*_I, 
      4+  9*_I,   3+  6*_I,   3+  3*_I,   7+  8*_I
    };

    float complex I4_test[16] = {
        1,  0,  0,  0,
        0,  1,  0,  0,
        0,  0,  1,  0,
        0,  0,  0,  1};

    float complex y[16];
    float complex z[16];

    // generate identity matrix
    matrixcf_eye(y,4);
    CONTEND_SAME_DATA(y, I4_test, 16*sizeof(float complex));

    // multiply with input
    matrixcf_mul(x, 4, 4,
                 y, 4, 4,
                 z, 4, 4);
    CONTEND_SAME_DATA(x, z, 16*sizeof(float complex));
}

// 
// AUTOTEST: inverse
//
void autotest_matrixcf_inv()
{
    float tol = 1e-3f;

    float complex x[9] = {
      0.054076+  0.263160*_I,   0.570850+ -0.208230*_I,   0.551480+ -0.189100*_I, 
     -0.223700+  0.298170*_I,   0.416250+  1.152200*_I,  -0.299920+  0.469310*_I, 
     -1.485400+ -0.192370*_I,  -0.679430+  0.528100*_I,  -0.827860+ -0.345740*_I
    };

    float complex x_inv[9];
    float complex x_inv_test[9] = {
     -0.277900+ -0.717820*_I,   0.559370+  0.305450*_I,  -0.691300+  0.080448*_I, 
     -0.026647+ -0.650270*_I,   0.702820+ -0.990530*_I,   0.237160+  0.231150*_I, 
      1.319100+  1.318300*_I,  -0.539880+  0.808700*_I,  -0.255610+  0.084624*_I
    };

    memmove(x_inv, x, sizeof(x));
    matrixcf_inv(x_inv,3,3);

    unsigned int i;
    for (i=0; i<9; i++) {
        CONTEND_DELTA(crealf(x_inv[i]), crealf(x_inv_test[i]), tol);
        CONTEND_DELTA(cimagf(x_inv[i]), cimagf(x_inv_test[i]), tol);
    }
}

// 
// AUTOTEST: Test matrixcf multiply/transpose
//
void autotest_matrixcf_transmul()
{
    // error tolerance
    float tol = 1e-4f;

    // 3 x 2 complex matrix
    float complex x[6] = {
      -1.984187180846 - _I*1.160899576428,  -0.094097208353 - _I*1.664868369484,
       1.444228871367 - _I*1.830348997497,  -2.157958848569 - _I*0.672986734466,
      -0.889198235505 - _I*2.191307103110,  -0.532344808521 - _I*1.384544279335};

    // computation vectors
    float complex xxT[9];
    float complex xTx[4];
    float complex xxH[9];
    float complex xHx[4];

    // test vectors

    float complex xxT_test[9] = {
        8.06533 +  _I*0.00000,    0.58272 -  _I*1.77897,    6.66340 -  _I*2.55969,
        0.58272 +  _I*1.77897,   10.54567 +  _I*0.00000,    4.80721 +  _I*2.16276,
        6.66340 +  _I*2.55969,    4.80721 -  _I*2.16276,    7.79285 +  _I*0.00000};

    float complex xTx_test[4] = {
       16.3132 +  _I*0.0000,    3.7420 -  _I*1.6630,
        3.7420 +  _I*1.6630,   10.0907 +  _I*0.0000};

    float complex xxH_test[9] = {
      -0.1736 + _I*4.9202,  -5.9078 + _I*5.6112,  -3.0345 + _I*6.3968,
      -5.9078 + _I*5.6112,   2.9395 - _I*2.3823,  -5.0781 + _I*1.8088,
      -3.0345 + _I*6.3968,  -5.0781 + _I*1.8088,  -5.6447 + _I*5.3711};

    float complex xHx_test[4] = {
      -2.6862 + _I*3.2170,  -8.6550 + _I*8.7882,
      -8.6550 + _I*8.7882,  -0.1926 + _I*4.6920};

    // run computations
    matrixcf_mul_transpose(x,3,2,xxT);
    matrixcf_transpose_mul(x,3,2,xTx);
    matrixcf_mul_hermitian(x,3,2,xxH);
    matrixcf_hermitian_mul(x,3,2,xHx);

    // test results
    unsigned int i;

    for (i=0; i<9; i++) {
        CONTEND_DELTA( crealf(xxT[i]), crealf(xxT_test[i]), tol );
        CONTEND_DELTA( cimagf(xxT[i]), cimagf(xxT_test[i]), tol );
    }

    for (i=0; i<4; i++) {
        CONTEND_DELTA( crealf(xTx[i]), crealf(xTx_test[i]), tol );
        CONTEND_DELTA( cimagf(xTx[i]), cimagf(xTx_test[i]), tol );
    }

    for (i=0; i<9; i++) {
        CONTEND_DELTA( crealf(xxH[i]), crealf(xxH_test[i]), tol );
        CONTEND_DELTA( cimagf(xxH[i]), cimagf(xxH_test[i]), tol );
    }

    for (i=0; i<4; i++) {
        CONTEND_DELTA( crealf(xHx[i]), crealf(xHx_test[i]), tol );
        CONTEND_DELTA( cimagf(xHx[i]), cimagf(xHx_test[i]), tol );
    }
}

// 
// AUTOTEST: Q/R decomp (Gram-Schmidt)
//
void autotest_matrixcf_qrdecomp()
{
    float tol = 1e-4f;  // error tolerance

    float complex A[16]= {
       2.11402 - 0.57604*_I,  0.41750 + 1.00833*_I, -0.96264 - 3.62196*_I, -0.20679 - 1.02668*_I,
       0.00854 + 1.61626*_I,  0.84695 - 0.32736*_I, -1.01862 - 1.10786*_I, -1.78877 + 1.84456*_I,
      -2.97901 - 1.30384*_I,  0.52289 + 1.89110*_I,  1.32576 - 0.36737*_I,  0.04717 + 0.20628*_I,
       0.28970 + 0.64247*_I, -0.55916 + 0.68302*_I,  1.40615 + 0.62398*_I, -0.12767 - 0.53997*_I};


    float complex Q[16];
    float complex R[16];

    float complex Q_test[16] = {
       0.4917069 - 0.1339829*_I,  0.4296604 + 0.5598343*_I, -0.3093337 - 0.2783187*_I,  0.2152058 - 0.1509579*_I,
       0.0019872 + 0.3759317*_I,  0.2427682 + 0.0092564*_I, -0.4223052 - 0.0325125*_I, -0.5035680 + 0.6055326*_I,
      -0.6928955 - 0.3032643*_I,  0.0541098 + 0.4680729*_I, -0.0821501 + 0.0696538*_I,  0.2796678 + 0.3407228*_I,
       0.0673832 + 0.1494333*_I, -0.2704656 + 0.3844264*_I, -0.2850673 + 0.7447073*_I, -0.1735849 - 0.2936151*_I};

    float complex R_test[16] = {
       4.29936 + 0.00000*_I, -0.92262 - 0.78948*_I, -1.02577 - 1.04067*_I,  0.54122 - 0.00234*_I,
       0.00000 + 0.00000*_I,  2.27373 + 0.00000*_I, -2.93951 - 2.62657*_I, -1.15474 + 0.32321*_I,
       0.00000 + 0.00000*_I,  0.00000 + 0.00000*_I,  1.70137 + 0.00000*_I,  0.68991 - 0.34832*_I,
       0.00000 + 0.00000*_I,  0.00000 + 0.00000*_I,  0.00000 + 0.00000*_I,  2.39237 + 0.00000*_I};

    unsigned int i;

    // run decomposition
    matrixcf_qrdecomp_gramschmidt(A,4,4,Q,R);

    if (liquid_autotest_verbose) {
        printf("Q :\n");
        matrixcf_print(Q,4,4);
        printf("expected Q :\n");
        matrixcf_print(Q_test,4,4);

        printf("\n\n");
        printf("R :\n");
        matrixcf_print(R,4,4);
        printf("expected R :\n");
        matrixcf_print(R_test,4,4);
    }

    for (i=0; i<16; i++) {
        CONTEND_DELTA( crealf(Q[i]), crealf(Q_test[i]), tol );
        CONTEND_DELTA( cimagf(Q[i]), cimagf(Q_test[i]), tol );

        CONTEND_DELTA( crealf(R[i]), crealf(R_test[i]), tol );
        CONTEND_DELTA( cimagf(R[i]), cimagf(R_test[i]), tol );
    }

    // test Q*R  == A
    float complex QR_test[16];
    matrixcf_mul(Q,4,4, R,4,4, QR_test,4,4);
    for (i=0; i<16; i++)
        CONTEND_DELTA( A[i], QR_test[i], tol );

    // test Q*Q' == eye(4)
    float complex QQT_test[16];
    matrixcf_mul_transpose(Q,4,4, QQT_test);
    float complex I4[16];
    matrixcf_eye(I4,4);
    for (i=0; i<16; i++)
        CONTEND_DELTA( QQT_test[i], I4[i], tol );
}

// 
// test Cholesky decomposition
//
void autotest_matrixcf_chol()
{
    float tol = 1e-3f;  // error tolerance

    // lower triangular matrix with positive values on diagonal
    float complex L[16]= {
        1.01,                    0,                       0,                       0,
       -1.42 + _Complex_I*0.25,  0.50,                    0,                       0,
        0.32 - _Complex_I*1.23,  2.01 + _Complex_I*0.78,  0.30,                    0,
       -1.02 + _Complex_I*1.02, -0.32 - _Complex_I*0.03, -1.65 + _Complex_I*2.01,  1.07};

    float complex A[16];    // A = L * L^T
    float complex Lp[16];   // output Cholesky decomposition

    unsigned int i;

    // compute A
    matrixcf_mul_transpose(L,4,4,A);

    // force A to be positive definite
    for (i=0; i<4; i++)
        matrix_access(A,4,4,i,i) = creal(matrix_access(A,4,4,i,i));

    // run decomposition
    matrixcf_chol(A,4,Lp);

    if (liquid_autotest_verbose) {
        printf("L :\n");
        matrixcf_print(L,4,4);
        printf("A :\n");
        matrixcf_print(A,4,4);
        printf("Lp:\n");
        matrixcf_print(Lp,4,4);
    }

    for (i=0; i<16; i++) {
        CONTEND_DELTA( crealf(L[i]), crealf(Lp[i]), tol );
        CONTEND_DELTA( cimagf(L[i]), cimagf(Lp[i]), tol );
    }
}


