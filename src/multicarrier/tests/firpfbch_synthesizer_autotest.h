/*
 * Copyright (c) 2007, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2010 Virginia Polytechnic Institute & State University
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

#include <assert.h>
#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: validate synthesis correctness
//
void autotest_firpfbch_crcf_synthesis()
{
    // options
    float tol = 1e-4f;              // error tolerance
    unsigned int num_channels=4;    // number of channels
    unsigned int p=5;               // filter length (symbols)
    unsigned int num_symbols=12;    // number of symbols

    // derived values
    unsigned int num_samples = num_channels * num_symbols;

    unsigned int i;
    unsigned int j;

    // generate filter
    // NOTE : these coefficients can be random; the purpose of this
    //        exercise is to demonstrate mathematical equivalence
    unsigned int h_len = p*num_channels;
    float h[h_len];
    for (i=0; i<h_len; i++) h[i] = randnf();
    //for (i=0; i<h_len; i++) h[i] = 0.1f*i;
    //for (i=0; i<h_len; i++) h[i] = (i<=m) ? 1.0f : 0.0f;
    //for (i=0; i<h_len; i++) h[i] = 1.0f;

    // create filter object
    firfilt_crcf f = firfilt_crcf_create(h, h_len);

    // create filterbank channelizer object
    firpfbch_crcf q = firpfbch_crcf_create(LIQUID_SYNTHESIZER, num_channels, p, h);

    float complex Y[num_symbols][num_channels];     // channelized input
    float complex y0[num_samples];                  // time-domain output
    float complex y1[num_samples];                  // time-domain output

    // generate input sequence (complex noise)
    for (i=0; i<num_symbols; i++) {
        for (j=0; j<num_channels; j++)
            Y[i][j] = randnf() * cexpf(_Complex_I*randf()*2*M_PI);
    }

    // 
    // run synthesis filter bank
    //

    for (i=0; i<num_symbols; i++)
        firpfbch_crcf_synthesizer_execute(q, &Y[i][0], &y0[i*num_channels]);

    // 
    // run traditional up-converter (inefficient)
    //

    // clear output array
    for (i=0; i<num_samples; i++)
        y1[i] = 0.0f;

    unsigned int n;
    float dphi; // carrier frequency
    float complex y_hat;
    for (i=0; i<num_channels; i++) {
        // reset filter
        firfilt_crcf_clear(f);

        // set center frequency
        dphi = 2.0f * M_PI * (float)i / (float)num_channels;

        // reset input symbol counter
        n=0;

        for (j=0; j<num_samples; j++) {

            // interpolate sequence
            if ( (j%num_channels)==0 ) {
                assert(n<num_symbols);
                firfilt_crcf_push(f, Y[n][i]);
                n++;
            } else {
                firfilt_crcf_push(f, 0);
            }
            firfilt_crcf_execute(f, &y_hat);

            // accumulate up-converted sample
            y1[j] += y_hat * cexpf(_Complex_I*j*dphi);
        }
        assert(n==num_symbols);
    }

    // destroy objects
    firfilt_crcf_destroy(f);
    firpfbch_crcf_destroy(q);


    // 
    // print channelizer outputs
    //
    if (liquid_autotest_verbose) {
        printf("\n");
        printf("output: filterbank:             traditional:\n");
        for (i=0; i<num_samples; i++) {
            printf("%3u: %10.5f+%10.5fj  %10.5f+%10.5fj\n",
                i,
                crealf(y0[i]), cimagf(y0[i]),
                crealf(y1[i]), cimagf(y1[i]));
        }
    }


    // 
    // compare results
    // 
    for (i=0; i<num_samples; i++) {
        CONTEND_DELTA( crealf(y0[i]), crealf(y1[i]), tol );
        CONTEND_DELTA( cimagf(y0[i]), cimagf(y1[i]), tol );
    }
}

