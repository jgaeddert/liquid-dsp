/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
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

