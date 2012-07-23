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
#define LIQUIDFPM_AUTOTEST_MUL_API(Q)                               \
                                                                    \
/* test portable multiplication */                                  \
void Q(_test_mul)(float _xf,                                        \
                  float _yf,                                        \
                  float _tol)                                       \
{                                                                   \
    /* convert to fixed-point and back to float */                  \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    Q(_t) y = Q(_float_to_fixed)(_yf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
    _yf = Q(_fixed_to_float)(y);                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_t) z = Q(_mul)(x,y);                                         \
    float zf = _xf * _yf;                                           \
                                                                    \
    /* convert to floating-point */                                 \
    float ztest = Q(_fixed_to_float)(z);                            \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(zf,ztest,_tol);                                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("%12.8f * %12.8f = %12.8f (%12.8f), e=%12.8f\n",     \
                _xf,     _yf,     ztest,  zf,        ztest-zf);     \
    }                                                               \
}                                                                   \
                                                                    \
/* test inline multiplication */                                    \
void Q(_test_mul_inline)(float _xf,                                 \
                         float _yf,                                 \
                         float _tol)                                \
{                                                                   \
    /* convert to fixed-point and back to float */                  \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    Q(_t) y = Q(_float_to_fixed)(_yf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
    _yf = Q(_fixed_to_float)(y);                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_t) z = Q(_mul_inline)(x,y);                                  \
    float zf = _xf * _yf;                                           \
                                                                    \
    /* convert to floating-point */                                 \
    float ztest = Q(_fixed_to_float)(z);                            \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(zf,ztest,_tol);                                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("%12.8f * %12.8f = %12.8f (%12.8f), e=%12.8f\n",     \
                _xf,     _yf,     ztest,  zf,        ztest-zf);     \
    }                                                               \
}

// define autotest API
LIQUIDFPM_AUTOTEST_MUL_API(LIQUIDFPM_MANGLE_Q32)
LIQUIDFPM_AUTOTEST_MUL_API(LIQUIDFPM_MANGLE_Q16)

// 
// q16
//

// portable mulitplication
void autotest_q16_mul()
{
    float tol = q16_fixed_to_float(1<<q16_intbits);

    // basic tests
    q16_test_mul( 0.25f, 2.25f, tol);
    q16_test_mul( 0.25f,-2.25f, tol);

    float a = q16_fixed_to_float(q16_max)*0.7f;
    float b = q16_fixed_to_float(q16_one)/3.0f;
    q16_test_mul(a, b, tol);

    // extremes
    q16_t x = q16_max;          // max
    q16_t y = q16_one>>1;       // 1/2
    q16_t z = q16_mul(x,y);     // 
    q16_t z_test = q16_max>>1;

    q16_t qtol = 1<<q16_intbits;      // fixed-point tolerance
    CONTEND_DELTA(z, z_test, qtol);
}

// inline multiplication
void autotest_q16_mul_inline()
{
    float tol = q16_fixed_to_float(1<<q16_intbits);

    // basic tests
    q16_test_mul_inline( 0.25f, 2.25f, tol);
    q16_test_mul_inline( 0.25f,-2.25f, tol);

    float a = q16_fixed_to_float(q16_max)*0.7f;
    float b = q16_fixed_to_float(q16_one)/3.0f;
    q16_test_mul_inline(a, b, tol);

    // extremes
    q16_t x = q16_max;          // max
    q16_t y = q16_one>>1;       // 1/2
    q16_t z = q16_mul_inline(x,y);
    q16_t z_test = q16_max>>1;

    q16_t qtol = 1<<q16_intbits;      // fixed-point tolerance
    CONTEND_DELTA(z, z_test, qtol);
}


// 
// q32
//

// portable mulitplication
void autotest_q32_mul()
{
    float tol = q32_fixed_to_float(1<<q32_intbits);

    // basic tests
    q32_test_mul( 0.25f, 2.25f, tol);
    q32_test_mul( 0.25f,-2.25f, tol);

    float a = q32_fixed_to_float(q32_max)*0.7f;
    float b = q32_fixed_to_float(q32_one)/3.0f;
    q32_test_mul(a, b, tol);

    // extremes
    q32_t x = q32_max;          // max
    q32_t y = q32_one>>1;       // 1/2
    q32_t z = q32_mul(x,y);     // 
    q32_t z_test = q32_max>>1;

    q32_t qtol = 1<<q32_intbits;      // fixed-point tolerance
    CONTEND_DELTA(z, z_test, qtol);
}


// inline multiplication
void autotest_q32_mul_inline()
{
    float tol = q32_fixed_to_float(1<<q32_intbits);

    // basic tests
    q32_test_mul_inline( 0.25f, 2.25f, tol);
    q32_test_mul_inline( 0.25f,-2.25f, tol);

    float a = q32_fixed_to_float(q32_max)*0.7f;
    float b = q32_fixed_to_float(q32_one)/3.0f;
    q32_test_mul_inline(a, b, tol);

    // extremes
    q32_t x = q32_max;          // max
    q32_t y = q32_one>>1;       // 1/2
    q32_t z = q32_mul_inline(x,y);
    q32_t z_test = q32_max>>1;

    q32_t qtol = 1<<q32_intbits;      // fixed-point tolerance
    CONTEND_DELTA(z, z_test, qtol);
}

