/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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

#include <complex.h>
#include "autotest/autotest.h"
#include "liquid.h"

//
// test floating point precision nco
//
void autotest_nco_basic() {
    nco_crcf p = nco_crcf_create(LIQUID_NCO);

    unsigned int i;     // loop index
    float s, c;         // sine/cosine result
    float tol=1e-4f;    // error tolerance
    float f=0.0f;       // frequency to test

    nco_crcf_set_phase( p, 0.0f);
    CONTEND_DELTA( nco_crcf_cos(p), 1.0f, tol );
    CONTEND_DELTA( nco_crcf_sin(p), 0.0f, tol );
    nco_crcf_sincos(p, &s, &c);
    CONTEND_DELTA( s, 0.0f, tol );
    CONTEND_DELTA( c, 1.0f, tol );

    nco_crcf_set_phase(p, M_PI/2);
    CONTEND_DELTA( nco_crcf_cos(p), 0.0f, tol );
    CONTEND_DELTA( nco_crcf_sin(p), 1.0f, tol );
    nco_crcf_sincos(p, &s, &c);
    CONTEND_DELTA( s, 1.0f, tol );
    CONTEND_DELTA( c, 0.0f, tol );

    // cycle through one full period in 64 steps
    nco_crcf_set_phase(p, 0.0f);
    f = 2.0f * M_PI / 64.0f;
    nco_crcf_set_frequency(p, f);
    for (i=0; i<128; i++) {
        nco_crcf_sincos(p, &s, &c);
        CONTEND_DELTA( s, sinf(i*f), tol );
        CONTEND_DELTA( c, cosf(i*f), tol );
        nco_crcf_step(p);
    }

    // double frequency: cycle through one full period in 32 steps
    nco_crcf_set_phase(p, 0.0f);
    f = 2.0f * M_PI / 32.0f;
    nco_crcf_set_frequency(p, f);
    for (i=0; i<128; i++) {
        nco_crcf_sincos(p, &s, &c);
        CONTEND_DELTA( s, sinf(i*f), tol );
        CONTEND_DELTA( c, cosf(i*f), tol );
        nco_crcf_step(p);
    }

}

//
// test nco mixing
//
void autotest_nco_mixing() {
    // frequency, phase
    float f = 0.1f;
    float phi = M_PI;

    // error tolerance (high for NCO)
    float tol = 0.05f;

    // initialize nco object
    nco_crcf p = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_frequency(p, f);
    nco_crcf_set_phase(p, phi);

    unsigned int i;
    float nco_i, nco_q;
    for (i=0; i<64; i++) {
        // generate sin/cos
        nco_crcf_sincos(p, &nco_q, &nco_i);

        // mix back to zero phase
        complex float nco_cplx_in = nco_i + _Complex_I*nco_q;
        complex float nco_cplx_out;
        nco_crcf_mix_down(p, nco_cplx_in, &nco_cplx_out);

        // assert mixer output is correct
        CONTEND_DELTA(crealf(nco_cplx_out), 1.0f, tol);
        CONTEND_DELTA(cimagf(nco_cplx_out), 0.0f, tol);
        //printf("%3u : %12.8f + j*%12.8f\n", i, crealf(nco_cplx_out), cimagf(nco_cplx_out));

        // step nco
        nco_crcf_step(p);
    }
}

