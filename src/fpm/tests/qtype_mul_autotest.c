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

