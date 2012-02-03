/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
 *                                Institute & State University
 *
 * This file is part of liquid-fpm.
 *
 * liquid-fpm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid-fpm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid-fpm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <math.h>

#include "liquidfpm.internal.h"
#include "autotest/autotest.h"

// API definition macro; helper function to keep code base small
#define LIQUIDFPM_AUTOTEST_INV_NEWTON_API(Q)                        \
void Q(_test_inv_newton)(float        _xf,                          \
                         unsigned int _precision,                   \
                         float        _tol)                         \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_inv_newton)(x, _precision);                        \
    float yf = 1.0f / _xf;                                          \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("  inv(%12.8f) = %12.8f (%12.8f), e=%12.8f\n",       \
                      _xf,      ytest,  yf,      1.0-_xf*ytest);    \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(ytest*_xf, 1.0f, _tol);                           \
}

// define autotest API
LIQUIDFPM_AUTOTEST_INV_NEWTON_API(LIQUIDFPM_MANGLE_Q32)
LIQUIDFPM_AUTOTEST_INV_NEWTON_API(LIQUIDFPM_MANGLE_Q16)


void autotest_q16_inv_newton()
{
    unsigned int precision = 16;    // precision
    unsigned int num_steps = 77;    // number of steps in test
    float xmin  = 1.01f / q16_fixed_to_float(q16_max);
    float xmax  = q16_fixed_to_float(q16_max)*0.99f;
    float sigma = powf(xmin/xmax,-1.0f/(float)(num_steps-1));
    float tol   = 256.0f * exp2f(-q16_bits);

    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q16_test_inv_newton(x, precision, tol);

        // increment input parameter
        x *= sigma;
    }
}

void autotest_q32_inv_newton()
{
    unsigned int precision = 16;    // precision
    unsigned int num_steps = 77;    // number of steps in test
    float xmin  = 1.01f / q32_fixed_to_float(q32_max);
    float xmax  = q32_fixed_to_float(q32_max)*0.99f;
    float sigma = powf(xmin/xmax,-1.0f/(float)(num_steps-1));
    float tol   = 256.0f * exp2f(-q32_bits);

    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q32_test_inv_newton(x, precision, tol);

        // increment input parameter
        x *= sigma;
    }
}

