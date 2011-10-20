/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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

// autotest data
#include "fft_data.h"

// autotest helper function
//  _x      :   fft input array
//  _test   :   expected fft output
//  _n      :   fft size
void fft_test(float complex *_x,
              float complex *_test,
              unsigned int _n)
{
    int _method = 0;
    float tol=1e-4f;

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
void autotest_fft_4()       { fft_test(x4,      test4,      4);     }
void autotest_fft_5()       { fft_test(x5,      test5,      5);     }
void autotest_fft_6()       { fft_test(x6,      test6,      6);     }
void autotest_fft_7()       { fft_test(x7,      test7,      7);     }
void autotest_fft_8()       { fft_test(x8,      test8,      8);     }
void autotest_fft_9()       { fft_test(x9,      test9,      9);     }
void autotest_fft_10()      { fft_test(x10,     test10,     10);    }
void autotest_fft_16()      { fft_test(x16,     test16,     16);    }
void autotest_fft_20()      { fft_test(x20,     test20,     20);    }
void autotest_fft_32()      { fft_test(x32,     test32,     32);    }
void autotest_fft_64()      { fft_test(x64,     test64,     64);    }

