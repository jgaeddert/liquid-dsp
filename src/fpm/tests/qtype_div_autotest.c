/*
 * Copyright (c) 2008, 2009, 2012 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2012 Virginia Polytechnic
 *                                Institute & State University
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

#include <stdio.h>
#include <math.h>

#include "autotest/autotest.h"
#include "liquidfpm.internal.h"

// API definition macro; helper function to keep code base small
#define LIQUIDFPM_AUTOTEST_DIV_API(Q)                               \
void Q(_test_div)(float _xf,                                        \
                  float _yf,                                        \
                  float _tol)                                       \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    Q(_t) y = Q(_float_to_fixed)(_yf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) z = Q(_div)(x,y);                                         \
    float zf = _xf / _yf;                                           \
                                                                    \
    /* convert to floating-point */                                 \
    float ztest = Q(_fixed_to_float)(z);                            \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(zf,ztest,_tol);                                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("%12.8f / %12.8f = %12.8f (%12.8f)\n",               \
                _xf,     _yf,     ztest,  zf);                      \
    }                                                               \
}

// define autotest API
LIQUIDFPM_AUTOTEST_DIV_API(LIQUIDFPM_MANGLE_Q32)
LIQUIDFPM_AUTOTEST_DIV_API(LIQUIDFPM_MANGLE_Q16)

void autotest_q16_div()
{
    float tol = q16_fixed_to_float(1<<q16_intbits);

    // basic tests
    q16_test_div( 0.25f, 2.25f, tol);
    q16_test_div( 0.25f,-2.25f, tol);
}

void autotest_q32_div()
{
    float tol = q32_fixed_to_float(1<<q32_intbits);

    // basic tests
    q32_test_div( 0.25f, 2.25f, tol);
    q32_test_div( 0.25f,-2.25f, tol);
}

