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

#include <stdlib.h>
#include <string.h>
#include "autotest/autotest.h"
#include "liquid.h"

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void fftfilt_rrrf_test(float *      _h,
                       unsigned int _h_len,
                       float *      _x,
                       unsigned int _x_len,
                       float *      _y,
                       unsigned int _y_len)
{
    float tol = 0.001f;

    // determine appropriate block size
    // NOTE: this number can be anything at least _h_len-1
    unsigned int n = 1 << liquid_nextpow2(_h_len-1);

    // determine number of blocks
    div_t d = div(_x_len, n);
    unsigned int num_blocks = d.quot + (d.rem ? 1 : 0);
    if (liquid_autotest_verbose) {
        printf("fftfilt_rrrf_test(), h_len: %3u, x_len: %3u (%3u blocks @ %3u samples, %3u remaining)\n",
                _h_len, _x_len, n, d.quot, d.rem);
    }

    // load filter coefficients externally
    fftfilt_rrrf q = fftfilt_rrrf_create(_h, _h_len, n);

    // allocate memory for output
    float x_test[n*num_blocks];
    float y_test[n*num_blocks];

    // copy input for test
    unsigned int i;
    for (i=0; i<n*num_blocks; i++)
        x_test[i] = (i < _x_len) ? _x[i] : 0;

    // compute output in blocks of size 'n'
    for (i=0; i<num_blocks; i++)
        fftfilt_rrrf_execute(q, &_x[i*n], &y_test[i*n]);

    // compare results
    for (i=0; i<_y_len; i++)
        CONTEND_DELTA( y_test[i], _y[i], tol );
    
    // destroy filter object
    fftfilt_rrrf_destroy(q);
}

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void fftfilt_crcf_test(float *         _h,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len)
{
    float tol = 0.001f;

    // determine appropriate block size
    // NOTE: this number can be anything at least _h_len-1
    unsigned int n = 1 << liquid_nextpow2(_h_len-1);

    // determine number of blocks
    div_t d = div(_x_len, n);
    unsigned int num_blocks = d.quot + (d.rem ? 1 : 0);
    if (liquid_autotest_verbose) {
        printf("fftfilt_crcf_test(), h_len: %3u, x_len: %3u (%3u blocks @ %3u samples, %3u remaining)\n",
                _h_len, _x_len, n, d.quot, d.rem);
    }

    // load filter coefficients externally
    fftfilt_crcf q = fftfilt_crcf_create(_h, _h_len, n);

    // allocate memory for output
    float complex x_test[n*num_blocks];
    float complex y_test[n*num_blocks];

    // copy input for test
    unsigned int i;
    for (i=0; i<n*num_blocks; i++)
        x_test[i] = (i < _x_len) ? _x[i] : 0;

    // compute output in blocks of size 'n'
    for (i=0; i<num_blocks; i++)
        fftfilt_crcf_execute(q, &_x[i*n], &y_test[i*n]);

    // compare results
    for (i=0; i<_y_len; i++) {
        CONTEND_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    fftfilt_crcf_destroy(q);
}

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void fftfilt_cccf_test(float complex * _h,
                       unsigned int    _h_len,
                       float complex * _x,
                       unsigned int    _x_len,
                       float complex * _y,
                       unsigned int    _y_len)
{
    float tol = 0.001f;

    // determine appropriate block size
    // NOTE: this number can be anything at least _h_len-1
    unsigned int n = 1 << liquid_nextpow2(_h_len-1);

    // determine number of blocks
    div_t d = div(_x_len, n);
    unsigned int num_blocks = d.quot + (d.rem ? 1 : 0);
    if (liquid_autotest_verbose) {
        printf("fftfilt_cccf_test(), h_len: %3u, x_len: %3u (%3u blocks @ %3u samples, %3u remaining)\n",
                _h_len, _x_len, n, d.quot, d.rem);
    }

    // load filter coefficients externally
    fftfilt_cccf q = fftfilt_cccf_create(_h, _h_len, n);

    // allocate memory for output
    float complex x_test[n*num_blocks];
    float complex y_test[n*num_blocks];

    // copy input for test
    unsigned int i;
    for (i=0; i<n*num_blocks; i++)
        x_test[i] = (i < _x_len) ? _x[i] : 0;

    // compute output in blocks of size 'n'
    for (i=0; i<num_blocks; i++)
        fftfilt_cccf_execute(q, &_x[i*n], &y_test[i*n]);

    // compare results
    for (i=0; i<_y_len; i++) {
        CONTEND_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        CONTEND_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    fftfilt_cccf_destroy(q);
}

