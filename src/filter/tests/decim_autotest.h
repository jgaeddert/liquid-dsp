#ifndef __LIQUID_DECIM_AUTOTEST_H__
#define __LIQUID_DECIM_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: 
//
void autotest_decim_rrrf_generic()
{
    float h[] = {0.5, 0.5, 0.5, 0.5};
    unsigned int h_len = 4; // filter length
    unsigned int D = 4;     // decim factor
    decim_rrrf q = decim_rrrf_create(D,h,h_len);

    float x[16] = { 
        0.5,  0.5,  0.5,  0.5,
       -0.5, -0.5, -0.5, -0.5,
        0.5,  0.5,  0.5,  0.5,
        0.5,  0.5,  0.5,  0.5};

    float y[4];
    float test[4] = {1, -1, 1, 1};
    float tol = 1e-6f;

    unsigned int i, n=0;
    for (i=0; i<4; i++) {
        decim_rrrf_execute(q, &x[n], &y[i], D-1);
        n+=D;
    }   

    for (i=0; i<4; i++) {
        CONTEND_DELTA(y[i], test[i], tol);
        
        if (_autotest_verbose)
            printf("  y(%u) = %8.4f;\n", i+1, y[i]);
    }

    decim_rrrf_destroy(q);
}

#endif // __LIQUID_DECIM_AUTOTEST_H__

