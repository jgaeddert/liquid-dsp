/*
 * Copyright (c) 2008, 2009 Joseph Gaeddert
 * Copyright (c) 2008, 2009 Virginia Polytechnic Institute & State University
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
#define LIQUIDFPM_AUTOTEST_SINCOS_TAB_API(Q)                        \
void Q(_test_sincos_tab)(float _thetaf,                             \
                         float _tol)                                \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) theta = Q(_angle_float_to_fixed)(_thetaf);                \
    Q(_t) qsin;                                                     \
    Q(_t) qcos;                                                     \
                                                                    \
    /* execute operation */                                         \
    Q(_sincos_tab)(theta, &qsin, &qcos);                            \
    float fsin = sinf(_thetaf);                                     \
    float fcos = cosf(_thetaf);                                     \
                                                                    \
    /* convert to floating-point */                                 \
    float qsin_test = Q(_fixed_to_float)(qsin);                     \
    float qcos_test = Q(_fixed_to_float)(qcos);                     \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("sin,cos(%11.8f) = %11.8f(%11.8f), %11.8f(%11.8f)\n",\
                _thetaf,                                            \
                qsin_test, fsin,                                    \
                qcos_test, fcos);                                   \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(qsin_test, fsin, _tol);                           \
    CONTEND_DELTA(qcos_test, fcos, _tol);                           \
}

// define autotest API
LIQUIDFPM_AUTOTEST_SINCOS_TAB_API(LIQUIDFPM_MANGLE_Q32)
LIQUIDFPM_AUTOTEST_SINCOS_TAB_API(LIQUIDFPM_MANGLE_Q16)

void autotest_q16_sincos_tab()
{
    unsigned int num_steps = 157;
    float xmin = -2*M_PI;
    float xmax =  2*M_PI;
    float dx = (xmax - xmin)/((float)(num_steps-1));
    float tol = expf(-sqrtf(q16_fracbits));

    // run tests
    float x = xmin;
    unsigned int i;
    for (i=0; i<num_steps; i++) {
        // run test
        q16_test_sincos_tab(x, tol);

        // increment input
        x += dx;
    }
}

void autotest_q32_sincos_tab()
{
    unsigned int num_steps = 157;
    float xmin = -2*M_PI;
    float xmax =  2*M_PI;
    float dx = (xmax - xmin)/((float)(num_steps-1));
    float tol = expf(-sqrtf(q32_fracbits));

    // run tests
    float x = xmin;
    unsigned int i;
    for (i=0; i<num_steps; i++) {
        // run test
        q32_test_sincos_tab(x, tol);

        // increment input
        x += dx;
    }
}

