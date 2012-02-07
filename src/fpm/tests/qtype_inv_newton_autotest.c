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
    /* convert to fixed-point and back to float */                  \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
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
                      _xf,      ytest,  yf,      yf-ytest);         \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(ytest, yf, _tol);                                 \
}

// define autotest API
LIQUIDFPM_AUTOTEST_INV_NEWTON_API(LIQUIDFPM_MANGLE_Q32)
LIQUIDFPM_AUTOTEST_INV_NEWTON_API(LIQUIDFPM_MANGLE_Q16)


void autotest_q16_inv_newton()
{
    unsigned int precision = 16;    // precision
    unsigned int num_steps = 77;    // number of steps in test
    float tol = 0.09f;              // error tolerance

    // determine qtype bounds
    float qmin = q16_fixed_to_float(q16_min);
    float qmax = q16_fixed_to_float(q16_max);

    // reduce lower bound until output is within qtype range
    float xmin = qmin;
    while ( 1.0f/xmin > qmax )
        xmin *= 1.01f;

    // reduce upper bound until output is within qtype range
    float xmax = qmax;
    while ( 1.0f/xmax < qmin )
        xmax *= 0.99f;

    if (liquid_autotest_verbose) {
        printf("  q16 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, 1.0f/xmin, 1.0f/xmax);
    }

    float sigma = powf(xmin/xmax,-1.0f/(float)(num_steps-1));

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
    float tol = 0.001f;             // error tolerance
    
    // determine qtype bounds
    float qmin = q16_fixed_to_float(q16_min);
    float qmax = q16_fixed_to_float(q16_max);

    // reduce lower bound until output is within qtype range
    float xmin = qmin;
    while ( 1.0f/xmin > qmax )
        xmin *= 1.01f;

    // reduce upper bound until output is within qtype range
    float xmax = qmax;
    while ( 1.0f/xmax < qmin )
        xmax *= 0.99f;

    if (liquid_autotest_verbose) {
        printf("  q16 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, 1.0f/xmin, 1.0f/xmax);
    }

    float sigma = powf(xmin/xmax,-1.0f/(float)(num_steps-1));

    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q32_test_inv_newton(x, precision, tol);

        // increment input parameter
        x *= sigma;
    }
}

