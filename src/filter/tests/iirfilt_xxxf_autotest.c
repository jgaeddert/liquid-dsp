/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

//
// iirfilt_xxxf_autotest.c : test floating-point filters
//

#include "liquid.autotest.h"
#include "liquid.h"

// rrrf
extern float         iirfilt_rrrf_data_h3x64_b[];
extern float         iirfilt_rrrf_data_h3x64_a[];
extern float         iirfilt_rrrf_data_h3x64_x[];
extern float         iirfilt_rrrf_data_h3x64_y[];

extern float         iirfilt_rrrf_data_h5x64_b[];
extern float         iirfilt_rrrf_data_h5x64_a[];
extern float         iirfilt_rrrf_data_h5x64_x[];
extern float         iirfilt_rrrf_data_h5x64_y[];

extern float         iirfilt_rrrf_data_h7x64_b[];
extern float         iirfilt_rrrf_data_h7x64_a[];
extern float         iirfilt_rrrf_data_h7x64_x[];
extern float         iirfilt_rrrf_data_h7x64_y[];

// crcf
extern float         iirfilt_crcf_data_h3x64_b[];
extern float         iirfilt_crcf_data_h3x64_a[];
extern float complex iirfilt_crcf_data_h3x64_x[];
extern float complex iirfilt_crcf_data_h3x64_y[];

extern float         iirfilt_crcf_data_h5x64_b[];
extern float         iirfilt_crcf_data_h5x64_a[];
extern float complex iirfilt_crcf_data_h5x64_x[];
extern float complex iirfilt_crcf_data_h5x64_y[];

extern float         iirfilt_crcf_data_h7x64_b[];
extern float         iirfilt_crcf_data_h7x64_a[];
extern float complex iirfilt_crcf_data_h7x64_x[];
extern float complex iirfilt_crcf_data_h7x64_y[];

// cccf
extern float complex iirfilt_cccf_data_h3x64_b[];
extern float complex iirfilt_cccf_data_h3x64_a[];
extern float complex iirfilt_cccf_data_h3x64_x[];
extern float complex iirfilt_cccf_data_h3x64_y[];

extern float complex iirfilt_cccf_data_h5x64_b[];
extern float complex iirfilt_cccf_data_h5x64_a[];
extern float complex iirfilt_cccf_data_h5x64_x[];
extern float complex iirfilt_cccf_data_h5x64_y[];

extern float complex iirfilt_cccf_data_h7x64_b[];
extern float complex iirfilt_cccf_data_h7x64_a[];
extern float complex iirfilt_cccf_data_h7x64_x[];
extern float complex iirfilt_cccf_data_h7x64_y[];

// autotest helper function
//  _b      :   filter coefficients (numerator)
//  _a      :   filter coefficients (denominator)
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void iirfilt_rrrf_test(liquid_autotest __q__,
                       float *      _b,
                       float *      _a,
                       unsigned int _h_len,
                       float *      _x,
                       unsigned int _x_len,
                       float *      _y,
                       unsigned int _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    iirfilt_rrrf q = iirfilt_rrrf_create(_b, _h_len, _a, _h_len);

    // allocate memory for output
    float y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_x_len; i++) {
        iirfilt_rrrf_execute(q, _x[i], &y_test[i]);
        
        LIQUID_CHECK_DELTA( y_test[i], _y[i], tol );
    }

    // destroy filter object
    iirfilt_rrrf_destroy(q);
}

// autotest helper function
//  _b      :   filter coefficients (numerator)
//  _a      :   filter coefficients (denominator)
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void iirfilt_crcf_test(liquid_autotest __q__,
                       float *         _b,
                       float *         _a,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    iirfilt_crcf q = iirfilt_crcf_create(_b, _h_len, _a, _h_len);

    // allocate memory for output
    float complex y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_x_len; i++) {
        iirfilt_crcf_execute(q, _x[i], &y_test[i]);
        
        LIQUID_CHECK_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    iirfilt_crcf_destroy(q);
}

// autotest helper function
//  _b      :   filter coefficients (numerator)
//  _a      :   filter coefficients (denominator)
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void iirfilt_cccf_test(liquid_autotest __q__,
                       float complex * _b,
                       float complex * _a,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    iirfilt_cccf q = iirfilt_cccf_create(_b, _h_len, _a, _h_len);

    // allocate memory for output
    float complex y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_x_len; i++) {
        iirfilt_cccf_execute(q, _x[i], &y_test[i]);
        
        LIQUID_CHECK_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    iirfilt_cccf_destroy(q);
}


// 
// AUTOTEST: iirfilt_rrrf tests
//
LIQUID_AUTOTEST(iirfilt_rrrf_h3x64,"description","",0.1)
{
    iirfilt_rrrf_test(__q__,
                      iirfilt_rrrf_data_h3x64_b,
                      iirfilt_rrrf_data_h3x64_a, 3,
                      iirfilt_rrrf_data_h3x64_x, 64,
                      iirfilt_rrrf_data_h3x64_y, 64);
}

LIQUID_AUTOTEST(iirfilt_rrrf_h5x64,"description","",0.1)
{
    iirfilt_rrrf_test(__q__,
                      iirfilt_rrrf_data_h5x64_b,
                      iirfilt_rrrf_data_h5x64_a, 5,
                      iirfilt_rrrf_data_h5x64_x, 64,
                      iirfilt_rrrf_data_h5x64_y, 64);
}
LIQUID_AUTOTEST(iirfilt_rrrf_h7x64,"description","",0.1)
{
    iirfilt_rrrf_test(__q__,
                      iirfilt_rrrf_data_h7x64_b,
                      iirfilt_rrrf_data_h7x64_a, 7,
                      iirfilt_rrrf_data_h7x64_x, 64,
                      iirfilt_rrrf_data_h7x64_y, 64);
}


// 
// AUTOTEST: iirfilt_crcf tests
//
LIQUID_AUTOTEST(iirfilt_crcf_h3x64,"description","",0.1)
{
    iirfilt_crcf_test(__q__,
                      iirfilt_crcf_data_h3x64_b,
                      iirfilt_crcf_data_h3x64_a, 3,
                      iirfilt_crcf_data_h3x64_x, 64,
                      iirfilt_crcf_data_h3x64_y, 64);
}
LIQUID_AUTOTEST(iirfilt_crcf_h5x64,"description","",0.1)
{
    iirfilt_crcf_test(__q__,
                      iirfilt_crcf_data_h5x64_b,
                      iirfilt_crcf_data_h5x64_a, 5,
                      iirfilt_crcf_data_h5x64_x, 64,
                      iirfilt_crcf_data_h5x64_y, 64);
}
LIQUID_AUTOTEST(iirfilt_crcf_h7x64,"description","",0.1)
{
    iirfilt_crcf_test(__q__,
                      iirfilt_crcf_data_h7x64_b,
                      iirfilt_crcf_data_h7x64_a, 7,
                      iirfilt_crcf_data_h7x64_x, 64,
                      iirfilt_crcf_data_h7x64_y, 64);
}


// 
// AUTOTEST: iirfilt_cccf tests
//
LIQUID_AUTOTEST(iirfilt_cccf_h3x64,"description","",0.1)
{
    iirfilt_cccf_test(__q__,
                      iirfilt_cccf_data_h3x64_b,
                      iirfilt_cccf_data_h3x64_a, 3,
                      iirfilt_cccf_data_h3x64_x, 64,
                      iirfilt_cccf_data_h3x64_y, 64);
}
LIQUID_AUTOTEST(iirfilt_cccf_h5x64,"description","",0.1)
{
    iirfilt_cccf_test(__q__,
                      iirfilt_cccf_data_h5x64_b,
                      iirfilt_cccf_data_h5x64_a, 5,
                      iirfilt_cccf_data_h5x64_x, 64,
                      iirfilt_cccf_data_h5x64_y, 64);
}
LIQUID_AUTOTEST(iirfilt_cccf_h7x64,"description","",0.1)
{
    iirfilt_cccf_test(__q__,
                      iirfilt_cccf_data_h7x64_b,
                      iirfilt_cccf_data_h7x64_a, 7,
                      iirfilt_cccf_data_h7x64_x, 64,
                      iirfilt_cccf_data_h7x64_y, 64);
}


