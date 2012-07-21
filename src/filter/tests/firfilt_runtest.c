/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
    q16_t h[_h_len];
    q16_t x[_x_len];
    q16_t y[_y_len];
    for (i=0; i<_h_len; i++)
        h[i] = q16_float_to_fixed(_h[i]);
    for (i=0; i<_x_len; i++)
        x[i] = q16_float_to_fixed(_x[i]);
    for (i=0; i<_y_len; i++)
        y[i] = q16_float_to_fixed(_y[i]);

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
    q16_t h[_h_len];
    cq16_t x[_x_len];
    cq16_t y[_y_len];
    for (i=0; i<_h_len; i++)
        h[i] = q16_float_to_fixed(_h[i]);
    for (i=0; i<_x_len; i++)
        x[i] = cq16_float_to_fixed(_x[i]);
    for (i=0; i<_y_len; i++)
        y[i] = cq16_float_to_fixed(_y[i]);

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
    cq16_t h[_h_len];
    cq16_t x[_x_len];
    cq16_t y[_y_len];
    for (i=0; i<_h_len; i++)
        h[i] = cq16_float_to_fixed(_h[i]);
    for (i=0; i<_x_len; i++)
        x[i] = cq16_float_to_fixed(_x[i]);
    for (i=0; i<_y_len; i++)
        y[i] = cq16_float_to_fixed(_y[i]);

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

