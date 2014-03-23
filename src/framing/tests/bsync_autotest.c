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

// 
// AUTOTEST: bsync_rrrf/simple correlation
//
void autotest_bsync_rrrf_15()
{
    // generate sequence (15-bit msequence)
    float h[15] = {
         1.0,  1.0,  1.0,  1.0, 
        -1.0,  1.0, -1.0,  1.0, 
         1.0, -1.0, -1.0,  1.0, 
        -1.0, -1.0, -1.0
    };
    float tol = 1e-3f;

    // generate synchronizer
    bsync_rrrf fs = bsync_rrrf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        bsync_rrrf_correlate(fs,h[i],&rxy);

    // correlation should be 1.0
    CONTEND_DELTA( rxy, 1.0f, tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        bsync_rrrf_correlate(fs,h[i],&rxy);
        CONTEND_DELTA( rxy, -1.0f/15.0f, tol );
    }

    // clean it up
    bsync_rrrf_destroy(fs);
}


// 
// AUTOTEST: bsync_crcf/simple correlation
//
void autotest_bsync_crcf_15()
{
    // generate sequence (15-bit msequence)
    float h[15] = {
         1.0,  1.0,  1.0,  1.0, 
        -1.0,  1.0, -1.0,  1.0, 
         1.0, -1.0, -1.0,  1.0, 
        -1.0, -1.0, -1.0
    };
    float tol = 1e-3f;

    // generate synchronizer
    bsync_crcf fs = bsync_crcf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float complex rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        bsync_crcf_correlate(fs,h[i],&rxy);

    // correlation should be 1.0  + j*(-1/15)
    CONTEND_DELTA( crealf(rxy),  1.0f,       tol );
    CONTEND_DELTA( cimagf(rxy), -1.0f/15.0f, tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        bsync_crcf_correlate(fs,h[i],&rxy);
        CONTEND_DELTA( crealf(rxy), -1.0f/15.0f, tol );
        CONTEND_DELTA( cimagf(rxy), -1.0f/15.0f, tol );
    }

    // clean it up
    bsync_crcf_destroy(fs);
}

// 
// AUTOTEST: bsync_crcf/simple correlation with phase
//           offset
//
void xautotest_bsync_crcf_phase_15()
{
    // generate sequence (15-bit msequence)
    float h[15] = {
         1.0,  1.0,  1.0,  1.0, 
        -1.0,  1.0, -1.0,  1.0, 
         1.0, -1.0, -1.0,  1.0, 
        -1.0, -1.0, -1.0
    };
    float tol = 1e-3f;
    float theta = 0.3f;

    // generate synchronizer
    bsync_crcf fs = bsync_crcf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float complex rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        bsync_crcf_correlate(fs,h[i]*cexpf(_Complex_I*theta),&rxy);

    // correlation should be 1.0
    CONTEND_DELTA( crealf(rxy), 1.0f*cosf(theta), tol );
    CONTEND_DELTA( cimagf(rxy), 1.0f*sinf(theta), tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        bsync_crcf_correlate(fs,h[i]*cexpf(_Complex_I*theta),&rxy);
        CONTEND_DELTA( crealf(rxy), -1.0f/15.0f*cosf(theta), tol );
        CONTEND_DELTA( cimagf(rxy), -1.0f/15.0f*sinf(theta), tol );
    }

    // clean it up
    bsync_crcf_destroy(fs);
}

