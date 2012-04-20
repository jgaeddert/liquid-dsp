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
// fft_composite_autotest.c : test FFTs of 'composite' length (not
//   prime, not of form 2^m)
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data for composite transforms
#include "fft_data_10.h"
#include "fft_data_21.h"
#include "fft_data_22.h"
#include "fft_data_24.h"
#include "fft_data_26.h"
#include "fft_data_30.h"
#include "fft_data_35.h"
#include "fft_data_36.h"
#include "fft_data_48.h"
#include "fft_data_63.h"
#include "fft_data_92.h"
#include "fft_data_96.h"

#include "fft_data_120.h"
#include "fft_data_130.h"
#include "fft_data_192.h"

// autotest helper function
//  _x      :   fft input array
//  _test   :   expected fft output
//  _n      :   fft size
void fft_composite_test(float complex * _x,
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
void autotest_fft_10()  { fft_composite_test(fft_test_x10,   fft_test_y10,   10); }
void autotest_fft_21()  { fft_composite_test(fft_test_x21,   fft_test_y21,   21); }
void autotest_fft_22()  { fft_composite_test(fft_test_x22,   fft_test_y22,   22); }
void autotest_fft_24()  { fft_composite_test(fft_test_x24,   fft_test_y24,   24); }
void autotest_fft_26()  { fft_composite_test(fft_test_x26,   fft_test_y26,   26); }
void autotest_fft_30()  { fft_composite_test(fft_test_x30,   fft_test_y30,   30); }
void autotest_fft_35()  { fft_composite_test(fft_test_x35,   fft_test_y35,   35); }
void autotest_fft_36()  { fft_composite_test(fft_test_x36,   fft_test_y36,   36); }
void autotest_fft_48()  { fft_composite_test(fft_test_x48,   fft_test_y48,   48); }
void autotest_fft_63()  { fft_composite_test(fft_test_x63,   fft_test_y63,   63); }
void autotest_fft_92()  { fft_composite_test(fft_test_x92,   fft_test_y92,   92); }
void autotest_fft_96()  { fft_composite_test(fft_test_x96,   fft_test_y96,   96); }

void autotest_fft_120() { fft_composite_test(fft_test_x120,  fft_test_y120, 120); }
void autotest_fft_130() { fft_composite_test(fft_test_x130,  fft_test_y130, 130); }
void autotest_fft_192() { fft_composite_test(fft_test_x192,  fft_test_y192, 192); }

