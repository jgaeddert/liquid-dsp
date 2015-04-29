/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
    firinterp_crcf interp = firinterp_crcf_create_rnyquist(LIQUID_FIRFILT_ARKAISER,k,m,beta,0);

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

