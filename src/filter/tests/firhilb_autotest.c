/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#define J _Complex_I

//
// AUTOTEST: Hilbert transform, 2:1 decimator
//
void autotest_firhilbf_decim()
{
    float x[32] = {
         1.0000,  0.7071,  0.0000, -0.7071, -1.0000, -0.7071, -0.0000,  0.7071,
         1.0000,  0.7071,  0.0000, -0.7071, -1.0000, -0.7071, -0.0000,  0.7071,
         1.0000,  0.7071,  0.0000, -0.7071, -1.0000, -0.7071, -0.0000,  0.7071,
         1.0000,  0.7071, -0.0000, -0.7071, -1.0000, -0.7071, -0.0000,  0.7071
    };

    float complex test[16] = {
         0.0000+J*-0.0055,  0.0000+J*-0.0231,  0.0000+J*-0.0605,  0.0000+J*-0.1459,
         0.0000+J*-0.5604,  0.7071+J* 0.7669, -0.7071+J* 0.7294, -0.7071+J*-0.7008,
         0.7071+J*-0.7064,  0.7071+J* 0.7064, -0.7071+J* 0.7064, -0.7071+J*-0.7064,
         0.7071+J*-0.7064,  0.7071+J* 0.7064, -0.7071+J* 0.7064, -0.7071+J*-0.7064
    };

    float complex y[16];
    unsigned int m=5;   // h_len = 4*m+1 = 21
    firhilbf ht = firhilbf_create(m,60.0f);
    float tol=0.005f;

    // run decimator
    unsigned int i;
    for (i=0; i<16; i++)
        firhilbf_decim_execute(ht, &x[2*i], &y[i]);

    if (liquid_autotest_verbose) {
        printf("hilbert transform decimator output:\n");
        for (i=0; i<16; i++)
            printf("  y(%3u) = %8.5f + j*%8.5f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // run validation
    for (i=0; i<16; i++) {
        CONTEND_DELTA(crealf(y[i]), crealf(test[i]), tol);
        CONTEND_DELTA(cimagf(y[i]), cimagf(test[i]), tol);
    }

    // clean up filter object
    firhilbf_destroy(ht);
}

//
// AUTOTEST: Hilbert transform, 1:2 interpolator
//
void autotest_firhilbf_interp()
{
    float complex x[16] = {
         1.0000+J* 0.0000,  0.0000+J* 1.0000, -1.0000+J* 0.0000, -0.0000+J*-1.0000,
         1.0000+J*-0.0000,  0.0000+J* 1.0000, -1.0000+J* 0.0000, -0.0000+J*-1.0000,
         1.0000+J*-0.0000,  0.0000+J* 1.0000, -1.0000+J* 0.0000, -0.0000+J*-1.0000,
         1.0000+J*-0.0000, -0.0000+J* 1.0000, -1.0000+J* 0.0000, -0.0000+J*-1.0000
    };

    float test[32] = {
         0.0000, -0.0055, -0.0000, -0.0231, -0.0000, -0.0605, -0.0000, -0.1459,
        -0.0000, -0.5604, -0.0000,  0.7669,  1.0000,  0.7294,  0.0000, -0.7008,
        -1.0000, -0.7064, -0.0000,  0.7064,  1.0000,  0.7064,  0.0000, -0.7064,
        -1.0000, -0.7064, -0.0000,  0.7064,  1.0000,  0.7064,  0.0000, -0.7064
    };


    float y[32];
    unsigned int m=5;   // h_len = 4*m+1 = 21
    firhilbf ht = firhilbf_create(m,60.0f);
    float tol=0.005f;

    // run interpolator
    unsigned int i;
    for (i=0; i<16; i++)
        firhilbf_interp_execute(ht, x[i], &y[2*i]);

    if (liquid_autotest_verbose) {
        printf("hilbert transform interpolator output:\n");
        for (i=0; i<32; i++)
            printf("  y(%3u) = %8.5f;\n", i+1, y[i]);
    }

    // run validation
    for (i=0; i<32; i++) {
        CONTEND_DELTA(y[i], test[i], tol);
    }

    // clean up filter object
    firhilbf_destroy(ht);
}

