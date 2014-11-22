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

// constant data sequence
const float eqrls_rrrf_autotest_data_sequence[64] = {
    -1.0, -1.0,  1.0, -1.0,  1.0, -1.0,  1.0, -1.0, 
    -1.0,  1.0,  1.0, -1.0, -1.0,  1.0, -1.0,  1.0, 
     1.0, -1.0, -1.0, -1.0,  1.0,  1.0, -1.0,  1.0, 
    -1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0, -1.0,
     1.0,  1.0, -1.0, -1.0,  1.0, -1.0,  1.0, -1.0, 
    -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 
     1.0, -1.0, -1.0, -1.0,  1.0,  1.0, -1.0,  1.0, 
    -1.0,  1.0,  1.0, -1.0,  1.0, -1.0,  1.0, -1.0
};

// 
// AUTOTEST: channel filter: delta with zero delay
//
void autotest_eqrls_rrrf_01()
{
    float tol=1e-2f;        // error tolerance

    // fixed parameters (do not change)
    unsigned int h_len=4;   // channel filter length
    unsigned int p=6;       // equalizer order
    unsigned int n=64;      // number of symbols to observe

    // bookkeeping variables
    float y[n];         // received data sequence (filtered by channel)
    //float d_hat[n];   // recovered data sequence
    float h[h_len];     // channel filter coefficients
    float w[p];         // equalizer filter coefficients
    unsigned int i;

    // create equalizer
    eqrls_rrrf eq = eqrls_rrrf_create(NULL, p);

    // create channel filter
    h[0] = 1.0f;
    for (i=1; i<h_len; i++)
        h[i] = 0.0f;
    firfilt_rrrf f = firfilt_rrrf_create(h,h_len);

    // data sequence
    float *d = (float*) eqrls_rrrf_autotest_data_sequence;

    // filter data signal through channel
    for (i=0; i<n; i++) {
        firfilt_rrrf_push(f,d[i]);
        firfilt_rrrf_execute(f,&y[i]);
    }

    // initialize weights, train equalizer
    for (i=0; i<p; i++)
        w[i] = 0;
    eqrls_rrrf_train(eq, w, y, d, n);

    // compare filter taps
    CONTEND_DELTA(w[0], 1.0f, tol);
    for (i=1; i<p; i++)
        CONTEND_DELTA(w[i], 0.0f, tol);

    // clean up objects
    firfilt_rrrf_destroy(f);
    eqrls_rrrf_destroy(eq);
}

