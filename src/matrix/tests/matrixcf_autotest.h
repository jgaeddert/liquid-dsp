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
void xautotest_matrixcf_transmul()
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

