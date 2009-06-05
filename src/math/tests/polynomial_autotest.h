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
// AUTOTEST: polyfit
//
void autotest_polyfit_q3n4()
{
    unsigned int Q=3;   // polynomial order
    unsigned int n=4;   // input vector size

    float x[4] = {0.2, 0.5, 0.7, 0.9};
    float y[4] = {0.2, 0.6, 0.9, 1.5};
    float p[Q+1];
    float p_test[4] = {
        -0.37500000,
         3.97976190,
        -6.50000000,
         4.88095238f};
    float tol = 1e-3f;

    polyfit(x,y,n, p,Q);

    unsigned int i;
    for (i=0; i<Q+1; i++)
        printf("p[%3u] = %12.8f\n", i, p[i]);
    
    CONTEND_DELTA(p[0], p_test[0], tol);
    CONTEND_DELTA(p[1], p_test[1], tol);
    CONTEND_DELTA(p[2], p_test[2], tol);
    CONTEND_DELTA(p[3], p_test[3], tol);
}

#endif 

