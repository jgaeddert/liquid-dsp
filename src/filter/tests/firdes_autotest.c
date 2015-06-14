/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

void autotest_liquid_firdes_rcos() {

    // Initialize variables
    unsigned int k=2, m=3;
    float beta=0.3f;
    float offset=0.0f;

    // Initialize pre-determined coefficient array
    float h0[13] = {
       1.65502646542134e-17,
       7.20253052925685e-02,
      -1.26653717080575e-16,
      -1.74718023726940e-01,
       2.95450626814946e-16,
       6.23332275392119e-01,
       1.00000000000000e+00,
       6.23332275392119e-01,
      -2.23850244261176e-16,
      -1.74718023726940e-01,
      -2.73763990895627e-17,
       7.20253052925685e-02
    };

    // Create filter
    float h[13];
    liquid_firdes_rcos(k,m,beta,offset,h);

    // Ensure data are equal
    unsigned int i;
    for (i=0; i<13; i++)
        CONTEND_DELTA( h[i], h0[i], 0.00001f );
}

void autotest_liquid_firdes_rrcos() {

    // Initialize variables
    unsigned int k=2, m=3;
    float beta=0.3f;
    float offset=0.0f;

    // Initialize pre-determined coefficient array
    float h0[13] = {
       -3.311577E-02, 
        4.501582E-02, 
        5.659688E-02, 
       -1.536039E-01, 
       -7.500154E-02, 
        6.153450E-01, 
        1.081972E+00, 
        6.153450E-01, 
       -7.500154E-02, 
       -1.536039E-01, 
        5.659688E-02, 
        4.501582E-02,
       -3.311577E-02}; 

    // Create filter
    float h[13];
    liquid_firdes_rrcos(k,m,beta,offset,h);

    // Ensure data are equal
    unsigned int i;
    for (i=0; i<13; i++)
        CONTEND_DELTA( h[i], h0[i], 0.00001f );
}


void autotest_liquid_firdes_rkaiser()
{
    // Initialize variables
    unsigned int k=2, m=3;
    float beta=0.3f;
    float offset=0.0f;
    float isi_test = -30.0f;

    // Create filter
    unsigned int h_len = 2*k*m+1;
    float h[h_len];
    liquid_firdes_rkaiser(k,m,beta,offset,h);

    // compute filter ISI
    float isi_max;
    float isi_rms;
    liquid_filter_isi(h,k,m,&isi_rms,&isi_max);

    // convert to log scale
    isi_max = 20*log10f(isi_max);
    isi_rms = 20*log10f(isi_rms);

    // ensure ISI is sufficiently small
    CONTEND_LESS_THAN(isi_max, isi_test);
    CONTEND_LESS_THAN(isi_rms, isi_test);
}


