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

// 
// AUTOTEST: static channel filter, blind equalization on QPSK symbols
//
void autotest_eqlms_cccf_blind()
{
    // parameters
    float           tol         = 2e-2f;    // error tolerance
    unsigned int    k           =  2;       // samples/symbol
    unsigned int    m           =  7;       // filter delay
    float           beta        =  0.3f;    // excess bandwidth factor
    unsigned int    p           =  7;       // equalizer order
    float           mu          =  0.7f;    // equalizer bandwidth
    unsigned int    num_symbols = 400;      // number of symbols to observe

    // create sequence generator for repeatability
    msequence ms = msequence_create_default(12);

    // create interpolating filter
    firinterp_crcf interp = firinterp_crcf_create_prototype(LIQUID_FIRFILT_ARKAISER,k,m,beta,0);

    // create equalizer
    eqlms_cccf eq = eqlms_cccf_create_rnyquist(LIQUID_FIRFILT_ARKAISER,k,p,beta,0);
    eqlms_cccf_set_bw(eq, mu);

    // create channel filter
    float complex h[5] = {
        { 1.00f,  0.00f},
        { 0.00f, -0.01f},
        {-0.11f,  0.02f},
        { 0.02f,  0.01f},
        {-0.09f, -0.04f} };
    firfilt_cccf fchannel = firfilt_cccf_create(h,5);

    // arrays
    float complex buf[k];               // filter buffer
    float complex sym_in [num_symbols]; // input symbols
    float complex sym_out[num_symbols]; // equalized symbols

    // run equalization
    unsigned int i;
    unsigned int j;
    for (i=0; i<num_symbols; i++) {
        // generate input symbol
        sym_in[i] = ( msequence_advance(ms) ? M_SQRT1_2 : -M_SQRT1_2 ) +
                    ( msequence_advance(ms) ? M_SQRT1_2 : -M_SQRT1_2 ) * _Complex_I;

        // interpolate
        firinterp_crcf_execute(interp, sym_in[i], buf);

        // apply channel filter (in place)
        firfilt_cccf_execute_block(fchannel, buf, k, buf);

        // apply equalizer as filter
        for (j=0; j<k; j++) {
            eqlms_cccf_push(eq, buf[j]);

            // decimate by k
            if ( (j%k) != 0 ) continue;

            eqlms_cccf_execute(eq, &sym_out[i]);

            // update equalization (blind operation)
            if (i > m + p)
                eqlms_cccf_step(eq, sym_out[i]/cabsf(sym_out[i]), sym_out[i]);
        }
    }

    // compare input, output
    unsigned int settling_delay = 285;
    for (i=m+p; i<num_symbols; i++) {
        // compensate for delay
        j = i-m-p;

        // absolute error
        float error = cabsf(sym_in[j]-sym_out[i]);

        if (liquid_autotest_verbose) {
            printf("x[%3u] = {%12.8f,%12.8f}, y[%3u] = {%12.8f,%12.8f}, error=%12.8f %s\n",
                    j, crealf(sym_in [j]), cimagf(sym_in [j]),
                    i, crealf(sym_out[i]), cimagf(sym_out[i]),
                    error, error > tol ? "*" : "");
            if (i == settling_delay + m + p)
                printf("--- start of test ---\n");
        }

        // check error
        if (i > settling_delay + m + p)
            CONTEND_DELTA(error, 0.0f, tol);
    }

    // clean up objects
    firfilt_cccf_destroy(fchannel);
    eqlms_cccf_destroy(eq);
    msequence_destroy(ms);
}

// 
// AUTOTEST: static channel filter, decision-directed on QPSK symbols
//
void autotest_eqlms_cccf_decisiondirected()
{
    // parameters
    float           tol         = 1e-2f;    // error tolerance
    unsigned int    k           =  2;       // samples/symbol
    unsigned int    m           =  7;       // filter delay
    float           beta        =  0.3f;    // excess bandwidth factor
    unsigned int    p           =  7;       // equalizer order
    unsigned int    num_symbols = 400;      // number of symbols to observe

    // create sequence generator for repeatability
    msequence ms = msequence_create_default(12);

    // create interpolating filter
    firinterp_crcf interp = firinterp_crcf_create_prototype(LIQUID_FIRFILT_ARKAISER,k,m,beta,0);

    // create equalizer
    eqlms_cccf eq = eqlms_cccf_create_rnyquist(LIQUID_FIRFILT_ARKAISER,k,p,beta,0);
    eqlms_cccf_set_bw(eq, 0.5f);

    // create channel filter
    unsigned int h_len = 5; // channel filter length
    float complex h[5] = {1.0f, 0.0f, -0.1f, 0.02f, -0.1f};
    firfilt_cccf fchannel = firfilt_cccf_create(h,h_len);

    // arrays
    float complex buf[k];               // filter buffer
    float complex sym_in [num_symbols]; // input symbols
    float complex sym_out[num_symbols]; // equalized symbols

    // run equalization
    unsigned int i;
    unsigned int j;
    for (i=0; i<num_symbols; i++) {
        // generate input symbol
        sym_in[i] = ( msequence_advance(ms) ? M_SQRT1_2 : -M_SQRT1_2 ) +
                    ( msequence_advance(ms) ? M_SQRT1_2 : -M_SQRT1_2 ) * _Complex_I;

        // interpolate
        firinterp_crcf_execute(interp, sym_in[i], buf);

        // apply channel filter (in place)
        firfilt_cccf_execute_block(fchannel, buf, k, buf);

        // apply equalizer as filter
        for (j=0; j<k; j++) {
            eqlms_cccf_push(eq, buf[j]);

            // decimate by k
            if ( (j%k) != 0 ) continue;

            eqlms_cccf_execute(eq, &sym_out[i]);

            // update equalization (blind operation)
            if (i > m + p)
                eqlms_cccf_step(eq, sym_out[i]/cabsf(sym_out[i]), sym_out[i]);
        }
    }

    // compare input, output
    unsigned int settling_delay = 250;
    for (i=m+p; i<num_symbols; i++) {
        // compensate for delay
        j = i-m-p;

        // absolute error
        float error = cabsf(sym_in[j]-sym_out[i]);

        if (liquid_autotest_verbose) {
            printf("x[%3u] = {%12.8f,%12.8f}, y[%3u] = {%12.8f,%12.8f}, error=%12.8f %s\n",
                    j, crealf(sym_in [j]), cimagf(sym_in [j]),
                    i, crealf(sym_out[i]), cimagf(sym_out[i]),
                    error, error > tol ? "*" : "");
            if (i == settling_delay + m + p)
                printf("--- start of test ---\n");
        }

        // check error
        if (i > settling_delay + m + p)
            CONTEND_DELTA(error, 0.0f, tol);
    }

    // clean up objects
    firfilt_cccf_destroy(fchannel);
    eqlms_cccf_destroy(eq);
    msequence_destroy(ms);
}

