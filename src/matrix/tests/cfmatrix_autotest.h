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

#ifndef __LIQUID_CFMATRIX_AUTOTEST_H__
#define __LIQUID_CFMATRIX_AUTOTEST_H__

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.h"

#ifdef _I
#  undef _I
#endif

#define _I _Complex_I

// 
// AUTOTEST: Test cfmatrix add
//
void autotest_cfmatrix_add() {

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

    cfmatrix_add(x,y,z,2,3);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: Test cfmatrix ops
//
void autotest_cfmatrix_mul() {

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
    
    cfmatrix_mul(x,2,3, y,3,3, z,2,3);
    if (liquid_autotest_verbose)
        cfmatrix_print(z,2,3);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: identity
//
void autotest_cfmatrix_eye() {
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
    cfmatrix_eye(y,4);
    CONTEND_SAME_DATA(y, I4_test, 16*sizeof(float complex));

    // multiply with input
    cfmatrix_mul(x, 4, 4,
                 y, 4, 4,
                 z, 4, 4);
    CONTEND_SAME_DATA(x, z, 16*sizeof(float complex));
}

// 
// AUTOTEST: inverse
//
void autotest_cfmatrix_inv()
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
    cfmatrix_inv(x_inv,3,3);

    unsigned int i;
    for (i=0; i<9; i++) {
        CONTEND_DELTA(crealf(x_inv[i]), crealf(x_inv_test[i]), tol);
        CONTEND_DELTA(cimagf(x_inv[i]), cimagf(x_inv_test[i]), tol);
    }
}

#endif // __LIQUID_CFMATRIX_AUTOTEST_H__
