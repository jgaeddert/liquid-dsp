#ifndef __FRAMESYNC_AUTOTEST_H__
#define __FRAMESYNC_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: simple correlation
//
void autotest_framesync_rrrf_15()
{
    // generate sequence
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

#endif 

