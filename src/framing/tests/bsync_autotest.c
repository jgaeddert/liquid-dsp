/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include "liquid.autotest.h"
#include "liquid.h"

LIQUID_AUTOTEST(bsync_rrrf_15,"bsync_rrrf/simple correlation","",0.1)
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
    LIQUID_CHECK_DELTA( rxy, 1.0f, tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        bsync_rrrf_correlate(fs,h[i],&rxy);
        LIQUID_CHECK_DELTA( rxy, -1.0f/15.0f, tol );
    }

    // clean it up
    bsync_rrrf_destroy(fs);
}

LIQUID_AUTOTEST(bsync_crcf_15,"bsync_crcf/simple correlation","",0.1)
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
    liquid_float_complex rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        bsync_crcf_correlate(fs,h[i],&rxy);

    // correlation should be 1.0  + j*(-1/15)
    LIQUID_CHECK_DELTA( crealf(rxy),  1.0f,       tol );
    LIQUID_CHECK_DELTA( cimagf(rxy), -1.0f/15.0f, tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        bsync_crcf_correlate(fs,h[i],&rxy);
        LIQUID_CHECK_DELTA( crealf(rxy), -1.0f/15.0f, tol );
        LIQUID_CHECK_DELTA( cimagf(rxy), -1.0f/15.0f, tol );
    }

    // clean it up
    bsync_crcf_destroy(fs);
}

LIQUID_AUTOTEST(bsync_crcf_phase_15,"bsync_crcf/simple correlation with phase offset","",0.1)
{
    /*
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
    liquid_float_complex rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        bsync_crcf_correlate(fs,h[i]*cexpf(_Complex_I*theta),&rxy);

    // correlation should be 1.0
    LIQUID_CHECK_DELTA( crealf(rxy), 1.0f*cosf(theta), tol );
    LIQUID_CHECK_DELTA( cimagf(rxy), 1.0f*sinf(theta), tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        bsync_crcf_correlate(fs,h[i]*cexpf(_Complex_I*theta),&rxy);
        LIQUID_CHECK_DELTA( crealf(rxy), -1.0f/15.0f*cosf(theta), tol );
        LIQUID_CHECK_DELTA( cimagf(rxy), -1.0f/15.0f*sinf(theta), tol );
    }

    // clean it up
    bsync_crcf_destroy(fs);
    */
}

