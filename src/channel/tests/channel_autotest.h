#ifndef __AWGN_CHANNEL_AUTOTEST_H__
#define __AWGN_CHANNEL_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: AWGN channel
//
void autotest_awgn_channel()
{
    unsigned long int N=100000; // number of trials
    unsigned long int i;
    float complex x;
    float m1=0.0f, m2=0.0f;
    float nvar = 3.0f;
    float tol=0.01f;

    awgn_channel q = awgn_channel_create(nvar);

    // uniform
    for (i=0; i<N; i++) {
        awgn_channel_execute(q, 0.0, &x);
        m1 += crealf(x) + cimagf(x);
        m2 += crealf(x)*crealf(x) + cimagf(x)*cimagf(x);
    }
    N *= 2; // double N for real and imag components
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    CONTEND_DELTA(m1, 0.0f, tol);
    CONTEND_DELTA(m2, nvar, tol);

    awgn_channel_destroy(q);
}

#endif // __AWGN_CHANNEL_AUTOTEST_H__

