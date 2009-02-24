#ifndef __LIQUID_ESTIMATION_AUTOTEST_H__
#define __LIQUID_ESTIMATION_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: frequency offset
//
void autotest_freqoffset_f0_n16()
{
    float tol=1e-3f;
    float dphi = 0.0f;
    unsigned int n=16;

    float theta=0.0f;
    unsigned int i;
    float complex x[n];
    for (i=0; i<n; i++) {
        x[i] = cexpf(_Complex_I*theta);
        theta += dphi;
    }

    float nu_hat = estimate_freqoffset(x,n);
    if (_autotest_verbose)
        printf("nu_hat: %f (expected %f)\n", nu_hat, dphi);

    CONTEND_DELTA( nu_hat, dphi, tol );
}

#endif // __LIQUID_ESTIMATION_AUTOTEST_H__

