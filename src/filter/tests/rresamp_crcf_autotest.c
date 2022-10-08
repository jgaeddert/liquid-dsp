/*
 * Copyright (c) 2007 - 2022 Joseph Gaeddert
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

// convenience methods
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

// test rational-rate resampler
void test_harness_rresamp_crcf(unsigned int _P,
                               unsigned int _Q,
                               unsigned int _m,
                               float        _bw,
                               float        _as)
{
    // options
    unsigned int n=800000;  // number of output samples to analyze
    float bw = 0.2f; // target output bandwidth
    unsigned int nfft = 800;
    float tol = 0.5f;

    // create resampler with rate P/Q
    rresamp_crcf resamp = rresamp_crcf_create_kaiser(_P, _Q, _m, _bw, _as);
    float r = rresamp_crcf_get_rate(resamp);

    // create and configure objects
    spgramcf     q   = spgramcf_create(nfft, LIQUID_WINDOW_HANN, nfft/2, nfft/4);
    symstreamrcf gen = symstreamrcf_create_linear(LIQUID_FIRFILT_KAISER,r*bw,25,0.2f,LIQUID_MODEM_QPSK);
    symstreamrcf_set_gain(gen, sqrtf(bw*r));

    // generate samples and push through spgram object
    float complex buf_0[_Q]; // input buffer
    float complex buf_1[_P]; // output buffer
    while (spgramcf_get_num_samples_total(q) < n) {
        // generate block of samples
        symstreamrcf_write_samples(gen, buf_0, _Q);

        // resample
        rresamp_crcf_execute(resamp, buf_0, buf_1);

        // run samples through the spgram object
        spgramcf_write(q, buf_1, _P);
    }

    // verify result
    float psd[nfft];
    spgramcf_get_psd(q, psd);
    autotest_psd_s regions[] = {
        {.fmin=-0.5f,    .fmax=-0.6f*bw, .pmin=0,     .pmax=-_as+tol, .test_lo=0, .test_hi=1},
        {.fmin=-0.4f*bw, .fmax=+0.4f*bw, .pmin=0-tol, .pmax=  0 +tol, .test_lo=1, .test_hi=1},
        {.fmin=+0.6f*bw, .fmax=+0.5f,    .pmin=0,     .pmax=-_as+tol, .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_spectrum(psd, nfft, regions, 3,
        liquid_autotest_verbose ? "autotest/logs/rresamp_crcf.m" : NULL);

    // destroy objects
    rresamp_crcf_destroy(resamp);
    spgramcf_destroy(q);
    symstreamrcf_destroy(gen);
}

// actual tests
void autotest_rresamp_crcf_P1_Q5() { test_harness_rresamp_crcf( 1, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P2_Q5() { test_harness_rresamp_crcf( 2, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P3_Q5() { test_harness_rresamp_crcf( 3, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P6_Q5() { test_harness_rresamp_crcf( 6, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P8_Q5() { test_harness_rresamp_crcf( 8, 5, 15, 0.4f, 60.0f); }
void autotest_rresamp_crcf_P9_Q5() { test_harness_rresamp_crcf( 9, 5, 15, 0.4f, 60.0f); }

// test copy method
void autotest_rresamp_copy()
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
        rresamp_crcf_execute(q0, buf_0, buf_1);
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

