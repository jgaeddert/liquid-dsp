#ifndef __LIQUID_FIRHILB_AUTOTEST_H__
#define __LIQUID_FIRHILB_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

#define J _Complex_I

//
// AUTOTEST: Hilbert transform, 2:1 decimator
//
void autotest_firhilb_decim()
{
    float x[32] = {
         1.0000,  0.7071,  0.0000, -0.7071, -1.0000, -0.7071, -0.0000,  0.7071,
         1.0000,  0.7071,  0.0000, -0.7071, -1.0000, -0.7071, -0.0000,  0.7071,
         1.0000,  0.7071,  0.0000, -0.7071, -1.0000, -0.7071, -0.0000,  0.7071,
         1.0000,  0.7071, -0.0000, -0.7071, -1.0000, -0.7071, -0.0000,  0.7071
    };

    float complex test[16] = {
         0.0000+J*-0.0055,  0.0000+J*-0.0231,  0.0000+J*-0.0605,  0.0000+J*-0.1459,
         0.0000+J*-0.5604,  0.7071+J* 0.7669, -0.7071+J* 0.7294, -0.7071+J*-0.7008,
         0.7071+J*-0.7064,  0.7071+J* 0.7064, -0.7071+J* 0.7064, -0.7071+J*-0.7064,
         0.7071+J*-0.7064,  0.7071+J* 0.7064, -0.7071+J* 0.7064, -0.7071+J*-0.7064
    };

    float complex y[16];
    firhilb ht = firhilb_create(21);
    float tol=0.001f;

    // run decimator
    unsigned int i;
    for (i=0; i<16; i++)
        firhilb_decim_execute(ht, &x[2*i], &y[i]);

    if (_autotest_verbose) {
        printf("hilbert transform decimator output:\n");
        for (i=0; i<16; i++)
            printf("  y(%3u) = %8.5f + j*%8.5f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // run validation
    for (i=0; i<16; i++) {
        CONTEND_DELTA(crealf(y[i]), crealf(test[i]), tol);
        CONTEND_DELTA(cimagf(y[i]), cimagf(test[i]), tol);
    }

    // clean up filter object
    firhilb_destroy(ht);
}

//
// AUTOTEST: Hilbert transform, 1:2 interpolator
//
void autotest_firhilb_interp()
{
    float complex x[16] = {
         1.0000+J* 0.0000,  0.0000+J* 1.0000, -1.0000+J* 0.0000, -0.0000+J*-1.0000,
         1.0000+J*-0.0000,  0.0000+J* 1.0000, -1.0000+J* 0.0000, -0.0000+J*-1.0000,
         1.0000+J*-0.0000,  0.0000+J* 1.0000, -1.0000+J* 0.0000, -0.0000+J*-1.0000,
         1.0000+J*-0.0000, -0.0000+J* 1.0000, -1.0000+J* 0.0000, -0.0000+J*-1.0000
    };

    float test[32] = {
         0.0000, -0.0055, -0.0000, -0.0231, -0.0000, -0.0605, -0.0000, -0.1459,
        -0.0000, -0.5604, -0.0000,  0.7669,  1.0000,  0.7294,  0.0000, -0.7008,
        -1.0000, -0.7064, -0.0000,  0.7064,  1.0000,  0.7064,  0.0000, -0.7064,
        -1.0000, -0.7064, -0.0000,  0.7064,  1.0000,  0.7064,  0.0000, -0.7064
    };


    float y[32];
    firhilb ht = firhilb_create(21);
    float tol=0.001f;

    // run interpolator
    unsigned int i;
    for (i=0; i<16; i++)
        firhilb_interp_execute(ht, x[i], &y[2*i]);

    if (_autotest_verbose) {
        printf("hilbert transform interpolator output:\n");
        for (i=0; i<32; i++)
            printf("  y(%3u) = %8.5f;\n", i+1, y[i]);
    }

    // run validation
    for (i=0; i<32; i++) {
        CONTEND_DELTA(y[i], test[i], tol);
    }

    // clean up filter object
    firhilb_destroy(ht);
}

#endif // __LIQUID_FIRHILB_AUTOTEST_H__

