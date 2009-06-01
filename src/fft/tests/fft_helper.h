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

#ifndef __FFT_AUTOTEST_HELPER_H__
#define __FFT_AUTOTEST_HELPER_H__

#include "liquid.h"

// autotest helper function
void fft_test(float complex *_x, float complex *_test, unsigned int _n)
{
    float tol=1e-4f;

    unsigned int i;

    float complex y[_n], z[_n];

    // compute FFT
    fftplan pf = fft_create_plan(_n, _x, y, FFT_FORWARD);
    fft_execute(pf);

    // compute IFFT
    fftplan pr = fft_create_plan(_n, y, z, FFT_REVERSE);
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

#endif // __FFT_AUTOTEST_HELPER_H__

