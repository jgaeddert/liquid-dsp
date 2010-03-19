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

//
// References
//  [Ziemer:1998] Ziemer, Tranter, Fannin, "Signals & Systems,
//      Continuous and Discrete," 4th ed., Prentice Hall, Upper
//      Saddle River, NJ, 1998

#ifndef __LIQUID_IIRDES_AUTOTEST_H__
#define __LIQUID_IIRDES_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.internal.h"

// 
// AUTOTEST : design 2nd-order butterworth filter (design comes
//            from [Ziemer:1998] Example 9-7, pp. 440--442)
//
void autotest_iirdes_butter_2()
{
    // initialize variables
    unsigned int order = 2; // filter order
    float fc = 0.25f;       // normalized cutoff frequency
    float f0 = 0.0f;        // center frequency (ignored for low-pass filter)
    float Ap = 1.0f;        // pass-band ripple (ignored for Butterworth)
    float As = 40.0f;       // stop-band attenuation (ignored for Butterworth)
    float tol = 1e-6f;      // error tolerance

    // initialize pre-determined coefficient array
    // for 2^nd-order low-pass Butterworth filter
    // with cutoff frequency 0.25
    float a_test[3] = {
        1.0f,
        0.0f,
        0.171572875253810f};
    float b_test[3] = {
        0.292893218813452f,
        0.585786437626905f,
        0.292893218813452f};

    // output coefficients
    float a[3];
    float b[3];

    // design butterworth filter
    iirdes(LIQUID_IIRDES_BUTTER,
           LIQUID_IIRDES_LOWPASS,
           LIQUID_IIRDES_TF,
           order,
           fc, f0,
           Ap, As,
           b, a);

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
#if 0
void xautotest_iirdes_cplxpair()
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
#endif

// 
// AUTOTEST : 
//
void autotest_iirdes_dzpk2sosf()
{
    unsigned int n=4;
    float fc = 0.25f;

    unsigned int i;
    unsigned int L = n % 2 ? (n+1)/2 : n/2;
    float B[3*L];
    float A[3*L];

    float complex za[n];    // analog zeros
    float complex pa[n];    // analog poles
    float complex ka;       // analog gain
    butter_azpkf(n,fc,za,pa,&ka);

    float complex zd[n];    // digital zeros
    float complex pd[n];    // digital poles
    float complex kd;       // digital gain
    float m = 1 / tanf(M_PI * fc);
    bilinear_zpkf(za,  0,
                  pa,  n,
                  ka,  m,
                  zd, pd, &kd);

    printf("poles (digital):\n");
    for (i=0; i<n; i++)
        printf("  pd[%3u] = %12.8f + j*%12.8f\n", i, crealf(pd[i]), cimagf(pd[i]));

    printf("zeros (digital):\n");
    for (i=0; i<n; i++)
        printf("  zd[%3u] = %12.8f + j*%12.8f\n", i, crealf(zd[i]), cimagf(zd[i]));

    iirdes_dzpk2sosf(zd,pd,n,kd,B,A);

    printf("B:\n");
    for (i=0; i<L; i++)
        printf("  %12.8f %12.8f %12.8f\n", B[3*i+0], B[3*i+1], B[3*i+2]);

    printf("A:\n");
    for (i=0; i<L; i++)
        printf("  %12.8f %12.8f %12.8f\n", A[3*i+0], A[3*i+1], A[3*i+2]);
}

#endif // __LIQUID_IIRDES_AUTOTEST_H__

