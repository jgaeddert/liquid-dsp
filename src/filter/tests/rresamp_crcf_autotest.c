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

#include <string.h>
//#include "autotest/autotest.h"

#include "liquid.internal.h"
#include "liquid.autotest.h"

// test rational-rate resampler
void test_rresamp_crcf(liquid_autotest __q__,
                       const char * _method,
                       unsigned int _interp,
                       unsigned int _decim,
                       unsigned int _m,
                       float        _bw,
                       float        _as)
{
    // options
    unsigned int n    = 800000; // number of output samples to analyze
    float        bw   = 0.2f;   // target output bandwidth
    unsigned int nfft = 800;    // number of bins in transform
    float        tol  = 0.5f;   // error tolerance [dB]

    // create resampler with rate _interp/_decim
    rresamp_crcf resamp = NULL;
    if (strcmp(_method,"baseline")==0) {
        resamp = rresamp_crcf_create_kaiser(_interp, _decim, _m, _bw, _as);
    } else if (strcmp(_method,"default")==0) {
        resamp = rresamp_crcf_create_default(_interp, _decim);
    } else {
        //printf("creating resampler using %s\n", _method);
        int ftype = liquid_getopt_str2firfilt(_method);
        float beta = _bw; // rename to avoid confusion
        resamp = rresamp_crcf_create_prototype(ftype, _interp, _decim, _m, beta);
    }

    LIQUID_REQUIRE( resamp != NULL );

    float r = rresamp_crcf_get_rate(resamp);

    // create and configure objects
    spgramcf     q   = spgramcf_create(nfft, LIQUID_WINDOW_HANN, nfft/2, nfft/4);
    symstreamrcf gen = symstreamrcf_create_linear(LIQUID_FIRFILT_KAISER,r*bw,25,0.2f,LIQUID_MODEM_QPSK);
    symstreamrcf_set_gain(gen, sqrtf(bw*r));

    // generate samples and push through spgram object
    float complex buf_0[_decim]; // input buffer
    float complex buf_1[_interp]; // output buffer
    while (spgramcf_get_num_samples_total(q) < n) {
        // generate block of samples
        symstreamrcf_write_samples(gen, buf_0, _decim);

        // resample
        rresamp_crcf_execute(resamp, buf_0, buf_1);

        // run samples through the spgram object
        spgramcf_write(q, buf_1, _interp);
    }

    // verify result
    float psd[nfft];
    spgramcf_get_psd(q, psd);
    autotest_psd_s regions[] = {
        {.fmin=-0.5f,    .fmax=-0.6f*bw, .pmin=0,     .pmax=-_as+tol, .test_lo=0, .test_hi=1},
        {.fmin=-0.4f*bw, .fmax=+0.4f*bw, .pmin=0-tol, .pmax=  0 +tol, .test_lo=1, .test_hi=1},
        {.fmin=+0.6f*bw, .fmax=+0.5f,    .pmin=0,     .pmax=-_as+tol, .test_lo=0, .test_hi=1},
    };
    char filename[256];
    sprintf(filename,"autotest/logs/rresamp_crcf_%s_P%u_Q%u.m", _method, _interp, _decim);
    liquid_autotest_validate_spectrum(__q__, psd, nfft, regions, 3, filename);

    // destroy objects
    rresamp_crcf_destroy(resamp);
    spgramcf_destroy(q);
    symstreamrcf_destroy(gen);
}

// baseline tests using create_kaiser() method
LIQUID_AUTOTEST(rresamp_crcf_baseline_P1_Q5, "baseline resampler", "", 0.1)
    { test_rresamp_crcf(__q__,"baseline", 1, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_baseline_P2_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"baseline", 2, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_baseline_P3_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"baseline", 3, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_baseline_P6_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"baseline", 6, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_baseline_P8_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"baseline", 8, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_baseline_P9_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"baseline", 9, 5, 15, -1, 60.0f); }

// tests using create_default() method
LIQUID_AUTOTEST(rresamp_crcf_default_P1_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"default", 1, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_default_P2_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"default", 2, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_default_P3_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"default", 3, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_default_P6_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"default", 6, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_default_P8_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"default", 8, 5, 15, -1, 60.0f); }

LIQUID_AUTOTEST(rresamp_crcf_default_P9_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"default", 9, 5, 15, -1, 60.0f); }

// tests using create_prototype() method
LIQUID_AUTOTEST(rresamp_crcf_arkaiser_P3_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"arkaiser", 3, 5, 40, 0.2, 50.0f); }

LIQUID_AUTOTEST(rresamp_crcf_arkaiser_P5_Q3, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"arkaiser", 5, 3, 40, 0.2, 50.0f); }

LIQUID_AUTOTEST(rresamp_crcf_rrcos_P3_Q5, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"rrcos",    3, 5, 40, 0.2, 50.0f); }

LIQUID_AUTOTEST(rresamp_crcf_rrcos_P5_Q3, "description", "", 0.1)
    { test_rresamp_crcf(__q__,"rrcos",    5, 3, 40, 0.2, 50.0f); }

/*
// test copy method
LIQUID_AUTOTEST(rresamp_copy, "copy rational-rate resampler", "", 0)
{
    // create resampler with rate P/Q
    unsigned int i, P = 17, Q = 23, m = 12;
    rresamp_crcf q0 = rresamp_crcf_create_kaiser(P, Q, m, 0.4f, 60.0f);
    rresamp_crcf_set_scale(q0, 0.12345f);

    // create generator with default parameters
    symstreamrcf gen = symstreamrcf_create();

    // generate samples and push through resampler
    float complex buf  [Q]; // input buffer
    float complex buf_0[P]; // output buffer (orig)
    float complex buf_1[P]; // output buffer (copy)
    for (i=0; i<10; i++) {
        // generate block of samples
        symstreamrcf_write_samples(gen, buf, Q);

        // resample
        rresamp_crcf_execute(q0, buf, buf_0);
    }

    // copy object
    rresamp_crcf q1 = rresamp_crcf_copy(q0);

    // run samples through both resamplers in parallel
    for (i=0; i<60; i++) {
        // generate block of samples
        symstreamrcf_write_samples(gen, buf, Q);

        // resample
        rresamp_crcf_execute(q0, buf, buf_0);
        rresamp_crcf_execute(q1, buf, buf_1);

        // compare output
        CONTEND_SAME_DATA(buf_0, buf_1, P*sizeof(float complex));
    }

    // destroy objects
    rresamp_crcf_destroy(q0);
    rresamp_crcf_destroy(q1);
    symstreamrcf_destroy(gen);
}
*/

LIQUID_AUTOTEST(rresamp_config, "test errors and invalid configuration", "", 0)
{
    _liquid_error_downgrade_enable();

    // test copying/creating invalid objects
    LIQUID_CHECK( NULL == rresamp_crcf_copy(NULL) );
    LIQUID_CHECK( NULL == rresamp_crcf_create(0, 5, 20, NULL) ); // interp is 0
    LIQUID_CHECK( NULL == rresamp_crcf_create(3, 0, 20, NULL) ); // decim is 0
    LIQUID_CHECK( NULL == rresamp_crcf_create(3, 5,  0, NULL) ); // filter length is 0
    LIQUID_CHECK( NULL == rresamp_crcf_create_kaiser(3,5,20,99.0f,60) ); // bandwidth > 0.5

    // create valid object
    rresamp_crcf resamp = rresamp_crcf_create_kaiser(30, 50, 20, 0.3f, 60.0f);
    LIQUID_CHECK( LIQUID_OK == rresamp_crcf_print(resamp) );
    LIQUID_CHECK( LIQUID_OK == rresamp_crcf_set_scale(resamp, 7.22f) );
    float scale;
    LIQUID_CHECK( LIQUID_OK == rresamp_crcf_get_scale(resamp, &scale) );
    LIQUID_CHECK( scale == 7.22f );

    // get properties
    LIQUID_CHECK( rresamp_crcf_get_delay    (resamp) == 20 );
    LIQUID_CHECK( rresamp_crcf_get_block_len(resamp) == 10 );
    LIQUID_CHECK( rresamp_crcf_get_rate     (resamp) ==0.6f);
    LIQUID_CHECK( rresamp_crcf_get_P        (resamp) == 30 );
    LIQUID_CHECK( rresamp_crcf_get_interp   (resamp) ==  3 );
    LIQUID_CHECK( rresamp_crcf_get_Q        (resamp) == 50 );
    LIQUID_CHECK( rresamp_crcf_get_decim    (resamp) ==  5 );
    rresamp_crcf_destroy(resamp);

    _liquid_error_downgrade_disable();
}

