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
// fftfilt_xxxf_autotest.c : test floating-point filters
//

#include "liquid.autotest.h"
#include "liquid.internal.h"

// rrrf
extern float         fftfilt_rrrf_data_h4x256_h[];
extern float         fftfilt_rrrf_data_h4x256_x[];
extern float         fftfilt_rrrf_data_h4x256_y[];

extern float         fftfilt_rrrf_data_h7x256_h[];
extern float         fftfilt_rrrf_data_h7x256_x[];
extern float         fftfilt_rrrf_data_h7x256_y[];

extern float         fftfilt_rrrf_data_h13x256_h[];
extern float         fftfilt_rrrf_data_h13x256_x[];
extern float         fftfilt_rrrf_data_h13x256_y[];

extern float         fftfilt_rrrf_data_h23x256_h[];
extern float         fftfilt_rrrf_data_h23x256_x[];
extern float         fftfilt_rrrf_data_h23x256_y[];

// crcf
extern float         fftfilt_crcf_data_h4x256_h[];
extern float complex fftfilt_crcf_data_h4x256_x[];
extern float complex fftfilt_crcf_data_h4x256_y[];

extern float         fftfilt_crcf_data_h7x256_h[];
extern float complex fftfilt_crcf_data_h7x256_x[];
extern float complex fftfilt_crcf_data_h7x256_y[];

extern float         fftfilt_crcf_data_h13x256_h[];
extern float complex fftfilt_crcf_data_h13x256_x[];
extern float complex fftfilt_crcf_data_h13x256_y[];

extern float         fftfilt_crcf_data_h23x256_h[];
extern float complex fftfilt_crcf_data_h23x256_x[];
extern float complex fftfilt_crcf_data_h23x256_y[];

// cccf
extern float complex fftfilt_cccf_data_h4x256_h[];
extern float complex fftfilt_cccf_data_h4x256_x[];
extern float complex fftfilt_cccf_data_h4x256_y[];

extern float complex fftfilt_cccf_data_h7x256_h[];
extern float complex fftfilt_cccf_data_h7x256_x[];
extern float complex fftfilt_cccf_data_h7x256_y[];

extern float complex fftfilt_cccf_data_h13x256_h[];
extern float complex fftfilt_cccf_data_h13x256_x[];
extern float complex fftfilt_cccf_data_h13x256_y[];

extern float complex fftfilt_cccf_data_h23x256_h[];
extern float complex fftfilt_cccf_data_h23x256_x[];
extern float complex fftfilt_cccf_data_h23x256_y[];

// autotest helper function
//  _h      :   filter coefficients
//  _h_len  :   filter coefficients length
//  _x      :   input array
//  _x_len  :   input array length
//  _y      :   output array
//  _y_len  :   output array length
void fftfilt_rrrf_test(liquid_autotest __q__,
                       float *      _h,
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
    liquid_log_debug("fftfilt_rrrf_test(), h_len: %3u, x_len: %3u (%3u blocks @ %3u samples, %3u remaining)",
            _h_len, _x_len, n, d.quot, d.rem);

    // load filter coefficients externally
    fftfilt_rrrf q = fftfilt_rrrf_create(_h, _h_len, n);

    // allocate memory for output
    float y_test[n*num_blocks];

    unsigned int i;

    // compute output in blocks of size 'n'
    for (i=0; i<num_blocks; i++)
        fftfilt_rrrf_execute(q, &_x[i*n], &y_test[i*n]);

    // compare results
    for (i=0; i<_y_len; i++)
        LIQUID_CHECK_DELTA( y_test[i], _y[i], tol );
    
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
void fftfilt_crcf_test(liquid_autotest __q__,
                       float *         _h,
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
    liquid_log_debug("fftfilt_crcf_test(), h_len: %3u, x_len: %3u (%3u blocks @ %3u samples, %3u remaining)",
            _h_len, _x_len, n, d.quot, d.rem);

    // load filter coefficients externally
    fftfilt_crcf q = fftfilt_crcf_create(_h, _h_len, n);

    // allocate memory for output
    float complex y_test[n*num_blocks];

    unsigned int i;

    // compute output in blocks of size 'n'
    for (i=0; i<num_blocks; i++)
        fftfilt_crcf_execute(q, &_x[i*n], &y_test[i*n]);

    // compare results
    for (i=0; i<_y_len; i++) {
        LIQUID_CHECK_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
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
void fftfilt_cccf_test(liquid_autotest __q__,
                       float complex * _h,
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
    liquid_log_debug("fftfilt_cccf_test(), h_len: %3u, x_len: %3u (%3u blocks @ %3u samples, %3u remaining)",
            _h_len, _x_len, n, d.quot, d.rem);

    // load filter coefficients externally
    fftfilt_cccf q = fftfilt_cccf_create(_h, _h_len, n);

    // allocate memory for output
    float complex y_test[n*num_blocks];

    unsigned int i;

    // compute output in blocks of size 'n'
    for (i=0; i<num_blocks; i++)
        fftfilt_cccf_execute(q, &_x[i*n], &y_test[i*n]);

    // compare results
    for (i=0; i<_y_len; i++) {
        LIQUID_CHECK_DELTA( crealf(y_test[i]), crealf(_y[i]), tol );
        LIQUID_CHECK_DELTA( cimagf(y_test[i]), cimagf(_y[i]), tol );
    }
    
    // destroy filter object
    fftfilt_cccf_destroy(q);
}


// 
// AUTOTEST: fftfilt_rrrf tests
//
LIQUID_AUTOTEST(fftfilt_rrrf_data_h4x256,"fftfilt testbench","",0.1)
{
    fftfilt_rrrf_test(__q__,
                      fftfilt_rrrf_data_h4x256_h, 4,
                      fftfilt_rrrf_data_h4x256_x, 256,
                      fftfilt_rrrf_data_h4x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_rrrf_data_h7x256,"fftfilt testbench","",0.1)
{
    fftfilt_rrrf_test(__q__,
                      fftfilt_rrrf_data_h7x256_h, 7,
                      fftfilt_rrrf_data_h7x256_x, 256,
                      fftfilt_rrrf_data_h7x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_rrrf_data_h13x256,"fftfilt testbench","",0.1)
{
    fftfilt_rrrf_test(__q__,
                      fftfilt_rrrf_data_h13x256_h, 13,
                      fftfilt_rrrf_data_h13x256_x, 256,
                      fftfilt_rrrf_data_h13x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_rrrf_data_h23x256,"fftfilt testbench","",0.1)
{
    fftfilt_rrrf_test(__q__,
                      fftfilt_rrrf_data_h23x256_h, 23,
                      fftfilt_rrrf_data_h23x256_x, 256,
                      fftfilt_rrrf_data_h23x256_y, 256);
}


// 
// AUTOTEST: fftfilt_crcf tests
//
LIQUID_AUTOTEST(fftfilt_crcf_data_h4x256,"fftfilt testbench","",0.1)
{
    fftfilt_crcf_test(__q__,
                      fftfilt_crcf_data_h4x256_h, 4,
                      fftfilt_crcf_data_h4x256_x, 256,
                      fftfilt_crcf_data_h4x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_crcf_data_h7x256,"fftfilt testbench","",0.1)
{
    fftfilt_crcf_test(__q__,
                      fftfilt_crcf_data_h7x256_h, 7,
                      fftfilt_crcf_data_h7x256_x, 256,
                      fftfilt_crcf_data_h7x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_crcf_data_h13x256,"fftfilt testbench","",0.1)
{
    fftfilt_crcf_test(__q__,
                      fftfilt_crcf_data_h13x256_h, 13,
                      fftfilt_crcf_data_h13x256_x, 256,
                      fftfilt_crcf_data_h13x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_crcf_data_h23x256,"fftfilt testbench","",0.1)
{
    fftfilt_crcf_test(__q__,
                      fftfilt_crcf_data_h23x256_h, 23,
                      fftfilt_crcf_data_h23x256_x, 256,
                      fftfilt_crcf_data_h23x256_y, 256);
}


// 
// AUTOTEST: fftfilt_cccf tests
//
LIQUID_AUTOTEST(fftfilt_cccf_data_h4x256,"fftfilt testbench","",0.1)
{
    fftfilt_cccf_test(__q__,
                      fftfilt_cccf_data_h4x256_h, 4,
                      fftfilt_cccf_data_h4x256_x, 256,
                      fftfilt_cccf_data_h4x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_cccf_data_h7x256,"fftfilt testbench","",0.1)
{
    fftfilt_cccf_test(__q__,
                      fftfilt_cccf_data_h7x256_h, 7,
                      fftfilt_cccf_data_h7x256_x, 256,
                      fftfilt_cccf_data_h7x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_cccf_data_h13x256,"fftfilt testbench","",0.1)
{
    fftfilt_cccf_test(__q__,
                      fftfilt_cccf_data_h13x256_h, 13,
                      fftfilt_cccf_data_h13x256_x, 256,
                      fftfilt_cccf_data_h13x256_y, 256);
}
LIQUID_AUTOTEST(fftfilt_cccf_data_h23x256,"fftfilt testbench","",0.1)
{
    fftfilt_cccf_test(__q__,
                      fftfilt_cccf_data_h23x256_h, 23,
                      fftfilt_cccf_data_h23x256_x, 256,
                      fftfilt_cccf_data_h23x256_y, 256);
}

LIQUID_AUTOTEST(fftfilt_config,"fftfilt testbench","",0.1)
{
    _liquid_error_downgrade_enable();
    // check that object returns NULL for invalid configurations
    float h[9] = {0,1,2,3,4,5,6,7,8,};
    LIQUID_CHECK(NULL ==fftfilt_crcf_create(h,0,64)); // filter length too small
    LIQUID_CHECK(NULL ==fftfilt_crcf_create(h,9, 7)); // block length too small

    // create proper object and test configurations
    fftfilt_crcf filt = fftfilt_crcf_create(h, 9, 64);

    LIQUID_CHECK(LIQUID_OK == fftfilt_crcf_print(filt));
    LIQUID_CHECK(LIQUID_OK == fftfilt_crcf_set_scale(filt,  3.0f));
    float scale = 0.0f;
    LIQUID_CHECK(LIQUID_OK == fftfilt_crcf_get_scale(filt,  &scale));
    LIQUID_CHECK(scale ==  3.0f);
    LIQUID_CHECK(9 ==  fftfilt_crcf_get_length(filt));

    fftfilt_crcf_destroy(filt);
    _liquid_error_downgrade_disable();
}

LIQUID_AUTOTEST(fftfilt_copy,"fftfilt copy","",0.1)
{
    // generate random filter coefficients
    unsigned int i, j, h_len = 31;
    float h[h_len];
    for (i=0; i<h_len; i++)
        h[i] = randnf();

    // determine appropriate block size
    // NOTE: this number can be anything at least _h_len-1
    unsigned int n = 96;

    // create object
    fftfilt_crcf q0 = fftfilt_crcf_create(h, h_len, n);

    // compute output in blocks of size 'n'
    float complex buf[n], buf_0[n], buf_1[n];
    for (i=0; i<10; i++) {
        for (j=0; j<n; j++)
            buf[j] = randnf() + _Complex_I*randnf();
        fftfilt_crcf_execute(q0, buf, buf_0);
    }

    // copy object
    fftfilt_crcf q1 = fftfilt_crcf_copy(q0);

    // run filters in parallel and compare results
    for (i=0; i<10; i++) {
        for (j=0; j<n; j++)
            buf[j] = randnf() + _Complex_I*randnf();
        fftfilt_crcf_execute(q0, buf, buf_0);
        fftfilt_crcf_execute(q1, buf, buf_1);

        LIQUID_CHECK_ARRAY( buf_0, buf_1, n*sizeof(float complex));
    }
    
    // destroy objects
    fftfilt_crcf_destroy(q0);
    fftfilt_crcf_destroy(q1);
}

