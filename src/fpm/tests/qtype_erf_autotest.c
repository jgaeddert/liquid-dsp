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
#define LIQUIDFPM_AUTOTEST_ERF_API(Q)                               \
                                                                    \
/* test erf functions */                                            \
void Q(_test_erf)(float _xf,                                        \
                  float _tol)                                       \
{                                                                   \
    /* convert to fixed-point, back to float */                     \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_erf)(x);                                           \
    float yf = erff(_xf);                                           \
                                                                    \
    /* convert to floating-point */                                 \
    float y_test = Q(_fixed_to_float)(y);                           \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("  erf(%12.8f) = %12.8f(%12.8f), e=%12.8f\n",        \
                      _xf,      y_test,yf,     y_test-yf);          \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(y_test, yf, _tol);                                \
}                                                                   \

// define autotest API
LIQUIDFPM_AUTOTEST_ERF_API(LIQUIDFPM_MANGLE_Q32)
LIQUIDFPM_AUTOTEST_ERF_API(LIQUIDFPM_MANGLE_Q16)

// 
// q16
//

void autotest_q16_erf()
{
    // options
    unsigned int num_steps = 79;
    float tol = 0.03f;
    
    // determine qtype bounds
    float qmin = q16_fixed_to_float(q16_min);
    float qmax = q16_fixed_to_float(q16_max);

    float xmin = 0.0f;
    float xmax = qmax > 3.0f ? 3.0f : qmax;

    if (liquid_autotest_verbose) {
        printf("  q16 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, erff(xmin), erff(xmax));
    }

    unsigned int i;
    float dx = (xmax - xmin) / (float)(num_steps-1);
    for (i=0; i<num_steps; i++) {
        // update input
        float x = xmin + i*dx;

        // run test
        q16_test_erf(x, tol);
    }
}


// 
// q32
//

void autotest_q32_erf()
{
    // options
    unsigned int num_steps = 79;
    float tol = 0.008f;
    
    // determine qtype bounds
    float qmin = q32_fixed_to_float(q32_min);
    float qmax = q32_fixed_to_float(q32_max);

    float xmin = 0.0f;
    float xmax = qmax > 3.0f ? 3.0f : qmax;

    if (liquid_autotest_verbose) {
        printf("  q32 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, erff(xmin), erff(xmax));
    }

    unsigned int i;
    float dx = (xmax - xmin) / (float)(num_steps-1);
    for (i=0; i<num_steps; i++) {
        // update input
        float x = xmin + i*dx;

        // run test
        q32_test_erf(x, tol);
    }
}

