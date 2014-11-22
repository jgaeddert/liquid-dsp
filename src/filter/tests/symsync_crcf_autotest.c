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

//
// symsync_crcf_autotest.c : test symbol timing synchronizer
//

#include "autotest/autotest.h"
#include "liquid.h"

void autotest_symsync_crcf()
{
    // options
    float        tol        =  0.2f;    // error tolerance
    unsigned int k          =  2;       // samples/symbol (input)
    unsigned int m          =  3;       // filter delay (symbols)
    float        beta       =  0.5f;    // filter excess bandwidth factor
    unsigned int num_filters= 32;       // number of filters in the bank

    unsigned int num_symbols_init=200;  // number of initial symbols
    unsigned int num_symbols_test=200;  // number of testing symbols

    liquid_firfilt_type ftype_tx = LIQUID_FIRFILT_ARKAISER;
    liquid_firfilt_type ftype_rx = LIQUID_FIRFILT_ARKAISER;

    float bt=0.02f;     // loop filter bandwidth
    float tau=-0.2f;    // fractional symbol offset
    float r = 1.00f;    // resampled rate

    // derived values
    unsigned int num_symbols = num_symbols_init + num_symbols_test;
    unsigned int num_samples = k*num_symbols;
    unsigned int num_samples_resamp = (unsigned int) ceilf(num_samples*r*1.1f) + 4;
    
    // compute delay
    while (tau < 0) tau += 1.0f;    // ensure positive tau
    float g = k*tau;                // number of samples offset
    int ds=floorf(g);               // additional symbol delay
    float dt = (g - (float)ds);     // fractional sample offset
    if (dt > 0.5f) {                // force dt to be in [0.5,0.5]
        dt -= 1.0f;
        ds++;
    }

    unsigned int i;

    // allocate arrays
    float complex s[num_symbols];       // data symbols
    float complex x[num_samples];       // interpolated samples
    float complex y[num_samples_resamp];// resampled data (resamp_crcf)
    float complex z[num_symbols + 64];  // synchronized symbols

    // generate pseudo-random QPSK symbols
    // NOTE: by using an m-sequence generator this sequence will be identical
    //       each time this test is run
    msequence ms = msequence_create_default(10);
    for (i=0; i<num_symbols; i++) {
        int si = msequence_generate_symbol(ms, 1);
        int sq = msequence_generate_symbol(ms, 1);
        s[i] = (si ? -1.0f : 1.0f) * M_SQRT1_2 +
               (sq ? -1.0f : 1.0f) * M_SQRT1_2 * _Complex_I;
    }
    msequence_destroy(ms);

    // 
    // create and run interpolator
    //

    // design interpolating filter
    firinterp_crcf interp = firinterp_crcf_create_rnyquist(ftype_tx,k,m,beta,dt);

    // interpolate block of samples
    firinterp_crcf_execute_block(interp, s, num_symbols, x);

    // destroy interpolator
    firinterp_crcf_destroy(interp);

    // 
    // run resampler
    //

#if 1
    // create resampler
    unsigned int resamp_len = 10*k; // resampling filter semi-length (filter delay)
    float resamp_bw = 0.45f;        // resampling filter bandwidth
    float resamp_As = 60.0f;        // resampling filter stop-band attenuation
    unsigned int resamp_npfb = 64;  // number of filters in bank
    resamp_crcf resamp = resamp_crcf_create(r, resamp_len, resamp_bw, resamp_As, resamp_npfb);

    // run resampler on block
    unsigned int ny;
    resamp_crcf_execute_block(resamp, x, num_samples, y, &ny);

    // destroy resampler
    resamp_crcf_destroy(resamp);
#else
    for (i=0; i<num_samples; i++)
        y[i] = x[i];
    unsigned int ny = num_samples;
#endif

    // 
    // create and run symbol synchronizer
    //

    // create symbol synchronizer
    symsync_crcf sync = symsync_crcf_create_rnyquist(ftype_rx, k, m, beta, num_filters);

    // set loop filter bandwidth
    symsync_crcf_set_lf_bw(sync,bt);

    // execute on entire block of samples
    unsigned int nz;
    symsync_crcf_execute(sync, y, ny, z, &nz);

    // destroy synchronizer
    symsync_crcf_destroy(sync);

    // compare (and print) results
    unsigned int delay = m + 10 + m;    // initial filter, resampler, matched filter
    printf("output symbols:\n");
    for (i=num_symbols_init; i<nz; i++) {
        // compute error
        float err = cabsf( z[i] - s[i-delay] );
        
        CONTEND_LESS_THAN( err, tol );

        if (liquid_autotest_verbose) {
            printf("  sym_out(%4u) = %8.4f + j*%8.4f; %% {%8.4f + j*%8.4f} e = %12.8f %s\n",
                    i+1,
                    crealf(z[i]      ), cimagf(z[i]      ),
                    crealf(s[i-delay]), cimagf(s[i-delay]),
                    err, err < tol ? "" : "*");
        }
    }

}

