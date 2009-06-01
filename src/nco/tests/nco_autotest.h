/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifndef __AUTOTEST_NCO__
#define __AUTOTEST_NCO__

#include <complex.h>
#include "autotest/autotest.h"
#include "liquid.h"

//
// test floating point precision nco
//
void autotest_nco_basic() {
    struct nco_s nco_struct;
    nco p = &nco_struct;

    nco_init(p);
    unsigned int i;     // loop index
    float s, c;         // sine/cosine result
    float tol=1e-4f;    // error tolerance
    float f=0.0f;       // frequency to test

    nco_set_phase( p, 0.0f);
    CONTEND_DELTA( nco_cos(p), 1.0f, tol );
    CONTEND_DELTA( nco_sin(p), 0.0f, tol );
    nco_sincos(p, &s, &c);
    CONTEND_DELTA( s, 0.0f, tol );
    CONTEND_DELTA( c, 1.0f, tol );

    nco_set_phase(p, M_PI/2);
    CONTEND_DELTA( nco_cos(p), 0.0f, tol );
    CONTEND_DELTA( nco_sin(p), 1.0f, tol );
    nco_sincos(p, &s, &c);
    CONTEND_DELTA( s, 1.0f, tol );
    CONTEND_DELTA( c, 0.0f, tol );

    // cycle through one full period in 64 steps
    nco_set_phase(p, 0.0f);
    f = 2.0f * M_PI / 64.0f;
    nco_set_frequency(p, f);
    for (i=0; i<128; i++) {
        nco_sincos(p, &s, &c);
        CONTEND_DELTA( s, sinf(i*f), tol );
        CONTEND_DELTA( c, cosf(i*f), tol );
        nco_step(p);
    }

    // double frequency: cycle through one full period in 32 steps
    nco_set_phase(p, 0.0f);
    f = 2.0f * M_PI / 32.0f;
    nco_set_frequency(p, f);
    for (i=0; i<128; i++) {
        nco_sincos(p, &s, &c);
        CONTEND_DELTA( s, sinf(i*f), tol );
        CONTEND_DELTA( c, cosf(i*f), tol );
        nco_step(p);
    }

}

//
// test nco mixing
//
void autotest_nco_mixing() {
    // frequency, phase
    float f = 0.1f;
    float phi = M_PI;

    // error tolerance
    float tol = 0.00001f;

    // initialize nco object
    struct nco_s nco_struct;
    nco p = &nco_struct;
    nco_init(p);
    nco_set_frequency(p, f);
    nco_set_phase(p, phi);

    unsigned int i;
    float nco_i, nco_q;
    for (i=0; i<64; i++) {
        // generate sin/cos
        nco_sincos(p, &nco_q, &nco_i);

        // mix back to zero phase
        complex float nco_cplx_in = nco_i + _Complex_I*nco_q;
        complex float nco_cplx_out;
        nco_mix_down(p, nco_cplx_in, &nco_cplx_out);

        // assert mixer output is correct
        CONTEND_DELTA(crealf(nco_cplx_out), 1.0f, tol);
        CONTEND_DELTA(cimagf(nco_cplx_out), 0.0f, tol);

        // step nco
        nco_step(p);
    }
}

#endif // __AUTOTEST_NCO__

