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
                   float         _tol)                              \
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
        printf("(%7.3f,%7.3f) + (%7.3f,%7.3f) = (%7.3f,%7.3f), expected: (%7.3f,%7.3f)\n", \
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
/* test subtraction */                                              \
void CQ(_test_sub)(float complex _xf,                               \
                   float complex _yf,                               \
                   float         _tol)                              \
{                                                                   \
    /* convert to fixed-point */                                    \
    CQ(_t) x = CQ(_float_to_fixed)(_xf);                            \
    CQ(_t) y = CQ(_float_to_fixed)(_yf);                            \
                                                                    \
    /* execute operation */                                         \
    CQ(_t) z = CQ(_sub)(x, y);                                      \
    float complex zf = _xf - _yf;                                   \
                                                                    \
    /* convert to floating-point */                                 \
    float complex ztest = CQ(_fixed_to_float)(z);                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("(%7.3f,%7.3f) - (%7.3f,%7.3f) = (%7.3f,%7.3f), expected: (%7.3f,%7.3f)\n", \
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
                   float         _tol)                              \
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
        printf("(%7.3f,%7.3f) * (%7.3f,%7.3f) = (%7.3f,%7.3f), expected: (%7.3f,%7.3f)\n", \
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
/* test division */                                                 \
void CQ(_test_div)(float complex _xf,                               \
                   float complex _yf,                               \
                   float         _tol)                              \
{                                                                   \
    /* convert to fixed-point */                                    \
    CQ(_t) x = CQ(_float_to_fixed)(_xf);                            \
    CQ(_t) y = CQ(_float_to_fixed)(_yf);                            \
                                                                    \
    /* execute operation */                                         \
    CQ(_t) z = CQ(_div)(x, y);                                      \
    float complex zf = _xf / _yf;                                   \
                                                                    \
    /* convert to floating-point */                                 \
    float complex ztest = CQ(_fixed_to_float)(z);                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("(%7.3f,%7.3f) / (%7.3f,%7.3f) = (%7.3f,%7.3f), expected: (%7.3f,%7.3f)\n", \
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
    cq16_test_add( 0.81911 +  0.83629*_Complex_I, -0.28676 +  0.94995*_Complex_I, tol);
    cq16_test_add( 0.60038 +  0.21438*_Complex_I,  0.75947 + -0.58910*_Complex_I, tol);
    cq16_test_add( 0.69875 + -0.51095*_Complex_I,  0.99962 +  0.89917*_Complex_I, tol);
    cq16_test_add( 0.39740 + -0.03323*_Complex_I,  0.45830 +  0.86018*_Complex_I, tol);
    cq16_test_add(-0.43622 +  0.95747*_Complex_I, -0.28784 + -0.95700*_Complex_I, tol);
    cq16_test_add( 0.65335 + -0.98661*_Complex_I, -0.56604 +  0.65523*_Complex_I, tol);
    cq16_test_add(-0.43161 +  0.10766*_Complex_I, -0.02236 + -0.57604*_Complex_I, tol);
    cq16_test_add( 0.31323 + -0.03695*_Complex_I, -0.81640 +  0.17809*_Complex_I, tol);
    cq16_test_add(-0.19660 + -0.06582*_Complex_I,  0.02889 +  0.71771*_Complex_I, tol);
    cq16_test_add(-0.95410 + -0.57502*_Complex_I,  0.86389 +  0.91886*_Complex_I, tol);
}

void autotest_cq16_sub()
{
    float tol = expf(-sqrtf(q16_fracbits));

    // run specific tests
    cq16_test_sub(-0.03088 +  0.18292*_Complex_I,  0.52888 + -0.11447*_Complex_I, tol);
    cq16_test_sub( 0.94725 +  0.96198*_Complex_I, -0.46320 +  0.50263*_Complex_I, tol);
    cq16_test_sub( 0.95797 + -0.04013*_Complex_I,  0.29476 + -0.87833*_Complex_I, tol);
    cq16_test_sub( 0.26583 + -0.08018*_Complex_I, -0.71007 +  0.23766*_Complex_I, tol);
    cq16_test_sub( 0.67738 +  0.65268*_Complex_I,  0.12252 + -0.93337*_Complex_I, tol);
    cq16_test_sub( 0.39551 +  0.46059*_Complex_I,  0.72501 + -0.55978*_Complex_I, tol);
    cq16_test_sub( 0.97282 + -0.52345*_Complex_I, -0.37437 + -0.79523*_Complex_I, tol);
    cq16_test_sub(-0.84438 + -0.43361*_Complex_I,  0.73363 + -0.42197*_Complex_I, tol);
    cq16_test_sub(-0.58692 +  0.64871*_Complex_I, -0.37983 + -0.63185*_Complex_I, tol);
    cq16_test_sub(-0.64057 +  0.20079*_Complex_I, -0.06919 + -0.05721*_Complex_I, tol);
}

void autotest_cq16_mul()
{
    // determine appropriate tolerance
    unsigned int shift = q16_intbits < 6 ? 6 : q16_intbits;
    float tol = q16_fixed_to_float(1<<shift);
    if (tol > 0.1) tol = 0.1f;

    // run specific tests
    cq16_test_mul( 0.39407 +  0.47544*_Complex_I, -0.54994 + -0.77925*_Complex_I, tol);
    cq16_test_mul( 0.36185 +  0.93651*_Complex_I, -0.93322 + -0.21109*_Complex_I, tol);
    cq16_test_mul( 0.16584 + -0.42133*_Complex_I, -0.10204 +  0.40487*_Complex_I, tol);
    cq16_test_mul(-0.06406 +  0.19082*_Complex_I, -0.17120 +  0.19229*_Complex_I, tol);
    cq16_test_mul( 0.20096 + -0.38047*_Complex_I, -0.30371 + -0.75139*_Complex_I, tol);
    cq16_test_mul(-0.31106 + -0.31922*_Complex_I,  0.21878 +  0.46536*_Complex_I, tol);
    cq16_test_mul( 0.71149 +  0.12000*_Complex_I, -0.70060 + -0.76876*_Complex_I, tol);
    cq16_test_mul( 0.80862 + -0.74223*_Complex_I,  0.69114 + -0.43045*_Complex_I, tol);
    cq16_test_mul(-0.20470 + -0.78997*_Complex_I, -0.50803 + -0.25716*_Complex_I, tol);
    cq16_test_mul( 0.39123 + -0.48306*_Complex_I, -0.36542 +  0.84223*_Complex_I, tol);
}

void autotest_cq16_div()
{
    // determine appropriate tolerance
    unsigned int shift = q16_intbits < 6 ? 6 : q16_intbits;
    float tol = q16_fixed_to_float(1<<shift);
    if (tol > 0.1) tol = 0.1f;

    // run specific tests
    cq16_test_div( 0.10851 + -0.13978*_Complex_I,  0.16811 +  0.68214*_Complex_I, tol);
    cq16_test_div(-0.70594 +  0.53183*_Complex_I, -0.92573 +  0.71159*_Complex_I, tol);
    cq16_test_div( 0.11517 +  0.07453*_Complex_I,  0.28529 +  0.25191*_Complex_I, tol);
    cq16_test_div( 0.45126 +  0.11577*_Complex_I, -0.05151 +  0.98181*_Complex_I, tol);
    cq16_test_div(-0.77249 +  0.03425*_Complex_I, -0.17600 + -0.94509*_Complex_I, tol);
    cq16_test_div(-0.20988 +  0.40998*_Complex_I, -0.53038 +  0.47055*_Complex_I, tol);
    cq16_test_div( 0.36404 +  0.04727*_Complex_I,  0.04576 +  0.57257*_Complex_I, tol);
    cq16_test_div(-0.02758 +  0.33975*_Complex_I, -0.40432 + -0.11819*_Complex_I, tol);
    cq16_test_div( 0.15848 +  0.64769*_Complex_I,  0.44643 +  0.89393*_Complex_I, tol);
    cq16_test_div( 0.12968 +  0.71151*_Complex_I, -0.82973 + -0.16768*_Complex_I, tol);
}


// 
// q32
//
void autotest_cq32_add()
{
    float tol = expf(-sqrtf(q32_fracbits));

    // run specific tests
    cq32_test_add( 0.81911 +  0.83629*_Complex_I, -0.28676 +  0.94995*_Complex_I, tol);
    cq32_test_add( 0.60038 +  0.21438*_Complex_I,  0.75947 + -0.58910*_Complex_I, tol);
    cq32_test_add( 0.69875 + -0.51095*_Complex_I,  0.99962 +  0.89917*_Complex_I, tol);
    cq32_test_add( 0.39740 + -0.03323*_Complex_I,  0.45830 +  0.86018*_Complex_I, tol);
    cq32_test_add(-0.43622 +  0.95747*_Complex_I, -0.28784 + -0.95700*_Complex_I, tol);
    cq32_test_add( 0.65335 + -0.98661*_Complex_I, -0.56604 +  0.65523*_Complex_I, tol);
    cq32_test_add(-0.43161 +  0.10766*_Complex_I, -0.02236 + -0.57604*_Complex_I, tol);
    cq32_test_add( 0.31323 + -0.03695*_Complex_I, -0.81640 +  0.17809*_Complex_I, tol);
    cq32_test_add(-0.19660 + -0.06582*_Complex_I,  0.02889 +  0.71771*_Complex_I, tol);
    cq32_test_add(-0.95410 + -0.57502*_Complex_I,  0.86389 +  0.91886*_Complex_I, tol);
}

void autotest_cq32_sub()
{
    float tol = expf(-sqrtf(q32_fracbits));

    // run specific tests
    cq32_test_sub(-0.03088 +  0.18292*_Complex_I,  0.52888 + -0.11447*_Complex_I, tol);
    cq32_test_sub( 0.94725 +  0.96198*_Complex_I, -0.46320 +  0.50263*_Complex_I, tol);
    cq32_test_sub( 0.95797 + -0.04013*_Complex_I,  0.29476 + -0.87833*_Complex_I, tol);
    cq32_test_sub( 0.26583 + -0.08018*_Complex_I, -0.71007 +  0.23766*_Complex_I, tol);
    cq32_test_sub( 0.67738 +  0.65268*_Complex_I,  0.12252 + -0.93337*_Complex_I, tol);
    cq32_test_sub( 0.39551 +  0.46059*_Complex_I,  0.72501 + -0.55978*_Complex_I, tol);
    cq32_test_sub( 0.97282 + -0.52345*_Complex_I, -0.37437 + -0.79523*_Complex_I, tol);
    cq32_test_sub(-0.84438 + -0.43361*_Complex_I,  0.73363 + -0.42197*_Complex_I, tol);
    cq32_test_sub(-0.58692 +  0.64871*_Complex_I, -0.37983 + -0.63185*_Complex_I, tol);
    cq32_test_sub(-0.64057 +  0.20079*_Complex_I, -0.06919 + -0.05721*_Complex_I, tol);
}

void autotest_cq32_mul()
{
    // determine appropriate tolerance
    unsigned int shift = q32_intbits < 6 ? 6 : q32_intbits;
    float tol = q32_fixed_to_float(1<<shift);
    if (tol > 0.1) tol = 0.1f;

    // run specific tests
    cq32_test_mul( 0.39407 +  0.47544*_Complex_I, -0.54994 + -0.77925*_Complex_I, tol);
    cq32_test_mul( 0.36185 +  0.93651*_Complex_I, -0.93322 + -0.21109*_Complex_I, tol);
    cq32_test_mul( 0.16584 + -0.42133*_Complex_I, -0.10204 +  0.40487*_Complex_I, tol);
    cq32_test_mul(-0.06406 +  0.19082*_Complex_I, -0.17120 +  0.19229*_Complex_I, tol);
    cq32_test_mul( 0.20096 + -0.38047*_Complex_I, -0.30371 + -0.75139*_Complex_I, tol);
    cq32_test_mul(-0.31106 + -0.31922*_Complex_I,  0.21878 +  0.46536*_Complex_I, tol);
    cq32_test_mul( 0.71149 +  0.12000*_Complex_I, -0.70060 + -0.76876*_Complex_I, tol);
    cq32_test_mul( 0.80862 + -0.74223*_Complex_I,  0.69114 + -0.43045*_Complex_I, tol);
    cq32_test_mul(-0.20470 + -0.78997*_Complex_I, -0.50803 + -0.25716*_Complex_I, tol);
    cq32_test_mul( 0.39123 + -0.48306*_Complex_I, -0.36542 +  0.84223*_Complex_I, tol);
}

void autotest_cq32_div()
{
    // determine appropriate tolerance
    unsigned int shift = q32_intbits < 6 ? 6 : q32_intbits;
    float tol = q32_fixed_to_float(1<<shift);
    if (tol > 0.1) tol = 0.1f;

    // run specific tests
    cq32_test_div( 0.10851 + -0.13978*_Complex_I,  0.16811 +  0.68214*_Complex_I, tol);
    cq32_test_div(-0.70594 +  0.53183*_Complex_I, -0.92573 +  0.71159*_Complex_I, tol);
    cq32_test_div( 0.11517 +  0.07453*_Complex_I,  0.28529 +  0.25191*_Complex_I, tol);
    cq32_test_div( 0.45126 +  0.11577*_Complex_I, -0.05151 +  0.98181*_Complex_I, tol);
    cq32_test_div(-0.77249 +  0.03425*_Complex_I, -0.17600 + -0.94509*_Complex_I, tol);
    cq32_test_div(-0.20988 +  0.40998*_Complex_I, -0.53038 +  0.47055*_Complex_I, tol);
    cq32_test_div( 0.36404 +  0.04727*_Complex_I,  0.04576 +  0.57257*_Complex_I, tol);
    cq32_test_div(-0.02758 +  0.33975*_Complex_I, -0.40432 + -0.11819*_Complex_I, tol);
    cq32_test_div( 0.15848 +  0.64769*_Complex_I,  0.44643 +  0.89393*_Complex_I, tol);
    cq32_test_div( 0.12968 +  0.71151*_Complex_I, -0.82973 + -0.16768*_Complex_I, tol);
}


