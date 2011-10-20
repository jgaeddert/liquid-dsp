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

#include "autotest/autotest.h"
#include "liquid.h"

void autotest_iirfilt_crcf_noise_01()
{
    // initialize filter with 2nd-order low-pass butterworth
    float a[3] = { 
        1.000000000000000,
       -0.942809041582063,
        0.333333333333333};

    float b[3] = { 
        0.0976310729378175,
        0.1952621458756350,
        0.0976310729378175};

    iirfilt_crcf f = iirfilt_crcf_create(b,3,a,3);

    // noise signal (input)
    float complex x[32] = {
     -1.3830e+00+ -8.1953e-01*_Complex_I,   1.1822e+00+  5.1835e-01*_Complex_I, 
      1.3043e+00+ -1.5471e+00*_Complex_I,   2.1511e+00+  9.4130e-02*_Complex_I, 
     -1.1309e+00+  4.2868e-01*_Complex_I,   1.3375e+00+  1.1528e+00*_Complex_I, 
      1.9402e+00+  5.0096e-01*_Complex_I,   1.4020e+00+ -9.7731e-01*_Complex_I, 
      8.6551e-01+  1.3291e+00*_Complex_I,   3.5416e-01+ -2.0070e+00*_Complex_I, 
      3.3764e-01+  1.0462e+00*_Complex_I,  -6.1531e-01+ -2.7888e-01*_Complex_I, 
      7.3587e-01+ -1.2247e+00*_Complex_I,   1.5724e+00+ -8.4525e-02*_Complex_I, 
      3.9648e-01+ -1.2167e+00*_Complex_I,   1.4494e-01+ -1.2502e+00*_Complex_I, 
      1.8058e-02+ -4.5100e-01*_Complex_I,   1.7843e-01+ -9.0617e-01*_Complex_I, 
     -1.0774e+00+ -1.5721e+00*_Complex_I,  -9.7350e-02+  1.0151e+00*_Complex_I, 
     -6.5302e-01+ -1.5959e+00*_Complex_I,  -5.6925e-02+  1.1245e+00*_Complex_I, 
     -3.8603e-01+ -9.7004e-01*_Complex_I,  -8.4837e-01+  8.7278e-01*_Complex_I, 
     -7.1488e-01+ -1.1243e+00*_Complex_I,  -1.9760e+00+ -8.7521e-01*_Complex_I, 
     -6.4040e-01+  2.9034e-01*_Complex_I,  -2.5674e-01+  1.5229e+00*_Complex_I, 
     -1.5919e+00+  1.6941e+00*_Complex_I,  -3.4932e-01+  1.5199e+00*_Complex_I, 
     -4.5390e-01+ -8.7006e-01*_Complex_I,   8.4877e-01+  2.3481e-01*_Complex_I
    };

    // test output
    float complex test[32] = {
     -1.3502e-01+ -8.0011e-02*_Complex_I,  -2.8193e-01+ -1.8485e-01*_Complex_I, 
      2.3712e-03+ -2.7745e-01*_Complex_I,   6.7633e-01+ -4.4225e-01*_Complex_I, 
      1.0738e+00+ -4.1529e-01*_Complex_I,   9.0674e-01+ -3.8673e-02*_Complex_I, 
      8.3711e-01+  4.1783e-01*_Complex_I,   1.1333e+00+  5.2178e-01*_Complex_I, 
      1.3371e+00+  3.4051e-01*_Complex_I,   1.2234e+00+  1.1527e-01*_Complex_I, 
      8.9430e-01+ -1.6482e-01*_Complex_I,   4.7580e-01+ -2.1271e-01*_Complex_I, 
      1.3515e-01+ -2.1749e-01*_Complex_I,   2.0595e-01+ -4.0877e-01*_Complex_I, 
      5.6671e-01+ -5.6776e-01*_Complex_I,   7.1073e-01+ -7.6691e-01*_Complex_I, 
      5.4996e-01+ -9.4074e-01*_Complex_I,   3.1669e-01+ -9.2989e-01*_Complex_I, 
      4.6673e-02+ -9.3759e-01*_Complex_I,  -2.6402e-01+ -8.7035e-01*_Complex_I, 
     -4.5243e-01+ -6.1914e-01*_Complex_I,  -4.8112e-01+ -3.9634e-01*_Complex_I, 
     -4.1536e-01+ -1.9823e-01*_Complex_I,  -3.9499e-01+ -4.9192e-02*_Complex_I, 
     -5.0709e-01+ -1.4351e-02*_Complex_I,  -7.6176e-01+ -2.1690e-01*_Complex_I, 
     -1.0673e+00+ -4.5202e-01*_Complex_I,  -1.0954e+00+ -2.3394e-01*_Complex_I, 
     -9.4506e-01+  4.2123e-01*_Complex_I,  -8.9589e-01+  1.1030e+00*_Complex_I, 
     -7.9757e-01+  1.2767e+00*_Complex_I,  -4.9320e-01+  8.3748e-01*_Complex_I
    };

    unsigned int i;
    float complex y;
    float tol=1e-4f;

    // filter noise signal, compare output
    for (i=0; i<32; i++) {
        iirfilt_crcf_execute(f, x[i], &y);

        CONTEND_DELTA( crealf(test[i]), crealf(y), tol );
        CONTEND_DELTA( cimagf(test[i]), cimagf(y), tol );
    }

    iirfilt_crcf_destroy(f);
}

