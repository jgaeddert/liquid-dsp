/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
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

#include "autotest/autotest.h"
#include "liquid.h"


void firfilt_crcq16_test(float *         _hf,
                         unsigned int    _h_len,
                         float complex * _xf,
                         unsigned int    _n,
                         float           _tol);

//
// AUTOTEST : random input
//
void autotest_firfilt_crcq16_noise()
{
    // error tolerance
    float tol = expf(-sqrtf(q16_fracbits));

    // Initialize variables
    float h[10] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

    // noise signal
    float complex x[32] = {
     -8.5985e-01+  1.9859e+00*_Complex_I,  -1.0981e+00+  7.3435e-01*_Complex_I, 
      1.9738e-02+  1.6488e-01*_Complex_I,   4.0861e-01+  9.8558e-01*_Complex_I, 
      8.8505e-01+  3.8529e-01*_Complex_I,  -7.3709e-01+ -1.2608e+00*_Complex_I, 
     -7.6302e-01+  1.4282e-01*_Complex_I,  -1.5708e+00+ -1.0774e+00*_Complex_I, 
     -4.7569e-02+ -1.0912e+00*_Complex_I,   1.3833e+00+  3.0085e-01*_Complex_I, 
      1.2652e+00+  1.7522e+00*_Complex_I,  -1.9119e-01+ -6.9241e-02*_Complex_I, 
      1.5155e+00+  1.2681e+00*_Complex_I,   1.1473e+00+ -1.1756e-02*_Complex_I, 
      1.0033e+00+ -1.2385e+00*_Complex_I,  -9.7532e-01+  4.4694e-02*_Complex_I, 
      1.4272e+00+  1.3811e+00*_Complex_I,  -5.2102e-02+ -1.4823e+00*_Complex_I, 
      6.5172e-01+  4.2771e-01*_Complex_I,  -2.9732e-01+  3.8132e-01*_Complex_I, 
      1.4363e-01+ -2.3562e-01*_Complex_I,  -4.1195e-01+  7.1421e-01*_Complex_I, 
     -1.5922e+00+  4.1696e-01*_Complex_I,   6.3603e-01+  1.4861e+00*_Complex_I, 
      1.1724e+00+ -1.8459e+00*_Complex_I,   1.7073e+00+ -1.6641e-01*_Complex_I, 
      2.8442e-01+ -6.1188e-01*_Complex_I,  -8.4867e-02+  1.0387e+00*_Complex_I, 
     -5.5189e-02+ -3.7222e-01*_Complex_I,   1.1913e+00+ -4.8040e-01*_Complex_I, 
     -1.0573e+00+ -2.1809e-01*_Complex_I,  -1.8163e-01+ -8.4464e-01*_Complex_I
    };
    
    // run test
    firfilt_crcq16_test(h, 10, x, 32, tol);
}

void firfilt_crcq16_test(float *         _hf,
                         unsigned int    _h_len,
                         float complex * _xf,
                         unsigned int    _n,
                         float           _tol)
{
    unsigned int i;

    // convert to fixed-point arrays, back to float (compensate for
    // roundoff error)
    q16_t  h[_h_len];
    float hf[_h_len];
    for (i=0; i<_h_len; i++) {
        h[i]  = q16_float_to_fixed(_hf[i]);
        hf[i] = q16_fixed_to_float(h[i]);
    }

    // convert input
    cq16_t x[_n];
    float complex xf[_n];
    for (i=0; i<_n; i++) {
        x[i]  = cq16_float_to_fixed(_xf[i]);
        xf[i] = cq16_fixed_to_float(x[i]);
    }

    // compute floating-point result
    float complex yf_test[_n];
    firfilt_crcf qf = firfilt_crcf_create(hf, _h_len);
    for (i=0; i<_n; i++) {
        firfilt_crcf_push(qf, xf[i]);
        firfilt_crcf_execute(qf, &yf_test[i]);
    }
    firfilt_crcf_destroy(qf);

    // compute fixed-point result
    cq16_t y[_n];
    float complex yf[_n];
    firfilt_crcq16 q = firfilt_crcq16_create(h, _h_len);
    for (i=0; i<_n; i++) {
        firfilt_crcq16_push(q, x[i]);
        firfilt_crcq16_execute(q, &y[i]);

        // convert to float
        yf[i] = cq16_fixed_to_float(y[i]);
    }
    firfilt_crcq16_destroy(q);

    if (liquid_autotest_verbose) {
        printf("testing dotprod_crcq16(%u)...\n", _n);
        for (i=0; i<_n; i++) {
            printf("  %3u : %8.4f+j%8.4f (expected %8.4f+j%8.4f)\n",
                    i,
                    crealf(yf[i]),      cimagf(yf[i]),
                    crealf(yf_test[i]), cimagf(yf_test[i]));
        }
    }

    // run checks
    for (i=0; i<_n; i++) {
        CONTEND_DELTA( crealf(yf[i]), crealf(yf_test[i]), _tol);
        CONTEND_DELTA( cimagf(yf[i]), cimagf(yf_test[i]), _tol);
    }

}

