/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
#include "liquid.autotest.h"
#include "liquid.internal.h"

LIQUID_AUTOTEST(iirdes_cplxpair_n6,"complex pair, n=6", "", 0.1)
{
    float tol = 1e-8f;

    //
    liquid_float_complex r[6] = {
       0.980066577841242 + 0.198669330795061 * _Complex_I,
       5.000000000000000 + 0.000000000000000 * _Complex_I,
      -0.416146836547142 + 0.909297426825682 * _Complex_I,
       0.980066577841242 - 0.198669330795061 * _Complex_I,
       0.300000000000000 + 0.000000000000000 * _Complex_I,
      -0.416146836547142 - 0.909297426825682 * _Complex_I
    };

    LIQUID_VLA(liquid_float_complex, p, 6);

    liquid_float_complex ptest[6] = {
      -0.416146836547142 - 0.909297426825682 * _Complex_I,
      -0.416146836547142 + 0.909297426825682 * _Complex_I,
       0.980066577841242 - 0.198669330795061 * _Complex_I,
       0.980066577841242 + 0.198669330795061 * _Complex_I,
       0.300000000000000 + 0.000000000000000 * _Complex_I,
       5.000000000000000 + 0.000000000000000 * _Complex_I
    };

    // compute complex pairs
    liquid_cplxpair(r,6,1e-6f,p);

    unsigned int i;

    liquid_log_debug("complex set:");
    for (i=0; i<6; i++)
        liquid_log_debug("  r[%3u] : %12.8f + j*%12.8f", i, crealf(r[i]), cimagf(r[i]));

    liquid_log_debug("complex pairs:");
    for (i=0; i<6; i++)
        liquid_log_debug("  p[%3u] : %12.8f + j*%12.8f", i, crealf(p[i]), cimagf(p[i]));

    // run test
    for (i=0; i<6; i++) {
        LIQUID_CHECK_DELTA( crealf(p[i]), crealf(ptest[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(p[i]), cimagf(ptest[i]), tol );
    }
}


LIQUID_AUTOTEST(iirdes_cplxpair_n20,"complex pair, n=20", "", 0.1)
{
    float tol = 1e-8f;

    //
    liquid_float_complex r[20] = {
      -0.340396183901119 + 1.109902927794652 * _Complex_I,
       1.148964416793990 + 0.000000000000000 * _Complex_I,
       0.190037889511651 + 0.597517076404221 * _Complex_I,
      -0.340396183901119 - 1.109902927794652 * _Complex_I,
       0.890883293686046 + 0.000000000000000 * _Complex_I,
      -0.248338528396292 - 0.199390430636670 * _Complex_I,
       0.190037889511651 - 0.597517076404221 * _Complex_I,
       0.003180396218998 + 0.000000000000000 * _Complex_I,
       0.261949046540733 - 0.739400953405199 * _Complex_I,
       0.261949046540733 + 0.739400953405199 * _Complex_I,
       0.309342570837113 + 0.000000000000000 * _Complex_I,
       0.035516103001236 + 0.000000000000000 * _Complex_I,
      -0.184159864176452 - 0.240335024546875 * _Complex_I,
      -0.485244526317243 + 0.452251520655749 * _Complex_I,
      -0.485244526317243 - 0.452251520655749 * _Complex_I,
      -0.581633365450190 + 0.000000000000000 * _Complex_I,
      -0.248338528396292 + 0.199390430636670 * _Complex_I,
      -0.184159864176452 + 0.240335024546875 * _Complex_I,
       1.013685316242435 + 0.000000000000000 * _Complex_I,
      -0.089598596934739 + 0.000000000000000 * _Complex_I
    };

    LIQUID_VLA(liquid_float_complex, p, 20);

    liquid_float_complex ptest[20] = {
      -0.485244526317243 - 0.452251520655749 * _Complex_I,
      -0.485244526317243 + 0.452251520655749 * _Complex_I,
      -0.340396183901119 - 1.109902927794652 * _Complex_I,
      -0.340396183901119 + 1.109902927794652 * _Complex_I,
      -0.248338528396292 - 0.199390430636670 * _Complex_I,
      -0.248338528396292 + 0.199390430636670 * _Complex_I,
      -0.184159864176452 - 0.240335024546875 * _Complex_I,
      -0.184159864176452 + 0.240335024546875 * _Complex_I,
       0.190037889511651 - 0.597517076404221 * _Complex_I,
       0.190037889511651 + 0.597517076404221 * _Complex_I,
       0.261949046540733 - 0.739400953405199 * _Complex_I,
       0.261949046540733 + 0.739400953405199 * _Complex_I,
      -0.581633365450190 + 0.000000000000000 * _Complex_I,
      -0.089598596934739 + 0.000000000000000 * _Complex_I,
       0.003180396218998 + 0.000000000000000 * _Complex_I,
       0.035516103001236 + 0.000000000000000 * _Complex_I,
       0.309342570837113 + 0.000000000000000 * _Complex_I,
       0.890883293686046 + 0.000000000000000 * _Complex_I,
       1.013685316242435 + 0.000000000000000 * _Complex_I,
       1.148964416793990 + 0.000000000000000 * _Complex_I
    };

    // compute complex pairs
    liquid_cplxpair(r,20,1e-6f,p);

    unsigned int i;

    liquid_log_debug("complex set:");
    for (i=0; i<20; i++)
        liquid_log_debug("  r[%3u] : %12.8f + j*%12.8f", i, crealf(r[i]), cimagf(r[i]));

    liquid_log_debug("complex pairs:");
    for (i=0; i<20; i++)
        liquid_log_debug("  p[%3u] : %12.8f + j*%12.8f", i, crealf(p[i]), cimagf(p[i]));

    // run test
    for (i=0; i<20; i++) {
        LIQUID_CHECK_DELTA( crealf(p[i]), crealf(ptest[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(p[i]), cimagf(ptest[i]), tol );
    }
}

LIQUID_AUTOTEST(iirdes_dzpk2sosf,"digital zeros/poles/gain to second-order sections", "", 0.1)
{
    unsigned int n=4;
    float fc = 0.25f;

    unsigned int i;
    unsigned int L = n % 2 ? (n+1)/2 : n/2;
    LIQUID_VLA(float, B, 3*L);
    LIQUID_VLA(float, A, 3*L);

    LIQUID_VLA(liquid_float_complex, za, n);    // analog zeros
    LIQUID_VLA(liquid_float_complex, pa, n);    // analog poles
    liquid_float_complex ka;       // analog gain
    butter_azpkf(n,za,pa,&ka);

    LIQUID_VLA(liquid_float_complex, zd, n);    // digital zeros
    LIQUID_VLA(liquid_float_complex, pd, n);    // digital poles
    liquid_float_complex kd;       // digital gain
    float m = 1 / tanf(M_PI * fc);
    bilinear_zpkf(za,  0,
                  pa,  n,
                  ka,  m,
                  zd, pd, &kd);

    liquid_log_debug("poles (digital):");
    for (i=0; i<n; i++)
        liquid_log_debug("  pd[%3u] = %12.8f + j*%12.8f", i, crealf(pd[i]), cimagf(pd[i]));

    liquid_log_debug("zeros (digital):");
    for (i=0; i<n; i++)
        liquid_log_debug("  zd[%3u] = %12.8f + j*%12.8f", i, crealf(zd[i]), cimagf(zd[i]));

    iirdes_dzpk2sosf(zd,pd,n,kd,B,A);

    liquid_log_debug("B:");
    for (i=0; i<L; i++)
        liquid_log_debug("  %12.8f %12.8f %12.8f", B[3*i+0], B[3*i+1], B[3*i+2]);

    liquid_log_debug("A:");
    for (i=0; i<L; i++)
        liquid_log_debug("  %12.8f %12.8f %12.8f", A[3*i+0], A[3*i+1], A[3*i+2]);
}

LIQUID_AUTOTEST(iirdes_isstable_n2_yes,"iirdes_isstable", "", 0.1)
{
    // initialize pre-determined coefficient array
    // for 2^nd-order low-pass Butterworth filter
    // with cutoff frequency 0.25
    float a[3] = {
        1.0f,
        0.0f,
        0.171572875253810f};
    float b[3] = {
        0.292893218813452f,
        0.585786437626905f,
        0.292893218813452f};

    int stable = iirdes_isstable(b,a,3);
    LIQUID_CHECK( stable ==  1 );
}


LIQUID_AUTOTEST(iirdes_isstable_n2_no,"iirdes_isstable", "", 0.1)
{
    // initialize unstable filter
    float a[3] = {
        1.0f,
        0.0f,
        1.171572875253810f};
    float b[3] = {
        0.292893218813452f,
        0.585786437626905f,
        0.292893218813452f};

    int stable = iirdes_isstable(b,a,3);
    LIQUID_CHECK( stable ==  0 );
}

