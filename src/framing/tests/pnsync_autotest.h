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

#ifndef __PNSYNC_AUTOTEST_H__
#define __PNSYNC_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: pnsync_rrrf/simple correlation
//
void autotest_pnsync_rrrf_15()
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
    pnsync_rrrf fs = pnsync_rrrf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        pnsync_rrrf_correlate(fs,h[i],&rxy);

    // correlation should be 1.0
    CONTEND_DELTA( rxy, 1.0f, tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        pnsync_rrrf_correlate(fs,h[i],&rxy);
        CONTEND_DELTA( rxy, -1.0f/15.0f, tol );
    }

    // clean it up
    pnsync_rrrf_destroy(fs);
}

// 
// AUTOTEST: pnsync_crcf/simple correlation
//
void autotest_pnsync_crcf_15()
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
    pnsync_crcf fs = pnsync_crcf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float complex rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        pnsync_crcf_correlate(fs,h[i],&rxy);

    // correlation should be 1.0
    CONTEND_DELTA( crealf(rxy), 1.0f, tol );
    CONTEND_DELTA( cimagf(rxy), 0.0f, tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        pnsync_crcf_correlate(fs,h[i],&rxy);
        CONTEND_DELTA( crealf(rxy), -1.0f/15.0f, tol );
        CONTEND_DELTA( cimagf(rxy), 0.0f,        tol );
    }

    // clean it up
    pnsync_crcf_destroy(fs);
}

// 
// AUTOTEST: pnsync_crcf/simple correlation with phase
//           offset
//
void autotest_pnsync_crcf_phase_15()
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
    pnsync_crcf fs = pnsync_crcf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float complex rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        pnsync_crcf_correlate(fs,h[i]*cexpf(_Complex_I*theta),&rxy);

    // correlation should be 1.0
    CONTEND_DELTA( crealf(rxy), 1.0f*cosf(theta), tol );
    CONTEND_DELTA( cimagf(rxy), 1.0f*sinf(theta), tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        pnsync_crcf_correlate(fs,h[i]*cexpf(_Complex_I*theta),&rxy);
        CONTEND_DELTA( crealf(rxy), -1.0f/15.0f*cosf(theta), tol );
        CONTEND_DELTA( cimagf(rxy), -1.0f/15.0f*sinf(theta), tol );
    }

    // clean it up
    pnsync_crcf_destroy(fs);
}

#endif 

