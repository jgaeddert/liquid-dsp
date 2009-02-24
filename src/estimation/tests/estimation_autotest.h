#ifndef __LIQUID_ESTIMATION_AUTOTEST_H__
#define __LIQUID_ESTIMATION_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// helper function to keep code base small
void test_freqoffset(float _dphi, unsigned int _n)
{
    float tol=1e-3f;

    float theta=0.0f;
    unsigned int i;
    float complex x[_n];
    for (i=0; i<_n; i++) {
        x[i] = cexpf(_Complex_I*theta);
        theta += _dphi;
    }

    float nu_hat = estimate_freqoffset(x,_n);
    if (_autotest_verbose)
        printf("nu_hat: %f (expected %f)\n", nu_hat, _dphi);

    CONTEND_DELTA( nu_hat, _dphi, tol );
}

void autotest_freqoffset_f0p0_n16() { test_freqoffset(0.0f, 16  ); }
void autotest_freqoffset_f0p1_n16() { test_freqoffset(0.1f, 16  ); }
void autotest_freqoffset_f1p3_n16() { test_freqoffset(1.3f, 16  ); }
void autotest_freqoffset_f2p7_n16() { test_freqoffset(2.7f, 16  ); }

#endif // __LIQUID_ESTIMATION_AUTOTEST_H__

