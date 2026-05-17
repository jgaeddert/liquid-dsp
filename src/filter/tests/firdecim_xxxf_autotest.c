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
// firdecim_xxxf_autotest.c : test floating-point decimators
//

#include "liquid.autotest.h"
#include "liquid.h"

// rrrf
extern float         firdecim_rrrf_data_M2h4x20_h[];
extern float         firdecim_rrrf_data_M2h4x20_x[];
extern float         firdecim_rrrf_data_M2h4x20_y[];

extern float         firdecim_rrrf_data_M3h7x30_h[];
extern float         firdecim_rrrf_data_M3h7x30_x[];
extern float         firdecim_rrrf_data_M3h7x30_y[];

extern float         firdecim_rrrf_data_M4h13x40_h[];
extern float         firdecim_rrrf_data_M4h13x40_x[];
extern float         firdecim_rrrf_data_M4h13x40_y[];

extern float         firdecim_rrrf_data_M5h23x50_h[];
extern float         firdecim_rrrf_data_M5h23x50_x[];
extern float         firdecim_rrrf_data_M5h23x50_y[];

// crcf
extern float         firdecim_crcf_data_M2h4x20_h[];
extern float complex firdecim_crcf_data_M2h4x20_x[];
extern float complex firdecim_crcf_data_M2h4x20_y[];

extern float         firdecim_crcf_data_M3h7x30_h[];
extern float complex firdecim_crcf_data_M3h7x30_x[];
extern float complex firdecim_crcf_data_M3h7x30_y[];

extern float         firdecim_crcf_data_M4h13x40_h[];
extern float complex firdecim_crcf_data_M4h13x40_x[];
extern float complex firdecim_crcf_data_M4h13x40_y[];

extern float         firdecim_crcf_data_M5h23x50_h[];
extern float complex firdecim_crcf_data_M5h23x50_x[];
extern float complex firdecim_crcf_data_M5h23x50_y[];

// cccf
extern float complex firdecim_cccf_data_M2h4x20_h[];
extern float complex firdecim_cccf_data_M2h4x20_x[];
extern float complex firdecim_cccf_data_M2h4x20_y[];

extern float complex firdecim_cccf_data_M3h7x30_h[];
extern float complex firdecim_cccf_data_M3h7x30_x[];
extern float complex firdecim_cccf_data_M3h7x30_y[];

extern float complex firdecim_cccf_data_M4h13x40_h[];
extern float complex firdecim_cccf_data_M4h13x40_x[];
extern float complex firdecim_cccf_data_M4h13x40_y[];

extern float complex firdecim_cccf_data_M5h23x50_h[];
extern float complex firdecim_cccf_data_M5h23x50_x[];
extern float complex firdecim_cccf_data_M5h23x50_y[];

// autotest helper function
//  _M      :   decimation rate
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firdecim_rrrf_test(liquid_autotest __q__,
                        unsigned int    _M,
                        float *         _h,
                        unsigned int    _h_len,
                        float *         _x,
                        unsigned int    _x_len,
                        float *         _y,
                        unsigned int    _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    firdecim_rrrf q = firdecim_rrrf_create(_M, _h, _h_len);

    // allocate memory for output
    float y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_y_len; i++) {
        firdecim_rrrf_execute(q, &_x[_M*i], &y_test[i]);
        
        LIQUID_CHECK_DELTA( y_test[i], _y[i], tol );
    }
    
    // destroy decimator object object
    firdecim_rrrf_destroy(q);
}

// autotest helper function
//  _M      :   decimation rate
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firdecim_crcf_test(liquid_autotest __q__,
                        unsigned int    _M,
                        float *         _h,
                        unsigned int    _h_len,
                        float complex * _x,
                        unsigned int    _x_len,
                        float complex * _y,
                        unsigned int    _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    firdecim_crcf q = firdecim_crcf_create(_M, _h, _h_len);

    // allocate memory for output
    float complex y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_y_len; i++) {
        firdecim_crcf_execute(q, &_x[_M*i], &y_test[i]);
        
        LIQUID_CHECK_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy decimator object object
    firdecim_crcf_destroy(q);
}

// autotest helper function
//  _M      :   decimation rate
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firdecim_cccf_test(liquid_autotest __q__,
                        unsigned int    _M,
                        float complex * _h,
                        unsigned int    _h_len,
                        float complex * _x,
                        unsigned int    _x_len,
                        float complex * _y,
                        unsigned int    _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    firdecim_cccf q = firdecim_cccf_create(_M, _h, _h_len);

    // allocate memory for output
    float complex y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_y_len; i++) {
        firdecim_cccf_execute(q, &_x[_M*i], &y_test[i]);
        
        LIQUID_CHECK_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy decimator object object
    firdecim_cccf_destroy(q);
}


LIQUID_AUTOTEST(firdecim_rrrf_common,"description","",0.1)
{
    firdecim_rrrf decim = firdecim_rrrf_create_kaiser(17, 4, 60.0f);
    LIQUID_CHECK(firdecim_rrrf_get_decim_rate(decim) ==  17);
    firdecim_rrrf_destroy(decim);
}

LIQUID_AUTOTEST(firdecim_crcf_common,"description","",0.1)
{
    firdecim_crcf decim = firdecim_crcf_create_kaiser(7, 4, 60.0f);
    LIQUID_CHECK(firdecim_crcf_get_decim_rate(decim) ==  7);
    firdecim_crcf_destroy(decim);
}

// 
// AUTOTEST: firdecim_rrrf tests
//
LIQUID_AUTOTEST(firdecim_rrrf_data_M2h4x20,"description","",0.1)
{
    firdecim_rrrf_test(__q__,
                       2,
                       firdecim_rrrf_data_M2h4x20_h, 4,
                       firdecim_rrrf_data_M2h4x20_x, 20,
                       firdecim_rrrf_data_M2h4x20_y, 10);
}
LIQUID_AUTOTEST(firdecim_rrrf_data_M3h7x30,"description","",0.1)
{
    firdecim_rrrf_test(__q__,
                       3,
                       firdecim_rrrf_data_M3h7x30_h, 7,
                       firdecim_rrrf_data_M3h7x30_x, 30,
                       firdecim_rrrf_data_M3h7x30_y, 10);
}
LIQUID_AUTOTEST(firdecim_rrrf_data_M4h13x40,"description","",0.1)
{
    firdecim_rrrf_test(__q__,
                       4,
                       firdecim_rrrf_data_M4h13x40_h, 13,
                       firdecim_rrrf_data_M4h13x40_x, 40,
                       firdecim_rrrf_data_M4h13x40_y, 10);
}
LIQUID_AUTOTEST(firdecim_rrrf_data_M5h23x50,"description","",0.1)
{
    firdecim_rrrf_test(__q__,
                       5,
                       firdecim_rrrf_data_M5h23x50_h, 23,
                       firdecim_rrrf_data_M5h23x50_x, 50,
                       firdecim_rrrf_data_M5h23x50_y, 10);
}


// 
// AUTOTEST: firdecim_crcf tests
//
LIQUID_AUTOTEST(firdecim_crcf_data_M2h4x20,"description","",0.1)
{
    firdecim_crcf_test(__q__,
                       2,
                       firdecim_crcf_data_M2h4x20_h, 4,
                       firdecim_crcf_data_M2h4x20_x, 20,
                       firdecim_crcf_data_M2h4x20_y, 8);
}
LIQUID_AUTOTEST(firdecim_crcf_data_M3h7x30,"description","",0.1)
{
    firdecim_crcf_test(__q__,
                       3,
                       firdecim_crcf_data_M3h7x30_h, 7,
                       firdecim_crcf_data_M3h7x30_x, 30,
                       firdecim_crcf_data_M3h7x30_y, 10);
}
LIQUID_AUTOTEST(firdecim_crcf_data_M4h13x40,"description","",0.1)
{
    firdecim_crcf_test(__q__,
                       4,
                       firdecim_crcf_data_M4h13x40_h, 13,
                       firdecim_crcf_data_M4h13x40_x, 40,
                       firdecim_crcf_data_M4h13x40_y, 10);
}
LIQUID_AUTOTEST(firdecim_crcf_data_M5h23x50,"description","",0.1)
{
    firdecim_crcf_test(__q__,
                       5,
                       firdecim_crcf_data_M5h23x50_h, 23,
                       firdecim_crcf_data_M5h23x50_x, 50,
                       firdecim_crcf_data_M5h23x50_y, 10);
}


// 
// AUTOTEST: firdecim_cccf tests
//
LIQUID_AUTOTEST(firdecim_cccf_data_M2h4x20,"description","",0.1)
{
    firdecim_cccf_test(__q__,
                       2,
                       firdecim_cccf_data_M2h4x20_h, 4,
                       firdecim_cccf_data_M2h4x20_x, 20,
                       firdecim_cccf_data_M2h4x20_y, 8);
}
LIQUID_AUTOTEST(firdecim_cccf_data_M3h7x30,"description","",0.1)
{
    firdecim_cccf_test(__q__,
                       3,
                       firdecim_cccf_data_M3h7x30_h, 7,
                       firdecim_cccf_data_M3h7x30_x, 30,
                       firdecim_cccf_data_M3h7x30_y, 10);
}
LIQUID_AUTOTEST(firdecim_cccf_data_M4h13x40,"description","",0.1)
{
    firdecim_cccf_test(__q__,
                       4,
                       firdecim_cccf_data_M4h13x40_h, 13,
                       firdecim_cccf_data_M4h13x40_x, 40,
                       firdecim_cccf_data_M4h13x40_y, 10);
}
LIQUID_AUTOTEST(firdecim_cccf_data_M5h23x50,"description","",0.1)
{
    firdecim_cccf_test(__q__,
                       5,
                       firdecim_cccf_data_M5h23x50_h, 23,
                       firdecim_cccf_data_M5h23x50_x, 50,
                       firdecim_cccf_data_M5h23x50_y, 10);
}


