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

// autotest data included in "fft_data.h"
extern float complex x4[];   extern float complex test4[];
extern float complex x5[];   extern float complex test5[];
extern float complex x6[];   extern float complex test6[];
extern float complex x7[];   extern float complex test7[];
extern float complex x8[];   extern float complex test8[];
extern float complex x9[];   extern float complex test9[];
extern float complex x16[];  extern float complex test16[];
extern float complex x20[];  extern float complex test20[];
extern float complex x32[];  extern float complex test32[];
extern float complex x64[];  extern float complex test64[];

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


// 
// AUTOTESTS: n-point FFTs
//
void autotest_fftq16_4()       { fftq16_test(x4,      test4,      4);     }
void autotest_fftq16_5()       { fftq16_test(x5,      test5,      5);     }
void autotest_fftq16_6()       { fftq16_test(x6,      test6,      6);     }
void autotest_fftq16_7()       { fftq16_test(x7,      test7,      7);     }
void autotest_fftq16_8()       { fftq16_test(x8,      test8,      8);     }
void autotest_fftq16_9()       { fftq16_test(x9,      test9,      9);     }
void autotest_fftq16_16()      { fftq16_test(x16,     test16,     16);    }
void autotest_fftq16_20()      { fftq16_test(x20,     test20,     20);    }
void autotest_fftq16_32()      { fftq16_test(x32,     test32,     32);    }
void autotest_fftq16_64()      { fftq16_test(x64,     test64,     64);    }

