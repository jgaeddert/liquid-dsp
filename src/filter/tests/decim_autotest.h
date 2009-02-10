#ifndef __DECIM_AUTOTEST_H__
#define __DECIM_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: old decim function
//
#if 0
void xautotest_decim_generic()
{
    decim d = decim_create(2, 0.0f, 0.1f, 40.0f);

    if (_autotest_verbose)
        decim_debug_print(d);

    CONTEND_LESS_THAN(d->h_len,FIR_FILTER_LEN_MAX+1);
    CONTEND_EQUALITY(d->fc, 0.0f);

    decim_destroy(d);
}
#endif

//
// AUTOTEST: 
//
void autotest_decim_generic()
{
    float h[] = {0.5, 0.5, 0.5, 0.5};
    unsigned int h_len = 4; // filter length
    unsigned int D = 4;     // decim factor
    decim q = decim_create(D,h,h_len);

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
        decim_execute(q, &x[n], &y[i], D-1);
        n+=D;
    }   

    for (i=0; i<4; i++) {
        CONTEND_DELTA(y[i], test[i], tol);
        
        if (_autotest_verbose)
            printf("  y(%u) = %8.4f;\n", i+1, y[i]);
    }

    decim_destroy(q);
}

#endif 

