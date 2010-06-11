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

#ifndef __LIQUID_FFT_R2R_AUTOTEST_H__
#define __LIQUID_FFT_R2R_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// include data sets
#include "fft_r2r_data.h"

// autotest helper function
void fft_r2r_test(float *_x,
                  float *_test,
                  unsigned int _n,
                  unsigned int _kind)
{
    int _flags = 0;
    float tol=1e-4f;

    unsigned int i;

    float y[_n];

    // compute real even/odd FFT
    fftplan q = fft_create_plan_r2r_1d(_n, _x, y, _kind, _flags);
    fft_execute(q);

    // validate results
    for (i=0; i<_n; i++)
        CONTEND_DELTA( y[i], _test[i], tol);

    // destroy plans
    fft_destroy_plan(q);
}


// 
// AUTOTESTS: n-point real-to-real ffts
//
void xautotest_fft_r2r_REDFT00_n8()  { fft_r2r_test(fftdata_r2r_n8, fftdata_r2r_REDFT00_n8, 8, FFT_REDFT00); }

#endif 

