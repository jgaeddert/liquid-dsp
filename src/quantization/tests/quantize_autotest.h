#ifndef __LIQUID_QUANTIZE_AUTOTEST_H__
#define __LIQUID_QUANTIZE_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.internal.h"

void autotest_quantize_float_n8() {
    float x = -1.0f;
    unsigned int num_steps=30;
    unsigned int num_bits=8;

    float dx = 2/(float)(num_steps);
    unsigned int q;
    float x_hat;
    float tol = 1e-2f;

    unsigned int i;
    for (i=0; i<num_steps; i++) {
        q = quantize_adc(x,num_bits);
        x_hat = quantize_dac(q,num_bits);

        if (_autotest_verbose)
            printf("%8.4f > 0x%2.2x > %8.4f\n", x, q, x_hat);

        CONTEND_DELTA(x,x_hat,tol);

        x += dx;
        x = (x > 1.0f) ? 1.0f : x;
    }
}

#endif // __LIQUID_QUANTIZE_AUTOTEST_H__

