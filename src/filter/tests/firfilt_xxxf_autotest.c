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
// firfilt_xxxf_autotest.c : test floating-point filters
//

#include "liquid.autotest.h"
#include "liquid.h"

// rrrf
extern float         firfilt_rrrf_data_h4x8_h[];
extern float         firfilt_rrrf_data_h4x8_x[];
extern float         firfilt_rrrf_data_h4x8_y[];

extern float         firfilt_rrrf_data_h7x16_h[];
extern float         firfilt_rrrf_data_h7x16_x[];
extern float         firfilt_rrrf_data_h7x16_y[];

extern float         firfilt_rrrf_data_h13x32_h[];
extern float         firfilt_rrrf_data_h13x32_x[];
extern float         firfilt_rrrf_data_h13x32_y[];

extern float         firfilt_rrrf_data_h23x64_h[];
extern float         firfilt_rrrf_data_h23x64_x[];
extern float         firfilt_rrrf_data_h23x64_y[];

// crcf
extern float         firfilt_crcf_data_h4x8_h[];
extern float complex firfilt_crcf_data_h4x8_x[];
extern float complex firfilt_crcf_data_h4x8_y[];

extern float         firfilt_crcf_data_h7x16_h[];
extern float complex firfilt_crcf_data_h7x16_x[];
extern float complex firfilt_crcf_data_h7x16_y[];

extern float         firfilt_crcf_data_h13x32_h[];
extern float complex firfilt_crcf_data_h13x32_x[];
extern float complex firfilt_crcf_data_h13x32_y[];

extern float         firfilt_crcf_data_h23x64_h[];
extern float complex firfilt_crcf_data_h23x64_x[];
extern float complex firfilt_crcf_data_h23x64_y[];

// cccf
extern float complex firfilt_cccf_data_h4x8_h[];
extern float complex firfilt_cccf_data_h4x8_x[];
extern float complex firfilt_cccf_data_h4x8_y[];

extern float complex firfilt_cccf_data_h7x16_h[];
extern float complex firfilt_cccf_data_h7x16_x[];
extern float complex firfilt_cccf_data_h7x16_y[];

extern float complex firfilt_cccf_data_h13x32_h[];
extern float complex firfilt_cccf_data_h13x32_x[];
extern float complex firfilt_cccf_data_h13x32_y[];

extern float complex firfilt_cccf_data_h23x64_h[];
extern float complex firfilt_cccf_data_h23x64_x[];
extern float complex firfilt_cccf_data_h23x64_y[];


// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firfilt_rrrf_test(liquid_autotest __q__,
                       float *      _h,
                       unsigned int _h_len,
                       float *      _x,
                       unsigned int _x_len,
                       float *      _y,
                       unsigned int _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    firfilt_rrrf q = firfilt_rrrf_create(_h, _h_len);

    // allocate memory for output
    float y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_x_len; i++) {
        firfilt_rrrf_push(q, _x[i]);
        firfilt_rrrf_execute(q, &y_test[i]);
        
        LIQUID_CHECK_DELTA( y_test[i], _y[i], tol );
    }

    // destroy filter object
    firfilt_rrrf_destroy(q);
}

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firfilt_crcf_test(liquid_autotest __q__,
                       float *         _h,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    firfilt_crcf q = firfilt_crcf_create(_h, _h_len);

    // allocate memory for output
    float complex y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_x_len; i++) {
        firfilt_crcf_push(q, _x[i]);
        firfilt_crcf_execute(q, &y_test[i]);
        
        LIQUID_CHECK_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    firfilt_crcf_destroy(q);
}

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firfilt_cccf_test(liquid_autotest __q__,
                       float complex * _h,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len)
{
    float tol = 0.001f;

    // load filter coefficients externally
    firfilt_cccf q = firfilt_cccf_create(_h, _h_len);

    // allocate memory for output
    float complex y_test[_y_len];

    unsigned int i;
    // compute output
    for (i=0; i<_x_len; i++) {
        firfilt_cccf_push(q, _x[i]);
        firfilt_cccf_execute(q, &y_test[i]);
        
        LIQUID_CHECK_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    firfilt_cccf_destroy(q);
}

// 
// AUTOTEST: firfilt_rrrf tests
//
LIQUID_AUTOTEST(firfilt_rrrf_data_h4x8,"description","",0.1)
{
    firfilt_rrrf_test(__q__,
                      firfilt_rrrf_data_h4x8_h, 4,
                      firfilt_rrrf_data_h4x8_x, 8,
                      firfilt_rrrf_data_h4x8_y, 8);
}
LIQUID_AUTOTEST(firfilt_rrrf_data_h7x16,"description","",0.1)
{
    firfilt_rrrf_test(__q__,
                      firfilt_rrrf_data_h7x16_h, 7,
                      firfilt_rrrf_data_h7x16_x, 16,
                      firfilt_rrrf_data_h7x16_y, 16);
}
LIQUID_AUTOTEST(firfilt_rrrf_data_h13x32,"description","",0.1)
{
    firfilt_rrrf_test(__q__,
                      firfilt_rrrf_data_h13x32_h, 13,
                      firfilt_rrrf_data_h13x32_x, 32,
                      firfilt_rrrf_data_h13x32_y, 32);
}
LIQUID_AUTOTEST(firfilt_rrrf_data_h23x64,"description","",0.1)
{
    firfilt_rrrf_test(__q__,
                      firfilt_rrrf_data_h23x64_h, 23,
                      firfilt_rrrf_data_h23x64_x, 64,
                      firfilt_rrrf_data_h23x64_y, 64);
}


// 
// AUTOTEST: firfilt_crcf tests
//
LIQUID_AUTOTEST(firfilt_crcf_data_h4x8,"description","",0.1)
{
    firfilt_crcf_test(__q__,
                      firfilt_crcf_data_h4x8_h, 4,
                      firfilt_crcf_data_h4x8_x, 8,
                      firfilt_crcf_data_h4x8_y, 8);
}
LIQUID_AUTOTEST(firfilt_crcf_data_h7x16,"description","",0.1)
{
    firfilt_crcf_test(__q__,
                      firfilt_crcf_data_h7x16_h, 7,
                      firfilt_crcf_data_h7x16_x, 16,
                      firfilt_crcf_data_h7x16_y, 16);
}
LIQUID_AUTOTEST(firfilt_crcf_data_h13x32,"description","",0.1)
{
    firfilt_crcf_test(__q__,
                      firfilt_crcf_data_h13x32_h, 13,
                      firfilt_crcf_data_h13x32_x, 32,
                      firfilt_crcf_data_h13x32_y, 32);
}
LIQUID_AUTOTEST(firfilt_crcf_data_h23x64,"description","",0.1)
{
    firfilt_crcf_test(__q__,
                      firfilt_crcf_data_h23x64_h, 23,
                      firfilt_crcf_data_h23x64_x, 64,
                      firfilt_crcf_data_h23x64_y, 64);
}


// 
// AUTOTEST: firfilt_cccf tests
//
LIQUID_AUTOTEST(firfilt_cccf_data_h4x8,"description","",0.1)
{
    firfilt_cccf_test(__q__,
                      firfilt_cccf_data_h4x8_h, 4,
                      firfilt_cccf_data_h4x8_x, 8,
                      firfilt_cccf_data_h4x8_y, 8);
}
LIQUID_AUTOTEST(firfilt_cccf_data_h7x16,"description","",0.1)
{
    firfilt_cccf_test(__q__,
                      firfilt_cccf_data_h7x16_h, 7,
                      firfilt_cccf_data_h7x16_x, 16,
                      firfilt_cccf_data_h7x16_y, 16);
}
LIQUID_AUTOTEST(firfilt_cccf_data_h13x32,"description","",0.1)
{
    firfilt_cccf_test(__q__,
                      firfilt_cccf_data_h13x32_h, 13,
                      firfilt_cccf_data_h13x32_x, 32,
                      firfilt_cccf_data_h13x32_y, 32);
}
LIQUID_AUTOTEST(firfilt_cccf_data_h23x64,"description","",0.1)
{
    firfilt_cccf_test(__q__,
                      firfilt_cccf_data_h23x64_h, 23,
                      firfilt_cccf_data_h23x64_x, 64,
                      firfilt_cccf_data_h23x64_y, 64);
}


