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

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.internal.h"

// helper function prototypes
void dotprod_rrrq32_test(float *      _hf,
                         float *      _xf,
                         unsigned int _n,
                         float        _tol);

void dotprod_crcq32_test(float *         _hf,
                         float complex * _xf,
                         unsigned int    _n,
                         float           _tol);

// 
// AUTOTEST: basic dot product (rrr)
//
void autotest_dotprod_rrrq32_basic()
{
    float tol = expf(-sqrtf(q32_fracbits));

    float hf[16] = {
    -0.050565, -0.952580,  0.274320,  1.232400, 
     1.268200,  0.565770,  0.800830,  0.923970, 
     0.517060, -0.530340, -0.378550, -1.127100, 
     1.123100, -1.006000, -1.483800, -0.062007
    };

    float xf[16] = {
    -0.384280, -0.812030,  0.156930,  1.919500, 
     0.564580, -0.123610, -0.138640,  0.004984, 
    -1.100200, -0.497620,  0.089977, -1.745500, 
     0.463640,  0.592100,  1.150000, -1.225400
    };

    // run check
    dotprod_rrrq32_test(hf, xf, 16, tol);
}

// 
// AUTOTEST: basic dot product (crc)
//
void autotest_dotprod_crcq32_basic()
{
    float tol = expf(-sqrtf(q32_fracbits));

    float hf[16] = {
      5.5375e-02,  -6.5857e-01,  -1.7657e+00,   7.7444e-01, 
      8.0730e-01,  -5.1340e-01,  -9.3437e-02,  -5.6301e-01, 
     -6.6480e-01,  -2.1673e+00,   9.0269e-01,   3.5284e+00, 
     -9.7835e-01,  -6.9512e-01,  -1.2958e+00,   1.1628e+00
    };

    float complex xf[16] = {
      1.3164e+00+  5.4161e-01*_Complex_I,   1.8295e-01+ -9.0284e-02*_Complex_I, 
      1.3487e+00+ -1.8148e+00*_Complex_I,  -7.4696e-01+ -4.1792e-01*_Complex_I, 
     -9.0551e-01+ -4.4294e-01*_Complex_I,   6.0591e-01+ -1.5383e+00*_Complex_I, 
     -7.5393e-01+ -3.5691e-01*_Complex_I,  -4.5733e-01+  1.1926e-01*_Complex_I, 
     -1.4744e-01+ -4.7676e-02*_Complex_I,  -1.2422e+00+ -2.0213e+00*_Complex_I, 
      3.3208e-02+ -1.3756e+00*_Complex_I,  -4.8573e-01+  1.0977e+00*_Complex_I, 
      1.5053e+00+  2.1141e-01*_Complex_I,  -8.4062e-01+ -1.0211e+00*_Complex_I, 
     -1.3932e+00+ -4.8491e-01*_Complex_I,  -1.4234e+00+  2.0333e-01*_Complex_I
    };


    // run check
    dotprod_crcq32_test(hf, xf, 16, tol);
}

// 
// helper functions to keep code base small
//
void dotprod_rrrq32_test(float *      _hf,
                         float *      _xf,
                         unsigned int _n,
                         float        _tol)
{
    unsigned int i;

    // compute floating point result
    float yf_test;
    dotprod_rrrf_run(_hf, _xf, _n, &yf_test);

    // convert to fixed-point arrays
    q32_t h[_n];
    q32_t x[_n];
    for (i=0; i<_n; i++) {
        h[i] = q32_float_to_fixed(_hf[i]);
        x[i] = q32_float_to_fixed(_xf[i]);
    }

    // compute fixed-point result using dotprod_rrrq32_run()
    q32_t y0;
    dotprod_rrrq32_run(h, x, _n, &y0);
    float y0f = q32_fixed_to_float(y0);

    // compute fixed-point result using dotprod_rrrq32_run4()
    q32_t y1;
    dotprod_rrrq32_run(h, x, _n, &y1);
    float y1f = q32_fixed_to_float(y1);

    // compute fixed-point result using dotprod_rrrq32 object
    q32_t y2;
    dotprod_rrrq32 q = dotprod_rrrq32_create(h,_n);
    dotprod_rrrq32_execute(q, x, &y2);
    dotprod_rrrq32_destroy(q);
    float y2f = q32_fixed_to_float(y2);

    if (liquid_autotest_verbose) {
        printf("  dotprod_rrrq32_run    : %12.8f (expected %12.8f), e=%12.8f, tol=%12.8f\n", y0f, yf_test, y0f-yf_test, _tol);
        printf("  dotprod_rrrq32_run4   : %12.8f (expected %12.8f), e=%12.8f, tol=%12.8f\n", y1f, yf_test, y1f-yf_test, _tol);
        printf("  dotprod_rrrq32 object : %12.8f (expected %12.8f), e=%12.8f, tol=%12.8f\n", y2f, yf_test, y2f-yf_test, _tol);
    }

    // run checks
    CONTEND_DELTA(y0f, yf_test, _tol);
    CONTEND_DELTA(y1f, yf_test, _tol);
    CONTEND_DELTA(y2f, yf_test, _tol);
}

void dotprod_crcq32_test(float *         _hf,
                         float complex * _xf,
                         unsigned int    _n,
                         float           _tol)
{
    unsigned int i;

    // compute floating point result
    float complex yf_test;
    dotprod_crcf_run(_hf, _xf, _n, &yf_test);

    // convert to fixed-point arrays
    q32_t h[_n];
    cq32_t x[_n];
    for (i=0; i<_n; i++) {
        h[i] =  q32_float_to_fixed(_hf[i]);
        x[i] = cq32_float_to_fixed(_xf[i]);
    }

    // compute fixed-point result using dotprod_rrrq32_run()
    cq32_t y0;
    dotprod_crcq32_run(h, x, _n, &y0);
    float complex y0f = cq32_fixed_to_float(y0);

    // compute fixed-point result using dotprod_rrrq32_run4()
    cq32_t y1;
    dotprod_crcq32_run(h, x, _n, &y1);
    float complex y1f = cq32_fixed_to_float(y1);

    // compute fixed-point result using dotprod_rrrq32 object
    cq32_t y2;
    dotprod_crcq32 q = dotprod_crcq32_create(h,_n);
    dotprod_crcq32_execute(q, x, &y2);
    dotprod_crcq32_destroy(q);
    float complex y2f = cq32_fixed_to_float(y2);

    if (liquid_autotest_verbose) {
        printf("  dotprod_crcq32_run    : %8.4f+j%8.4f (expected %8.4f+j%8.4f)\n", crealf(y0f), cimagf(y0f), crealf(yf_test), cimagf(yf_test));
        printf("  dotprod_crcq32_run4   : %8.4f+j%8.4f (expected %8.4f+j%8.4f)\n", crealf(y1f), cimagf(y1f), crealf(yf_test), cimagf(yf_test));
        printf("  dotprod_crcq32 object : %8.4f+j%8.4f (expected %8.4f+j%8.4f)\n", crealf(y2f), cimagf(y2f), crealf(yf_test), cimagf(yf_test));
    }

    // run checks
    CONTEND_DELTA( crealf(y0f), crealf(yf_test), _tol);
    CONTEND_DELTA( cimagf(y0f), cimagf(yf_test), _tol);

    CONTEND_DELTA( crealf(y1f), crealf(yf_test), _tol);
    CONTEND_DELTA( cimagf(y1f), cimagf(yf_test), _tol);

    CONTEND_DELTA( crealf(y2f), crealf(yf_test), _tol);
    CONTEND_DELTA( cimagf(y2f), cimagf(yf_test), _tol);
}

