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
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firfilt_rrrf_test(float *      _h,
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
        
        CONTEND_DELTA( y_test[i], _y[i], tol );
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
void firfilt_crcf_test(float *         _h,
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
        
        CONTEND_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
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
void firfilt_cccf_test(float complex * _h,
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
        
        CONTEND_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    firfilt_cccf_destroy(q);
}







//
// fixed-point helper functions
//

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firfilt_rrrq16_test(float *      _h,
                         unsigned int _h_len,
                         float *      _x,
                         unsigned int _x_len,
                         float *      _y,
                         unsigned int _y_len)
{
    unsigned int i;
    float tol = 0.01f;

    // convert to fixed-point
    q16_t h[_h_len];    q16_memmove_float_to_fixed(h, _h, _h_len);
    q16_t x[_x_len];    q16_memmove_float_to_fixed(x, _x, _x_len);
    q16_t y[_y_len];    q16_memmove_float_to_fixed(y, _y, _y_len);

    // load filter coefficients
    firfilt_rrrq16 q = firfilt_rrrq16_create(h, _h_len);

    // compute output
    for (i=0; i<_x_len; i++) {
        firfilt_rrrq16_push(q, x[i]);
        firfilt_rrrq16_execute(q, &y[i]);
       
        // test floating-point result
        float yf = q16_fixed_to_float(y[i]);
        CONTEND_DELTA( yf, _y[i], tol );
    }

    // destroy filter object
    firfilt_rrrq16_destroy(q);
}

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firfilt_crcq16_test(float *         _h,
                         unsigned int    _h_len,
                         float complex * _x,
                         unsigned int    _x_len,
                         float complex * _y,
                         unsigned int    _y_len)
{
    unsigned int i;
    float tol = 0.01f;

    // convert to fixed-point
    q16_t h[_h_len];    q16_memmove_float_to_fixed(h, _h, _h_len);
    cq16_t x[_x_len];   cq16_memmove_float_to_fixed(x, _x, _x_len);
    cq16_t y[_y_len];   cq16_memmove_float_to_fixed(y, _y, _y_len);

    // load filter coefficients
    firfilt_crcq16 q = firfilt_crcq16_create(h, _h_len);

    // compute output
    for (i=0; i<_x_len; i++) {
        firfilt_crcq16_push(q, x[i]);
        firfilt_crcq16_execute(q, &y[i]);
       
        // test floating-point result
        float complex yf = cq16_fixed_to_float(y[i]);
        CONTEND_DELTA( crealf(yf), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(yf), cimagf(_y[i]), tol );
    }

    // destroy filter object
    firfilt_crcq16_destroy(q);
}

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firfilt_cccq16_test(float complex * _h,
                         unsigned int    _h_len,
                         float complex * _x,
                         unsigned int    _x_len,
                         float complex * _y,
                         unsigned int    _y_len)
{
    unsigned int i;
    float tol = 0.01f;

    // convert to fixed-point
    cq16_t h[_h_len];   cq16_memmove_float_to_fixed(h, _h, _h_len);
    cq16_t x[_x_len];   cq16_memmove_float_to_fixed(x, _x, _x_len);
    cq16_t y[_y_len];   cq16_memmove_float_to_fixed(y, _y, _y_len);

    // load filter coefficients
    firfilt_cccq16 q = firfilt_cccq16_create(h, _h_len);

    // compute output
    for (i=0; i<_x_len; i++) {
        firfilt_cccq16_push(q, x[i]);
        firfilt_cccq16_execute(q, &y[i]);
       
        // test floating-point result
        float complex yf = cq16_fixed_to_float(y[i]);
        CONTEND_DELTA( crealf(yf), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(yf), cimagf(_y[i]), tol );
    }

    // destroy filter object
    firfilt_cccq16_destroy(q);
}

