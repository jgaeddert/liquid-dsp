/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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

// autotest helper function
//  _x      :   fft input array
//  _test   :   expected fft output
//  _n      :   fft size
void fftq16_test(float complex * _x,
                 float complex * _test,
                 unsigned int    _n)
{
    int _method = 0;
    float tol   = 0.15f;    // error tolerance

    unsigned int i;

    cq16_t x[_n];
    cq16_t y[_n];

    // convert input to fixed-point and compute FFT
    for (i=0; i<_n; i++)
        x[i] = cq16_float_to_fixed(_x[i]);
    fftq16plan pf = fftq16_create_plan(_n, x, y, FFT_FORWARD, _method);
    fftq16_execute(pf);
    fftq16_destroy_plan(pf);

    // validate FFT results
    for (i=0; i<_n; i++) {
        float complex yf = cq16_fixed_to_float(y[i]);
        float error = cabsf( yf - _test[i] );

        printf("  %3u : %12.8f + j*%12.8f (expected %12.8f + j%12.8f), |e| = %12.8f\n",
                i,
                crealf(yf), cimagf(yf),
                crealf(_test[i]), cimagf(_test[i]),
                error);

        CONTEND_DELTA( error, 0, tol );
    }

#if 0
    // convert input to fixed-point and compute IFFT
    for (i=0; i<_n; i++)
        y[i] = cq16_float_to_fixed(_test[i]);
    fftq16plan pr = fftq16_create_plan(_n, y, x, FFT_REVERSE, _method);
    fftq16_execute(pr);
    fftq16_destroy_plan(pr);

    // validate IFFT results
    for (i=0; i<_n; i++) {
        float complex xf = cq16_fixed_to_float(x[i]);
        float error = cabsf( xf - _x[i] );

        printf("  %3u : %12.8f + j*%12.8f (expected %12.8f + j%12.8f), |e| = %12.8f\n",
                i,
                crealf(xf), cimagf(xf),
                crealf(_x[i]), cimagf(_x[i]),
                error);

        CONTEND_DELTA( error, 0, tol );
    }
#endif
}

