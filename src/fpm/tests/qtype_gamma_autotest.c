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
#define LIQUIDFPM_AUTOTEST_GAMMA_API(Q)                             \
                                                                    \
/* test gamma functions */                                          \
void Q(_test_lgamma)(float _xf,                                     \
                     float _tol)                                    \
{                                                                   \
    /* convert to fixed-point, back to float */                     \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_lgamma)(x);                                        \
    float yf = lgammaf(_xf);                                        \
                                                                    \
    /* convert to floating-point */                                 \
    float y_test = Q(_fixed_to_float)(y);                           \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("  lgamma(%12.8f) = %12.8f(%12.8f), e=%12.8f\n",     \
                         _xf,      y_test,yf,     y_test-yf);       \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(y_test, yf, _tol);                                \
}                                                                   \

// define autotest API
LIQUIDFPM_AUTOTEST_GAMMA_API(LIQUIDFPM_MANGLE_Q32)
LIQUIDFPM_AUTOTEST_GAMMA_API(LIQUIDFPM_MANGLE_Q16)

// 
// q16
//

void autotest_q16_lgamma()
{
    // options
    unsigned int num_steps = 57;
    float tol = 0.3f;
    
    // determine qtype bounds
    float qmin = q16_fixed_to_float(q16_min);
    float qmax = q16_fixed_to_float(q16_max);

    // reduce lower bound until output is within qtype range
    float xmin = qmin;
    while ( lgammaf(xmin) > qmax )
        xmin *= 0.99f;

    // reduce upper bound until output is within qtype range
    float xmax = qmax;
    while ( lgammaf(xmax) > qmax*0.99f )
        xmax *= 0.99f;

    if (liquid_autotest_verbose) {
        printf("  q16 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, lgammaf(xmin), lgammaf(xmax));
    }

    unsigned int i;
    float sigma = powf(xmin/xmax,-(1.0f - 1e-6f)/(float)(num_steps-1));
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q16_test_lgamma(x, tol);
        
        // update input
        x *= sigma;
    }
}


// 
// q32
//

void autotest_q32_lgamma()
{
    // options
    unsigned int num_steps = 57;
    float tol = 0.01f;
    
    // determine qtype bounds
    float qmin = q32_fixed_to_float(q32_min);
    float qmax = q32_fixed_to_float(q32_max);

    // reduce lower bound until output is within qtype range
    float xmin = qmin;
    while ( lgammaf(xmin) > qmax )
        xmin *= 0.99f;

    // reduce upper bound until output is within qtype range
    float xmax = qmax;
    while ( lgammaf(xmax) > qmax*0.99f )
        xmax *= 0.99f;

    if (liquid_autotest_verbose) {
        printf("  q32 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, lgammaf(xmin), lgammaf(xmax));
    }

    unsigned int i;
    float sigma = powf(xmin/xmax,-(1.0f - 1e-6f)/(float)(num_steps-1));
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q32_test_lgamma(x, tol);
        
        // update input
        x *= sigma;
    }
}

