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

//
// fft_prime_autotest.c : test FFTs of prime length
//

#include "autotest/autotest.h"
#include "liquid.h"

#include "fft_data.h"

// autotest helper function
//  _x      :   fft input array
//  _test   :   expected fft output
//  _n      :   fft size
void fft_prime_test(float complex * _x,
                    float complex * _test,
                    unsigned int    _n)
{
    int _method = 0;
    float tol=2e-4f;

    unsigned int i;

    float complex y[_n], z[_n];

    // compute FFT
    fftplan pf = fft_create_plan(_n, _x, y, FFT_FORWARD, _method);
    fft_execute(pf);

    // compute IFFT
    fftplan pr = fft_create_plan(_n, y, z, FFT_REVERSE, _method);
    fft_execute(pr);

    // normalize inverse
    for (i=0; i<_n; i++)
        z[i] /= (float) _n;

    // validate results
    float fft_error, ifft_error;
    for (i=0; i<_n; i++) {
        fft_error = cabsf( y[i] - _test[i] );
        ifft_error = cabsf( _x[i] - z[i] );
        CONTEND_DELTA( fft_error, 0, tol);
        CONTEND_DELTA( ifft_error, 0, tol);
    }

    // destroy plans
    fft_destroy_plan(pf);
    fft_destroy_plan(pr);
}


// 
// AUTOTESTS: n-point ffts
//
void autotest_fft_17()  { fft_prime_test(fft_test_x17,   fft_test_y17,   17); }
void autotest_fft_43()  { fft_prime_test(fft_test_x43,   fft_test_y43,   43); }
void autotest_fft_79()  { fft_prime_test(fft_test_x79,   fft_test_y79,   79); }
void autotest_fft_157() { fft_prime_test(fft_test_x157,  fft_test_y157, 157); }
void autotest_fft_317() { fft_prime_test(fft_test_x317,  fft_test_y317, 317); }
void autotest_fft_509() { fft_prime_test(fft_test_x509,  fft_test_y509, 509); }
