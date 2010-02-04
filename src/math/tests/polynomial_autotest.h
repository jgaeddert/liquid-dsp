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

#ifndef __LIQUID_POLYNOMIAL_AUTOTEST_H__
#define __LIQUID_POLYNOMIAL_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: polyfit 3rd order polynomial, critically sampled
//
void autotest_fpolyfit_q3n3()
{
    unsigned int Q=2;   // polynomial order
    unsigned int n=3;   // input vector size

    float x[3] = {-1.0f, 0.0f, 1.0f};
    float y[3] = { 1.0f, 0.0f, 1.0f};
    float p[3];
    float p_test[3] = {0.0f, 0.0f, 1.0f};
    float tol = 1e-3f;

    unsigned int k=Q+1;
    fpolyfit(x,y,n, p,k);

    if (liquid_autotest_verbose) {
        unsigned int i;
        for (i=0; i<3; i++)
            printf("%3u : %12.8f > %12.8f\n", i, x[i], y[i]);
        for (i=0; i<3; i++)
            printf("p[%3u] = %12.8f\n", i, p[i]);
    }
    
    CONTEND_DELTA(p[0], p_test[0], tol);
    CONTEND_DELTA(p[1], p_test[1], tol);
    CONTEND_DELTA(p[2], p_test[2], tol);
    //CONTEND_DELTA(p[3], p_test[3], tol);
}

// 
// AUTOTEST: poly_expandroots
//
void autotest_poly_expandroots_4()
{
    float a[5] = { 2, 1, 4, -5, -3 };
    float c[6];
    float c_test[6] = { 120, 146, 1, -27, -1, 1 };
    float tol = 1e-3f;

    fpoly_expandroots(a,5,c);

    if (liquid_autotest_verbose) {
        unsigned int i;
        printf("c[6] = ");
        for (i=0; i<6; i++)
            printf("%8.2f", c[i]);
        printf("\n");
    }
    
    CONTEND_DELTA(c[0], c_test[0], tol);
    CONTEND_DELTA(c[1], c_test[1], tol);
    CONTEND_DELTA(c[2], c_test[2], tol);
    CONTEND_DELTA(c[3], c_test[3], tol);
    CONTEND_DELTA(c[4], c_test[4], tol);
    CONTEND_DELTA(c[5], c_test[5], tol);
}

// 
// AUTOTEST: polymul
//
void autotest_polymul_2_3()
{
    float a[3] = {  2, -4,  3 };
    float b[4] = { -9,  3, -2,  5};
    float c[6];
    float c_test[6] = { -18, 42, -43, 27, -26, 15 };
    float tol = 1e-3f;

    fpolymul(a,2,b,3,c);

    if (liquid_autotest_verbose) {
        unsigned int i;
        printf("c[6] = ");
        for (i=0; i<6; i++)
            printf("%8.2f", c[i]);
        printf("\n");
    }
    
    CONTEND_DELTA(c[0], c_test[0], tol);
    CONTEND_DELTA(c[1], c_test[1], tol);
    CONTEND_DELTA(c[2], c_test[2], tol);
    CONTEND_DELTA(c[3], c_test[3], tol);
    CONTEND_DELTA(c[4], c_test[4], tol);
    CONTEND_DELTA(c[5], c_test[5], tol);
}

#endif 

