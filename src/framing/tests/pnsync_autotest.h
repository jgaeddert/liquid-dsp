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

