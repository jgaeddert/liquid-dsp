/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

// autotest helper function
//  _b      :   filter coefficients (numerator)
//  _a      :   filter coefficients (denominator)
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void iirfilt_rrrf_test(float *      _b,
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
        
        CONTEND_DELTA( y_test[i], _y[i], tol );
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
void iirfilt_crcf_test(float *         _b,
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
        
        CONTEND_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
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
void iirfilt_cccf_test(float complex * _b,
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
        
        CONTEND_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    iirfilt_cccf_destroy(q);
}




// 
// fixed-point tests
//

// autotest helper function
//  _b      :   filter coefficients (numerator)
//  _a      :   filter coefficients (denominator)
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void iirfilt_rrrq16_test(float *      _b,
                         float *      _a,
                         unsigned int _h_len,
                         float *      _x,
                         unsigned int _x_len,
                         float *      _y,
                         unsigned int _y_len)
{
    unsigned int i;
    float tol = 0.2f;

    // convert coefficients to fixed-point
    q16_t b[_h_len];
    q16_t a[_h_len];
    for (i=0; i<_h_len; i++) {
        b[i] = q16_float_to_fixed(_b[i]);
        a[i] = q16_float_to_fixed(_a[i]);
    }

    // load filter coefficients externally
    iirfilt_rrrq16 q = iirfilt_rrrq16_create(b, _h_len, a, _h_len);

    // compute output
    for (i=0; i<_x_len; i++) {
        // convert input sample to fixed-point
        q16_t x = q16_float_to_fixed(_x[i]);
        q16_t y;
        iirfilt_rrrq16_execute(q, x, &y);

        // convert result to floating-point
        float yf = q16_fixed_to_float(y);
        
        // check result
        CONTEND_DELTA( yf, _y[i], tol );
    }

    // destroy filter object
    iirfilt_rrrq16_destroy(q);
}

// autotest helper function
//  _b      :   filter coefficients (numerator)
//  _a      :   filter coefficients (denominator)
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void iirfilt_crcq16_test(float *         _b,
                         float *         _a,
                         unsigned int    _h_len,
                         float complex * _x,
                         unsigned int    _x_len,
                         float complex * _y,
                         unsigned int    _y_len)
{
    unsigned int i;
    float tol = 0.2f;

    // convert coefficients to fixed-point
    q16_t b[_h_len];
    q16_t a[_h_len];
    for (i=0; i<_h_len; i++) {
        b[i] = q16_float_to_fixed(_b[i]);
        a[i] = q16_float_to_fixed(_a[i]);
    }

    // load filter coefficients externally
    iirfilt_crcq16 q = iirfilt_crcq16_create(b, _h_len, a, _h_len);

    // compute output
    for (i=0; i<_x_len; i++) {
        // convert input sample to fixed-point
        cq16_t x = cq16_float_to_fixed(_x[i]);
        cq16_t y;
        iirfilt_crcq16_execute(q, x, &y);

        // convert result to floating-point
        float complex yf = cq16_fixed_to_float(y);
        
        // check result
        CONTEND_DELTA( yf, _y[i], tol );
    }

    // destroy filter object
    iirfilt_crcq16_destroy(q);
}

// autotest helper function
//  _b      :   filter coefficients (numerator)
//  _a      :   filter coefficients (denominator)
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void iirfilt_cccq16_test(float complex * _b,
                         float complex * _a,
                         unsigned int    _h_len,
                         float complex * _x,
                         unsigned int    _x_len,
                         float complex * _y,
                         unsigned int    _y_len)
{
    unsigned int i;
    float tol = 0.2f;

    // convert coefficients to fixed-point
    cq16_t b[_h_len];
    cq16_t a[_h_len];
    for (i=0; i<_h_len; i++) {
        b[i] = cq16_float_to_fixed(_b[i]);
        a[i] = cq16_float_to_fixed(_a[i]);
    }

    // load filter coefficients externally
    iirfilt_cccq16 q = iirfilt_cccq16_create(b, _h_len, a, _h_len);

    // compute output
    for (i=0; i<_x_len; i++) {
        // convert input sample to fixed-point
        cq16_t x = cq16_float_to_fixed(_x[i]);
        cq16_t y;
        iirfilt_cccq16_execute(q, x, &y);

        // convert result to floating-point
        float complex yf = cq16_fixed_to_float(y);
        
        // check result
        CONTEND_DELTA( yf, _y[i], tol );
    }

    // destroy filter object
    iirfilt_cccq16_destroy(q);
}

