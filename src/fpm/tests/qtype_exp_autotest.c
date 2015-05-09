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
#define LIQUIDFPM_AUTOTEST_EXP_API(Q)                               \
                                                                    \
/* test exp2 using shift|add method */                              \
void Q(_test_exp2_shiftadd)(float        _xf,                       \
                            unsigned int _precision,                \
                            float        _tol)                      \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_exp2_shiftadd)(x,_precision);                      \
    float yf = exp2f(_xf);                                          \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("exp2(%12.8f) = %12.8f (%12.8f), e = %12.8f\n",      \
                     _xf,      ytest,  yf,          ytest-yf);      \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(yf,ytest,_tol);                                   \
}                                                                   \
                                                                    \
/* test exp2 using look-up table */                                 \
void Q(_test_exp2_frac)(float _xf,                                  \
                        float _tol)                                 \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_exp2_frac)(x);                                     \
    float yf = exp2f(_xf);                                          \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("exp2(%12.8f) = %12.8f (%12.8f), e = %12.8f\n",      \
                     _xf,      ytest,  yf,          ytest-yf);      \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(yf,ytest,_tol);                                   \
}                                                                   \

// define autotest APIs
LIQUIDFPM_AUTOTEST_EXP_API(LIQUIDFPM_MANGLE_Q16)
LIQUIDFPM_AUTOTEST_EXP_API(LIQUIDFPM_MANGLE_Q32)

// 
// exp2 shift|add tests
//

void autotest_q16_exp2_shiftadd()
{
    unsigned int precision = 16;
    unsigned int num_steps = 77;

    // compute lower range
    float xmin0= -q16_fixed_to_float(q16_max);
    float xmin1= -0.95f*(float)(q16_fracbits);
    float xmin = xmin0 > xmin1 ? xmin0 : xmin1;

    // upper range
    float xmax0 = (float)(q16_intbits-1)*0.95f;
    float xmax1 = 0.95f*(float)(1<<q16_intbits);
    float xmax  = xmax0 < xmax1 ? xmax0 : xmax1;

    float dx   = (xmax - xmin)/((float)(num_steps-1));
    //float tol  = expf(-sqrtf(q16_fracbits));
    float tol = 0.7f;   // TODO: check proper tolerance
    
    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q16_test_exp2_shiftadd(x,precision,tol);

        // increment input parameter
        x += dx;
    }
}

void autotest_q32_exp2_shiftadd()
{
    unsigned int precision = 16;
    unsigned int num_steps = 77;
    
    // compute lower range
    float xmin0 = -q32_fixed_to_float(q32_max);
    float xmin1 = -0.95f*(float)(q32_fracbits);
    float xmin  = xmin0 > xmin1 ? xmin0 : xmin1;

    // upper range
    float xmax0 = (float)(q32_intbits-1)*0.95f;
    float xmax1 = 0.95f*(float)(1<<q32_intbits);
    float xmax  = xmax0 < xmax1 ? xmax0 : xmax1;

    float dx   = (xmax - xmin)/((float)(num_steps-1));
    float tol  = expf(-sqrtf(q32_fracbits));

    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q32_test_exp2_shiftadd(x,precision,tol);

        // increment input parameter
        x += dx;
    }
}

// 
// exp2 frac tests
//

void autotest_q16_exp2_frac()
{
    unsigned int num_steps = 77;

    // compute lower range
    float xmin0= -q16_fixed_to_float(q16_max);
    float xmin1= -0.95f*(float)(q16_fracbits);
    float xmin = xmin0 > xmin1 ? xmin0 : xmin1;

    // upper range
    float xmax0 = (float)(q16_intbits-1)*0.95f;
    float xmax1 = 0.95f*(float)(1<<q16_intbits);
    float xmax  = xmax0 < xmax1 ? xmax0 : xmax1;

    float dx   = (xmax - xmin)/((float)(num_steps-1));
    //float tol  = expf(-sqrtf(q16_fracbits));
    float tol = 0.7f;   // TODO: check proper tolerance
    
    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q16_test_exp2_frac(x,tol);

        // increment input parameter
        x += dx;
    }
}

void autotest_q32_exp2_frac()
{
    unsigned int num_steps = 77;
    
    // compute lower range
    float xmin0 = -q32_fixed_to_float(q32_max);
    float xmin1 = -0.95f*(float)(q32_fracbits);
    float xmin  = xmin0 > xmin1 ? xmin0 : xmin1;

    // upper range
    float xmax0 = (float)(q32_intbits-1)*0.95f;
    float xmax1 = 0.95f*(float)(1<<q32_intbits);
    float xmax  = xmax0 < xmax1 ? xmax0 : xmax1;

    float dx   = (xmax - xmin)/((float)(num_steps-1));
    //float tol  = expf(-sqrtf(q32_fracbits));
    float tol = 0.08f;   // TODO: check proper tolerance

    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q32_test_exp2_frac(x,tol);

        // increment input parameter
        x += dx;
    }
}

