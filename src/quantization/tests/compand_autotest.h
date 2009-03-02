#ifndef __LIQUID_COMPAND_AUTOTEST_H__
#define __LIQUID_COMPAND_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.internal.h"

void autotest_compand_float() {
    float x = -1.0f;
    float mu=255.0f;
    unsigned int n=30;

    float dx = 2/(float)(n);
    float y;
    float x_hat;
    float tol = 1e-6f;

    unsigned int i;
    for (i=0; i<n; i++) {
        y = compress_mulaw(x,mu);
        x_hat = expand_mulaw(y,mu);

        if (_autotest_verbose)
            printf("%8.4f -> %8.4f -> %8.4f\n", x, y, x_hat);

        CONTEND_DELTA(x,x_hat,tol);

        x += dx;
        x = (x > 1.0f) ? 1.0f : x;
    }
}


void autotest_compand_cfloat() {
    float complex x = -0.707f - 0.707f*_Complex_I;
    float mu=255.0f;
    unsigned int n=30;

    float complex dx = 2*(0.707f +0.707f* _Complex_I)/(float)(n);
    float complex y;
    float complex z;
    float tol = 1e-6f;

    unsigned int i;
    for (i=0; i<n; i++) {
        compress_cf_mulaw(x,mu,&y);
        expand_cf_mulaw(y,mu,&z);

        if (_autotest_verbose) {
            printf("%8.4f +j%8.4f > ", crealf(x), cimagf(x));
            printf("%8.4f +j%8.4f > ", crealf(y), cimagf(y));
            printf("%8.4f +j%8.4f\n",  crealf(z), cimagf(z));
        }

        CONTEND_DELTA(crealf(x),crealf(z),tol);
        CONTEND_DELTA(cimagf(x),cimagf(z),tol);

        x += dx;
        //x = (x > 1.0f) ? 1.0f : x;
    }
}


#endif // __LIQUID_COMPAND_AUTOTEST_H__

