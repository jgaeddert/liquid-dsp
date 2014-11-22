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

// autotest helper function
//  _M      :   decimation rate
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void firdecim_rrrf_test(unsigned int    _M,
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
        
        CONTEND_DELTA( y_test[i], _y[i], tol );
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
void firdecim_crcf_test(unsigned int    _M,
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
        
        CONTEND_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
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
void firdecim_cccf_test(unsigned int    _M,
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
        
        CONTEND_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy decimator object object
    firdecim_cccf_destroy(q);
}

