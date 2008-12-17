#ifndef __LIQUID_FIRHILB_AUTOTEST_H__
#define __LIQUID_FIRHILB_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/filter.h"

#define J _Complex_I

//
// AUTOTEST: 
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

    unsigned int i;
    for (i=0; i<16; i++)
        firhilb_decim_execute(ht, &x[2*i], &y[i]);

    if (_autotest_verbose) {
        printf("hilbert transform decimator output:\n");
        for (i=0; i<16; i++)
            printf("y(%3u) = %8.5f + j*%8.5f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    for (i=0; i<16; i++) {
        CONTEND_DELTA(crealf(y[i]), crealf(test[i]), tol);
        CONTEND_DELTA(cimagf(y[i]), cimagf(test[i]), tol);
    }

    firhilb_destroy(ht);
}

#endif // __LIQUID_FIRHILB_AUTOTEST_H__

