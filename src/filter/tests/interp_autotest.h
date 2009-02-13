#ifndef __INTERP_AUTOTEST_H__
#define __INTERP_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: 
//
void autotest_interp_generic()
{
    float h[] = {0, 0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25, 0}; 
    unsigned int h_len = 9; // filter length
    unsigned int M = 4;     // interp factor
    interp_rrrf q = interp_rrrf_create(4,h,h_len);

    float x[] = {1.0, -1.0, 1.0, 1.0};
    float y[16];
    float test[16] = {
         0.00,  0.25,  0.50,  0.75,
         1.00,  0.50,  0.00, -0.50,
        -1.00, -0.50,  0.00,  0.50,
         1.00,  1.00,  1.00,  1.00};
    float tol = 1e-6;

    unsigned int i, n=0;
    for (i=0; i<4; i++) {
        interp_rrrf_execute(q, x[i], &y[n]);
        n+=M;
    }   

    for (i=0; i<16; i++) {
        CONTEND_DELTA(y[i], test[i], tol);

        if (_autotest_verbose)
            printf("  y(%u) = %8.4f;\n", i+1, y[i]);
    }

    if (_autotest_verbose)
        interp_rrrf_print(q);

    interp_rrrf_destroy(q);
}

#endif 

