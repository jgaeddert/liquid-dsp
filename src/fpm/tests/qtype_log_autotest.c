/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <math.h>

#include "autotest/autotest.h"
#include "liquidfpm.internal.h"

// API definition macro; helper function to keep code base small
#define LIQUIDFPM_AUTOTEST_LOG_API(Q)                               \
                                                                    \
/* test log2 using shift|add method */                              \
void Q(_test_log2_shiftadd)(float        _xf,                       \
                            unsigned int _precision,                \
                            float        _tol)                      \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_log2_shiftadd)(x,_precision);                      \
    float yf = log2f(_xf);                                          \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("log2(%12.8f) = %12.8f (%12.8f), e = %12.8f\n",      \
                     _xf,      ytest,  yf,          ytest-yf);      \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(yf,ytest,_tol);                                   \
}                                                                   \
                                                                    \
/* test log2 using look-up table */                                 \
void Q(_test_log2_frac)(float _xf,                                  \
                        float _tol)                                 \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_log2_frac)(x);                                     \
    float yf = log2f(_xf);                                          \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("log2(%12.8f) = %12.8f (%12.8f), e = %12.8f\n",      \
                     _xf,      ytest,  yf,          ytest-yf);      \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(yf,ytest,_tol);                                   \
}                                                                   \

// define autotest APIs
LIQUIDFPM_AUTOTEST_LOG_API(LIQUIDFPM_MANGLE_Q16)
LIQUIDFPM_AUTOTEST_LOG_API(LIQUIDFPM_MANGLE_Q32)

// 
// log2 shift|add tests
//

void autotest_q16_log2_shiftadd()
{
    unsigned int precision = 16;
    unsigned int num_steps = 77;
    float xmin = q16_fixed_to_float( 1 << 8  ); // min input
    float xmax = q16_fixed_to_float( q16_max ); // max input
    float ymin = q16_fixed_to_float(-q16_max ); // min output
    //float ymax = q16_fixed_to_float( q16_max ); // max output
    
    // adjust input accordingly
    if (xmin < exp2f(ymin)) xmin = exp2f(ymin);
    //if (xmax > exp2f(ymax)) xmax = exp2f(ymax);

    //float dx = (xmax - xmin)/((float)(num_steps-1));
    float sigma = powf(xmin/xmax,-(1.0f - 1e-6f)/(float)(num_steps-1));
    float tol = expf(-sqrtf(q16_fracbits));

    // testing variables
    float xf;

    unsigned int i;
    xf = xmin;
    for (i=0; i<num_steps; i++) {
        if (xf > q16_fixed_to_float(q16_max))
            continue;

        // run test
        q16_test_log2_shiftadd(xf,precision,tol);

        // increment input parameter
        //xf += dx;
        xf *= sigma;
    }
}

void autotest_q32_log2_shiftadd()
{
    unsigned int precision = 32;
    unsigned int num_steps = 77;
    float xmin = q32_fixed_to_float( 1 << 8  ); // min input
    float xmax = q32_fixed_to_float( q32_max ); // max input
    float ymin = q32_fixed_to_float(-q32_max ); // min output
    //float ymax = q32_fixed_to_float( q32_max ); // max output
    
    // adjust input accordingly
    if (xmin < exp2f(ymin)) xmin = exp2f(ymin);
    //if (xmax > exp2f(ymax)) xmax = exp2f(ymax);

    //float dx = (xmax - xmin)/((float)(num_steps-1));
    float sigma = powf(xmin/xmax,-(1.0f - 1e-6f)/(float)(num_steps-1));
    float tol = expf(-sqrtf(q32_fracbits));

    // testing variables
    float xf;

    unsigned int i;
    xf = xmin;
    for (i=0; i<num_steps; i++) {
        if (xf > q32_fixed_to_float(q32_max))
            continue;

        // run test
        q32_test_log2_shiftadd(xf,precision,tol);

        // increment input parameter
        //xf += dx;
        xf *= sigma;
    }
}

// 
// log2 frac tests
//
void autotest_q16_log2_frac()
{
    unsigned int num_steps = 37;
    float xmin = q16_fixed_to_float( 1 << 8  ); // min input
    float xmax = q16_fixed_to_float( q16_max ); // max input
    float ymin = q16_fixed_to_float(-q16_max ); // min output
    //float ymax = q16_fixed_to_float( q16_max ); // max output
    
    // adjust input accordingly
    if (xmin < exp2f(ymin)) xmin = exp2f(ymin);
    //if (xmax > exp2f(ymax)) xmax = exp2f(ymax);

    //float dx = (xmax - xmin)/((float)(num_steps-1));
    float sigma = powf(xmin/xmax,-(1.0f - 1e-6f)/(float)(num_steps-1));
    //float tol = expf(-sqrtf(q16_fracbits));
    float tol = 0.01f;

    // testing variables
    float xf;

    unsigned int i;
    xf = xmin;
    for (i=0; i<num_steps; i++) {
        if (xf > q16_fixed_to_float(q16_max))
            continue;

        // run test
        q16_test_log2_frac(xf,tol);

        // increment input parameter
        //xf += dx;
        xf *= sigma;
    }
}

void autotest_q32_log2_frac()
{
    unsigned int num_steps = 37;
    float xmin = q32_fixed_to_float( 1 << 8  ); // min input
    float xmax = q32_fixed_to_float( q32_max ); // max input
    float ymin = q32_fixed_to_float(-q32_max ); // min output
    //float ymax = q32_fixed_to_float( q32_max ); // max output
    
    // adjust input accordingly
    if (xmin < exp2f(ymin)) xmin = exp2f(ymin);
    //if (xmax > exp2f(ymax)) xmax = exp2f(ymax);

    //float dx = (xmax - xmin)/((float)(num_steps-1));
    float sigma = powf(xmin/xmax,-(1.0f - 1e-6f)/(float)(num_steps-1));
    //float tol = expf(-sqrtf(q32_fracbits));
    float tol = 0.01f;

    // testing variables
    float xf;

    unsigned int i;
    xf = xmin;
    for (i=0; i<num_steps; i++) {
        if (xf > q32_fixed_to_float(q32_max))
            continue;

        // run test
        q32_test_log2_frac(xf,tol);

        // increment input parameter
        //xf += dx;
        xf *= sigma;
    }
}

