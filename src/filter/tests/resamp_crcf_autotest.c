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

// test single-stage arbitrary resampler
//   r  : resampling rate (output/input)
//   As : resampling filter stop-band attenuation [dB]
void testbench_resamp_crcf(float r, float As)
{
    // options
    unsigned int n=800000;      // number of output samples to analyze
    float bw = 0.2f; // target output bandwidth
    unsigned int nfft = 800;
    float tol = 0.5f;
    unsigned int m = 15, npfb = 256;
    float fc = 0.45f;

    // create and configure objects
    spgramcf     q   = spgramcf_create(nfft, LIQUID_WINDOW_HANN, nfft/2, nfft/4);
    symstreamrcf gen = symstreamrcf_create_linear(LIQUID_FIRFILT_KAISER,r*bw,25,0.2f,LIQUID_MODEM_QPSK);
    symstreamrcf_set_gain(gen, sqrtf(bw));
    resamp_crcf resamp = resamp_crcf_create(r,m,fc,As,npfb);

    // generate samples and push through spgram object
    unsigned int  buf_len = 256;
    float complex buf_0[buf_len]; // input buffer
    float complex buf_1[buf_len]; // output buffer
    while (spgramcf_get_num_samples_total(q) < n) {
        // generate block of samples
        symstreamrcf_write_samples(gen, buf_0, buf_len);

        // resample
        unsigned int nw = 0;
        resamp_crcf_execute_block(resamp, buf_0, buf_len, buf_1, &nw);

        // run samples through the spgram object
        spgramcf_write(q, buf_1, nw);
    }

    // verify result
    float psd[nfft];
    spgramcf_get_psd(q, psd);
    autotest_psd_s regions[] = {
        {.fmin=-0.5f,    .fmax=-0.6f*bw, .pmin=0,     .pmax=-As+tol, .test_lo=0, .test_hi=1},
        {.fmin=-0.4f*bw, .fmax=+0.4f*bw, .pmin=0-tol, .pmax= 0 +tol, .test_lo=1, .test_hi=1},
        {.fmin=+0.6f*bw, .fmax=+0.5f,    .pmin=0,     .pmax=-As+tol, .test_lo=0, .test_hi=1},
    };
    liquid_autotest_validate_spectrum(psd, nfft, regions, 3,
        liquid_autotest_verbose ? "autotest/logs/resamp_crcf.m" : NULL);

    // destroy objects
    spgramcf_destroy(q);
    symstreamrcf_destroy(gen);
    resamp_crcf_destroy(resamp);
}

void autotest_resamp_crcf_01() { testbench_resamp_crcf(0.127115323f, 60.0f); }
void autotest_resamp_crcf_02() { testbench_resamp_crcf(0.373737373f, 60.0f); }
void autotest_resamp_crcf_03() { testbench_resamp_crcf(0.676543210f, 60.0f); }
//void xautotest_resamp_crcf_04() { testbench_resamp_crcf(0.127115323f,80.0f); }

// test arbitrary resampler output length calculation
void testbench_resamp_crcf_num_output(float _rate, unsigned int _npfb)
{
    if (liquid_autotest_verbose)
        printf("testing resamp_crcf_get_num_output() with r=%g, npfb=%u\n", _rate, _npfb);

    // create object
    float fc = 0.4f;
    float As = 60.0f;
    unsigned int m = 20;
    resamp_crcf q = resamp_crcf_create(_rate, m, fc, As, _npfb);

    // sizes to test in sequence
    unsigned int sizes[10] = {1, 2, 3, 20, 7, 64, 4, 4, 4, 27};

    // allocate buffers (over-provision output to help avoid segmentation faults on error)
    unsigned int max_input = 64;
    unsigned int max_output = 16 + (unsigned int)(4.0f * max_input * _rate);
    printf("max_input : %u, max_output : %u\n", max_input, max_output);
    float complex buf_0[max_input];
    float complex buf_1[max_output];
    unsigned int i;
    for (i=0; i<max_input; i++)
        buf_0[i] = 0.0f;

    // run numerous blocks
    unsigned int b;
    for (b=0; b<8; b++) {
        for (i=0; i<10; i++) {
            unsigned int num_input  = sizes[i];
            unsigned int num_output = resamp_crcf_get_num_output(q, num_input);
            unsigned int num_written;
            resamp_crcf_execute_block(q, buf_0, num_input, buf_1, &num_written);
            if (liquid_autotest_verbose) {
                printf(" b[%2u][%2u], num_input:%5u, num_output:%5u, num_written:%5u\n",
                        b, i, num_input, num_output, num_written);
            }
            CONTEND_EQUALITY(num_output, num_written)
        }
    }

    // destroy object
    resamp_crcf_destroy(q);
}

void autotest_resamp_crcf_num_output_0(){ testbench_resamp_crcf_num_output(1.00f,     64); }
void autotest_resamp_crcf_num_output_1(){ testbench_resamp_crcf_num_output(1.00f,    256); }
void autotest_resamp_crcf_num_output_2(){ testbench_resamp_crcf_num_output(0.50f,    256); }
void autotest_resamp_crcf_num_output_3(){ testbench_resamp_crcf_num_output(sqrtf( 2),256); }
void autotest_resamp_crcf_num_output_4(){ testbench_resamp_crcf_num_output(sqrtf(17), 16); }
void autotest_resamp_crcf_num_output_5(){ testbench_resamp_crcf_num_output(1.0f/M_PI, 64); }
void autotest_resamp_crcf_num_output_6(){ testbench_resamp_crcf_num_output(expf(5.0f),64); }
void autotest_resamp_crcf_num_output_7(){ testbench_resamp_crcf_num_output(expf(-5.f),64); }

// test copy method
void autotest_resamp_crcf_copy()
{
    // create object with irregular parameters
    float rate = 0.71239213987520f;
    resamp_crcf q0 = resamp_crcf_create(rate, 17, 0.37f, 60.0f, 64);

    // run samples through filter
    unsigned int i, nw0, nw1, num_samples = 80;
    float complex y0, y1;
    for (i=0; i<num_samples; i++) {
        float complex v = randnf() + _Complex_I*randnf();
        resamp_crcf_execute(q0, v, &y0, &nw0);
    }

    // copy object
    resamp_crcf q1 = resamp_crcf_copy(q0);

    // run samples through both filters and check equality
    for (i=0; i<num_samples; i++) {
        float complex v = randnf() + _Complex_I*randnf();
        resamp_crcf_execute(q0, v, &y0, &nw0);
        resamp_crcf_execute(q1, v, &y1, &nw1);

        // check that either 0 or 1 samples were written
        CONTEND_LESS_THAN(nw0, 2);
        CONTEND_LESS_THAN(nw1, 2);

        // check that the same number of samples were written
        CONTEND_EQUALITY(nw0, nw1);

        // check output sample values
        if (nw0==1 && nw1==1)
            CONTEND_EQUALITY(y0, y1);
    }

    // destroy objects
    resamp_crcf_destroy(q0);
    resamp_crcf_destroy(q1);
}

