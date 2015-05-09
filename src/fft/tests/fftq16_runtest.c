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

#include <stdlib.h>
#include <stdio.h>

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

    // allocate memory for arrays
    cq16_t * x = (cq16_t*) malloc( _n*sizeof(cq16_t) );
    cq16_t * y = (cq16_t*) malloc( _n*sizeof(cq16_t) );

    // convert input to fixed-point and compute FFT
    if (liquid_autotest_verbose)
        printf("running %u-point fft...\n", _n);
    for (i=0; i<_n; i++)
        x[i] = cq16_float_to_fixed(_x[i]);
    fftq16plan pf = fftq16_create_plan(_n, x, y, LIQUID_FFT_FORWARD, _method);
    fftq16_execute(pf);
    fftq16_destroy_plan(pf);

    // validate FFT results
    for (i=0; i<_n; i++) {
        float complex yf = cq16_fixed_to_float(y[i]);
        float error = cabsf( yf - _test[i] );

        if (liquid_autotest_verbose) {
            printf("  %3u : (%10.6f, %10.6f), expected (%10.6f, %10.6f), |e| = %12.8f\n",
                    i,
                    crealf(yf), cimagf(yf),
                    crealf(_test[i]), cimagf(_test[i]),
                    error);
        }

        CONTEND_DELTA( error, 0, tol );
    }

    // convert input to fixed-point and compute IFFT
    if (liquid_autotest_verbose)
        printf("running %u-point ifft...\n", _n);
    for (i=0; i<_n; i++)
        y[i] = cq16_float_to_fixed(_test[i]);
    fftq16plan pr = fftq16_create_plan(_n, y, x, LIQUID_FFT_BACKWARD, _method);
    fftq16_execute(pr);
    fftq16_destroy_plan(pr);

    // validate IFFT results
    for (i=0; i<_n; i++) {
        float complex xf = cq16_fixed_to_float(x[i]);
        float error = cabsf( xf - _x[i] );

        if (liquid_autotest_verbose) {
            printf("  %3u : (%10.6f, %10.6f), expected (%10.6f, %10.6f), |e| = %12.8f\n",
                i,
                crealf(xf), cimagf(xf),
                crealf(_x[i]), cimagf(_x[i]),
                error);
        }

        CONTEND_DELTA( error, 0, tol );
    }

    // free allocated memory
    free(x);
    free(y);
}

