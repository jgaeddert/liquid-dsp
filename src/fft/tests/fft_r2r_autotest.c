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

// include data sets
#include "fft_runtest.h"

// autotest helper function
void fft_r2r_test(float *      _x,
                  float *      _test,
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

    // print results
    if (liquid_autotest_verbose) {
        printf("%12s %12s\n", "expected", "actual");
        for (i=0; i<_n; i++)
            printf("%12.8f %12.8f\n", _test[i], y[i]);
    }

    // validate results
    for (i=0; i<_n; i++)
        CONTEND_DELTA( y[i], _test[i], tol);

    // destroy plans
    fft_destroy_plan(q);
}


// 
// AUTOTESTS: 8-point real-to-real ffts
//

void autotest_fft_r2r_REDFT00_n8()  { fft_r2r_test(fftdata_r2r_x8, fftdata_r2r_REDFT00_y8, 8, LIQUID_FFT_REDFT00); }
void autotest_fft_r2r_REDFT10_n8()  { fft_r2r_test(fftdata_r2r_x8, fftdata_r2r_REDFT10_y8, 8, LIQUID_FFT_REDFT10); }
void autotest_fft_r2r_REDFT01_n8()  { fft_r2r_test(fftdata_r2r_x8, fftdata_r2r_REDFT01_y8, 8, LIQUID_FFT_REDFT01); }
void autotest_fft_r2r_REDFT11_n8()  { fft_r2r_test(fftdata_r2r_x8, fftdata_r2r_REDFT11_y8, 8, LIQUID_FFT_REDFT11); }

void autotest_fft_r2r_RODFT00_n8()  { fft_r2r_test(fftdata_r2r_x8, fftdata_r2r_RODFT00_y8, 8, LIQUID_FFT_RODFT00); }
void autotest_fft_r2r_RODFT10_n8()  { fft_r2r_test(fftdata_r2r_x8, fftdata_r2r_RODFT10_y8, 8, LIQUID_FFT_RODFT10); }
void autotest_fft_r2r_RODFT01_n8()  { fft_r2r_test(fftdata_r2r_x8, fftdata_r2r_RODFT01_y8, 8, LIQUID_FFT_RODFT01); }
void autotest_fft_r2r_RODFT11_n8()  { fft_r2r_test(fftdata_r2r_x8, fftdata_r2r_RODFT11_y8, 8, LIQUID_FFT_RODFT11); }


// 
// AUTOTESTS: 32-point real-to-real ffts
//

void autotest_fft_r2r_REDFT00_n32()  { fft_r2r_test(fftdata_r2r_x32, fftdata_r2r_REDFT00_y32, 32, LIQUID_FFT_REDFT00); }
void autotest_fft_r2r_REDFT10_n32()  { fft_r2r_test(fftdata_r2r_x32, fftdata_r2r_REDFT10_y32, 32, LIQUID_FFT_REDFT10); }
void autotest_fft_r2r_REDFT01_n32()  { fft_r2r_test(fftdata_r2r_x32, fftdata_r2r_REDFT01_y32, 32, LIQUID_FFT_REDFT01); }
void autotest_fft_r2r_REDFT11_n32()  { fft_r2r_test(fftdata_r2r_x32, fftdata_r2r_REDFT11_y32, 32, LIQUID_FFT_REDFT11); }

void autotest_fft_r2r_RODFT00_n32()  { fft_r2r_test(fftdata_r2r_x32, fftdata_r2r_RODFT00_y32, 32, LIQUID_FFT_RODFT00); }
void autotest_fft_r2r_RODFT10_n32()  { fft_r2r_test(fftdata_r2r_x32, fftdata_r2r_RODFT10_y32, 32, LIQUID_FFT_RODFT10); }
void autotest_fft_r2r_RODFT01_n32()  { fft_r2r_test(fftdata_r2r_x32, fftdata_r2r_RODFT01_y32, 32, LIQUID_FFT_RODFT01); }
void autotest_fft_r2r_RODFT11_n32()  { fft_r2r_test(fftdata_r2r_x32, fftdata_r2r_RODFT11_y32, 32, LIQUID_FFT_RODFT11); }


// 
// AUTOTESTS: 27-point real-to-real ffts
//

void autotest_fft_r2r_REDFT00_n27()  { fft_r2r_test(fftdata_r2r_x27, fftdata_r2r_REDFT00_y27, 27, LIQUID_FFT_REDFT00); }
void autotest_fft_r2r_REDFT10_n27()  { fft_r2r_test(fftdata_r2r_x27, fftdata_r2r_REDFT10_y27, 27, LIQUID_FFT_REDFT10); }
void autotest_fft_r2r_REDFT01_n27()  { fft_r2r_test(fftdata_r2r_x27, fftdata_r2r_REDFT01_y27, 27, LIQUID_FFT_REDFT01); }
void autotest_fft_r2r_REDFT11_n27()  { fft_r2r_test(fftdata_r2r_x27, fftdata_r2r_REDFT11_y27, 27, LIQUID_FFT_REDFT11); }

void autotest_fft_r2r_RODFT00_n27()  { fft_r2r_test(fftdata_r2r_x27, fftdata_r2r_RODFT00_y27, 27, LIQUID_FFT_RODFT00); }
void autotest_fft_r2r_RODFT10_n27()  { fft_r2r_test(fftdata_r2r_x27, fftdata_r2r_RODFT10_y27, 27, LIQUID_FFT_RODFT10); }
void autotest_fft_r2r_RODFT01_n27()  { fft_r2r_test(fftdata_r2r_x27, fftdata_r2r_RODFT01_y27, 27, LIQUID_FFT_RODFT01); }
void autotest_fft_r2r_RODFT11_n27()  { fft_r2r_test(fftdata_r2r_x27, fftdata_r2r_RODFT11_y27, 27, LIQUID_FFT_RODFT11); }

