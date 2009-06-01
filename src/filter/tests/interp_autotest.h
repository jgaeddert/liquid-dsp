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

#ifndef __INTERP_AUTOTEST_H__
#define __INTERP_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: 
//
void autotest_interp_rrrf_generic()
{
    float h[] = {0, 0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25, 0}; 
    unsigned int h_len = 9; // filter length
    unsigned int M = 4;     // interp factor
    interp_rrrf q = interp_rrrf_create(4,h,h_len);

    float x[] = {1.0, -1.0, 1.0, 1.0};
    float y[16];
    float test[16] = {
         0.00,  0.25,  0.50,  0.75,
         1.00,  0.50,  0.00, -0.50,
        -1.00, -0.50,  0.00,  0.50,
         1.00,  1.00,  1.00,  1.00};
    float tol = 1e-6;

    unsigned int i, n=0;
    for (i=0; i<4; i++) {
        interp_rrrf_execute(q, x[i], &y[n]);
        n+=M;
    }   

    for (i=0; i<16; i++) {
        CONTEND_DELTA(y[i], test[i], tol);

        if (_autotest_verbose)
            printf("  y(%u) = %8.4f;\n", i+1, y[i]);
    }

    if (_autotest_verbose)
        interp_rrrf_print(q);

    interp_rrrf_destroy(q);
}

void autotest_interp_crcf_generic()
{
    // h = [0, 0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25, 0];
    float h[] = {0, 0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25, 0}; 
    unsigned int h_len = 9; // filter length
    unsigned int M = 4;     // interp factor
    interp_crcf q = interp_crcf_create(4,h,h_len);

    //  x = [1+j*0.2, -0.2+j*1.3, 0.5+j*0.3, 1.1-j*0.2]
    float complex x[4] = {
      1.0000e+00+  2.0000e-01*_Complex_I, 
     -2.0000e-01+  1.3000e+00*_Complex_I, 
      5.0000e-01+  3.0000e-01*_Complex_I, 
      1.1000e+00+ -2.0000e-01*_Complex_I
    };
        
    float complex y[16];

    // z = [x(1) 0 0 0 x(2) 0 0 0 x(3) 0 0 0 x(4) 0 0 0];
    // test = filter(h,1,z)
    float complex test[16] = {
      0.0000+  0.0000*_Complex_I, 
      0.2500+  0.0500*_Complex_I, 
      0.5000+  0.1000*_Complex_I, 
      0.7500+  0.1500*_Complex_I, 
      1.0000+  0.2000*_Complex_I, 
      0.7000+  0.4750*_Complex_I, 
      0.4000+  0.7500*_Complex_I, 
      0.1000+  1.0250*_Complex_I, 
     -0.2000+  1.3000*_Complex_I, 
     -0.0250+  1.0500*_Complex_I, 
      0.1500+  0.8000*_Complex_I, 
      0.3250+  0.5500*_Complex_I, 
      0.5000+  0.3000*_Complex_I, 
      0.6500+  0.1750*_Complex_I, 
      0.8000+  0.0500*_Complex_I, 
      0.9500+ -0.0750*_Complex_I
    };

    float tol = 1e-6;

    unsigned int i, n=0;
    for (i=0; i<4; i++) {
        interp_crcf_execute(q, x[i], &y[n]);
        n+=M;
    }   

    for (i=0; i<16; i++) {
        CONTEND_DELTA( crealf(y[i]), crealf(test[i]), tol);
        CONTEND_DELTA( cimagf(y[i]), cimagf(test[i]), tol);

        if (_autotest_verbose)
            printf("  y(%u) = %8.4f + j%8.4f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    if (_autotest_verbose)
        interp_crcf_print(q);

    interp_crcf_destroy(q);
}

#endif 

