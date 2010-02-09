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

#ifndef __LIQUID_IIRDES_AUTOTEST_H__
#define __LIQUID_IIRDES_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// Bibliography:
// [1] Zimer, Tranter, Fannin, "Signals & Systems, Continuous
//     and Discrete," 4th ed., Prentice Hall, Upper Saddle
//     River, NJ, 1998

// 
// AUTOTEST : design 2nd-order butterworth filter (design
//            comes from [1] Example 9-7, pp. 440--442
//
void autotest_iirdes_butter_2()
{
    // initialize variables
    unsigned int n = 2;     // filter order
    float fc = 0.25f;       // normalized cutoff frequency
    float tol = 1e-6f;      // error tolerance

    // output coefficients
    float a[3];
    float b[3];

    // initialize pre-determined coefficient array
    float a_test[3] = {
        1.0f,
        0.0f,
        0.171572875253810f};
    float b_test[3] = {
        0.292893218813452f,
        0.585786437626905f,
        0.292893218813452f};

    // design filter
    butterf(n,fc,b,a);

    // Ensure data are equal to within tolerance
    unsigned int i;
    for (i=0; i<3; i++) {
        CONTEND_DELTA( b[i], b_test[i], tol );
        CONTEND_DELTA( a[i], a_test[i], tol );
    }
}

// 
// AUTOTEST : 
//
void autotest_iirdes_cplxpair()
{
    unsigned int n=5;
    float complex r[n];
    float complex p[n];

    butter_rootsf(n,r);

    liquid_cplxpair(r,n,1e-6f,p);

    unsigned int i;
    printf("roots:\n");
    for (i=0; i<n; i++)
        printf("  r[%3u] : %12.8f + j*%12.8f\n", i, crealf(r[i]), cimagf(r[i]));

    printf("paired roots:\n");
    for (i=0; i<n; i++)
        printf("  p[%3u] : %12.8f + j*%12.8f\n", i, crealf(p[i]), cimagf(p[i]));
}

// 
// AUTOTEST : 
//
void autotest_iirdes_zpk2sos()
{
    unsigned int n=5;
    float m=1.0f;

    float complex z[n]; // zeros
    float complex p[n]; // poles

    unsigned int L = n % 2 ? (n+1)/2 : n/2;
    float B[3*L];
    float A[3*L];

    float complex r[n]; // roots
    butter_rootsf(n,r);
    unsigned int i;
    for (i=0; i<n; i++) {
        p[i] = (r[i]/m - 1.0) / (r[i]/m + 1.0);
        z[i] = 1.0;
    }

    printf("poles:\n");
    for (i=0; i<n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(p[i]), cimagf(p[i]));

    printf("zeros:\n");
    for (i=0; i<n; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i, crealf(z[i]), cimagf(z[i]));

    iirdes_zpk2sos(z,p,n,1.0f,B,A);
}

#endif // __LIQUID_IIRDES_AUTOTEST_H__

