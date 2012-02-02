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

#include "liquid.internal.h"
#include "liquidfpm.internal.h"
#include "autotest/autotest.h"

#include <complex.h>

// API definition macro; helper function to keep code base small
#define LIQUIDFPM_AUTOTEST_COMPLEX_ARITHMETIC_API(Q,CQ)             \
                                                                    \
/* test addition */                                                 \
void CQ(_test_add)(float complex _xf,                               \
                   float complex _yf,                               \
                   float        _tol)                               \
{                                                                   \
    /* convert to fixed-point */                                    \
    CQ(_t) x = CQ(_float_to_fixed)(_xf);                            \
    CQ(_t) y = CQ(_float_to_fixed)(_yf);                            \
                                                                    \
    /* execute operation */                                         \
    CQ(_t) z = CQ(_add)(x, y);                                      \
    float complex zf = _xf + _yf;                                   \
                                                                    \
    /* convert to floating-point */                                 \
    float complex ztest = CQ(_fixed_to_float)(z);                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("(%8.4f,%8.4f) + (%8.4f,%8.4f) = (%8.4f,%8.4f), expected: (%8.4f,%8.4f)\n", \
                crealf(_xf), cimagf(_xf),                           \
                crealf(_yf), cimagf(_yf),                           \
                crealf(ztest), cimagf(ztest),                       \
                crealf(zf), cimagf(zf));                            \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(crealf(zf),crealf(ztest),_tol);                   \
    CONTEND_DELTA(cimagf(zf),cimagf(ztest),_tol);                   \
}                                                                   \
                                                                    \
/* test multiplication */                                           \
void CQ(_test_mul)(float complex _xf,                               \
                   float complex _yf,                               \
                   float        _tol)                               \
{                                                                   \
    /* convert to fixed-point */                                    \
    CQ(_t) x = CQ(_float_to_fixed)(_xf);                            \
    CQ(_t) y = CQ(_float_to_fixed)(_yf);                            \
                                                                    \
    /* execute operation */                                         \
    CQ(_t) z = CQ(_mul)(x, y);                                      \
    float complex zf = _xf * _yf;                                   \
                                                                    \
    /* convert to floating-point */                                 \
    float complex ztest = CQ(_fixed_to_float)(z);                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("(%8.4f,%8.4f) * (%8.4f,%8.4f) = (%8.4f,%8.4f), expected: (%8.4f,%8.4f)\n", \
                crealf(_xf), cimagf(_xf),                           \
                crealf(_yf), cimagf(_yf),                           \
                crealf(ztest), cimagf(ztest),                       \
                crealf(zf), cimagf(zf));                            \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(crealf(zf),crealf(ztest),_tol);                   \
    CONTEND_DELTA(cimagf(zf),cimagf(ztest),_tol);                   \
}


// define autotest API
LIQUIDFPM_AUTOTEST_COMPLEX_ARITHMETIC_API(LIQUIDFPM_MANGLE_Q16, LIQUIDFPM_MANGLE_CQ16)
LIQUIDFPM_AUTOTEST_COMPLEX_ARITHMETIC_API(LIQUIDFPM_MANGLE_Q32, LIQUIDFPM_MANGLE_CQ32)


// 
// q16
//
void autotest_cq16_add()
{
    float tol = expf(-sqrtf(q16_fracbits));

    // run specific tests
    cq16_test_add(1.0f + _Complex_I*0.5f, 0.5f - _Complex_I*0.9f, tol);
}

void autotest_cq16_mul()
{
    // determine appropriate tolerance
    unsigned int shift = q16_intbits < 6 ? 6 : q16_intbits;
    float tol = q16_fixed_to_float(1<<shift);
    if (tol > 0.1) tol = 0.1f;

    // floating-point tests
    cq16_test_mul(1.0f + _Complex_I*0.5f, 0.3f - _Complex_I*0.9f, tol);
}


// 
// q32
//
void autotest_cq32_add()
{
    float tol = expf(-sqrtf(q32_fracbits));

    // run specific tests
    cq32_test_add(1.0f + _Complex_I*0.5f, 0.5f - _Complex_I*0.9f, tol);
}

void autotest_cq32_mul()
{
    // determine appropriate tolerance
    unsigned int shift = q32_intbits < 6 ? 6 : q32_intbits;
    float tol = q32_fixed_to_float(1<<shift);
    if (tol > 0.1) tol = 0.1f;

    // floating-point tests
    cq32_test_mul(1.0f + _Complex_I*0.5f, 0.3f - _Complex_I*0.9f, tol);
}


