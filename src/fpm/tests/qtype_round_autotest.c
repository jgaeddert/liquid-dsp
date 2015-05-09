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

// 
// test rounding methods: ceil, floor, round
//
#include <stdio.h>
#include <math.h>

#include "autotest/autotest.h"
#include "liquidfpm.internal.h"

// API definition macro; helper function to keep code base small
#define LIQUIDFPM_AUTOTEST_ROUND_API(Q)                             \
                                                                    \
void Q(_test_floor)(float _xf,                                      \
                    float _tol)                                     \
{                                                                   \
    /* convert to fixed-point and back to float */                  \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y  = Q(_floor)(x);                                        \
    float yf = floorf(_xf);                                         \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("  floor(%12.8f) = %12.8f (%12.8f), e=%12.8f\n",     \
                        _xf,      yf,     ytest,     ytest-yf);     \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(yf,ytest,_tol);                                   \
}                                                                   \
                                                                    \
void Q(_test_ceil)(float _xf,                                       \
                   float _tol)                                      \
{                                                                   \
    /* convert to fixed-point and back to float */                  \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y  = Q(_ceil)(x);                                         \
    float yf = ceilf(_xf);                                          \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("  ceil(%12.8f) = %12.8f (%12.8f), e=%12.8f\n",      \
                        _xf,      yf,     ytest,     ytest-yf);     \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(yf,ytest,_tol);                                   \
}                                                                   \

// define autotest API
LIQUIDFPM_AUTOTEST_ROUND_API(LIQUIDFPM_MANGLE_Q16)
LIQUIDFPM_AUTOTEST_ROUND_API(LIQUIDFPM_MANGLE_Q32)

// 
// q16
//

void autotest_q16_floor()
{
    unsigned int num_steps = 57;
    float tol  = 0.001f;

    // TODO : check bounds
    float qmax = q16_fixed_to_float(q16_max);
    //float qmin = q16_fixed_to_float(q16_min);
    float xmax = qmax*0.95f;
    float xmin = -xmax;
    float dx   = (xmax - xmin) / (float)(num_steps-1);

    unsigned int i;
    for (i=0; i<num_steps; i++) {
        // compute input
        float xf = xmin + i*dx;

        // run test
        q16_test_floor(xf, tol);
    }
}

void autotest_q16_ceil()
{
    unsigned int num_steps = 57;
    float tol  = 0.01f;

    // TODO : check bounds
    float qmax = q16_fixed_to_float(q16_max);
    //float qmin = q16_fixed_to_float(q16_min);
    float xmax = (qmax-1.0f)*0.95f;
    float xmin = -xmax;
    float dx   = (xmax - xmin) / (float)(num_steps-1);

    unsigned int i;
    for (i=0; i<num_steps; i++) {
        // compute input
        float xf = xmin + i*dx;

        // run test
        q16_test_ceil(xf, tol);
    }
}

// 
// q32
//

void autotest_q32_floor()
{
    unsigned int num_steps = 57;
    float tol  = 0.001f;

    // TODO : check bounds
    float qmax = q32_fixed_to_float(q32_max);
    //float qmin = q32_fixed_to_float(q32_min);
    float xmax = qmax*0.95f;
    float xmin = -xmax;
    float dx   = (xmax - xmin) / (float)(num_steps-1);

    unsigned int i;
    for (i=0; i<num_steps; i++) {
        // compute input
        float xf = xmin + i*dx;

        // run test
        q32_test_floor(xf, tol);
    }
}

void autotest_q32_ceil()
{
    unsigned int num_steps = 57;
    float tol  = 0.01f;

    // TODO : check bounds
    float qmax = q32_fixed_to_float(q32_max);
    //float qmin = q32_fixed_to_float(q32_min);
    float xmax = (qmax-1.0f)*0.95f;
    float xmin = -xmax;
    float dx   = (xmax - xmin) / (float)(num_steps-1);

    unsigned int i;
    for (i=0; i<num_steps; i++) {
        // compute input
        float xf = xmin + i*dx;

        // run test
        q32_test_ceil(xf, tol);
    }
}

