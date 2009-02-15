#ifndef __FRAMESYNC_AUTOTEST_H__
#define __FRAMESYNC_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: framesync_rrrf/simple correlation
//
void autotest_framesync_rrrf_15()
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
    framesync_rrrf fs = framesync_rrrf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        rxy = framesync_rrrf_correlate(fs,h[i]);

    // correlation should be 1.0
    CONTEND_DELTA( rxy, 1.0f, tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        rxy = framesync_rrrf_correlate(fs,h[i]);
        CONTEND_DELTA( rxy, -1.0f/15.0f, tol );
    }

    // clean it up
    framesync_rrrf_destroy(fs);
}

// 
// AUTOTEST: framesync_crcf/simple correlation
//
void autotest_framesync_crcf_15()
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
    framesync_crcf fs = framesync_crcf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float complex rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        rxy = framesync_crcf_correlate(fs,h[i]);

    // correlation should be 1.0
    CONTEND_DELTA( crealf(rxy), 1.0f, tol );
    CONTEND_DELTA( cimagf(rxy), 0.0f, tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        rxy = framesync_crcf_correlate(fs,h[i]);
        CONTEND_DELTA( crealf(rxy), -1.0f/15.0f, tol );
        CONTEND_DELTA( cimagf(rxy), 0.0f,        tol );
    }

    // clean it up
    framesync_crcf_destroy(fs);
}

// 
// AUTOTEST: framesync_crcf/simple correlation with phase
//           offset
//
void autotest_framesync_crcf_phase_15()
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
    framesync_crcf fs = framesync_crcf_create(15,h);

    // 
    // run tests
    //
    unsigned int i;
    float complex rxy;

    // fill buffer with sequence
    for (i=0; i<15; i++)
        rxy = framesync_crcf_correlate(fs,h[i]*cexpf(_Complex_I*theta));

    // correlation should be 1.0
    CONTEND_DELTA( crealf(rxy), 1.0f*cosf(theta), tol );
    CONTEND_DELTA( cimagf(rxy), 1.0f*sinf(theta), tol );

    // all other cross-correlations should be exactly -1/15
    for (i=0; i<14; i++) {
        rxy = framesync_crcf_correlate(fs,h[i]*cexpf(_Complex_I*theta));
        CONTEND_DELTA( crealf(rxy), -1.0f/15.0f*cosf(theta), tol );
        CONTEND_DELTA( cimagf(rxy), -1.0f/15.0f*sinf(theta), tol );
    }

    // clean it up
    framesync_crcf_destroy(fs);
}

#endif 

