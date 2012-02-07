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
#define LIQUIDFPM_AUTOTEST_COMPLEX_TRIG_API(Q,CQ)                   \
                                                                    \
/* test complex exponential */                                      \
void CQ(_test_cexp)(float complex _xf,                              \
                    float         _tol)                             \
{                                                                   \
    /* convert to fixed-point and back to float */                  \
    CQ(_t) x = CQ(_float_to_fixed)(_xf);                            \
    _xf = CQ(_fixed_to_float)(x);                                   \
                                                                    \
    /* execute operation */                                         \
    CQ(_t) y = CQ(_cexp)(x);                                        \
    float complex yf = cexpf(_xf);                                  \
                                                                    \
    /* convert to floating-point */                                 \
    float complex ytest = CQ(_fixed_to_float)(y);                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("cexp(%7.3f,%7.3f) = (%7.3f,%7.3f), expected: (%7.3f,%7.3f), e=(%7.3f,%7.3f)\n",\
                crealf(_xf),   cimagf(_xf),                         \
                crealf(ytest), cimagf(ytest),                       \
                crealf(yf),    cimagf(yf),                          \
                crealf(ytest-yf), cimagf(ytest-yf));                \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(crealf(yf), crealf(ytest), _tol);                 \
    CONTEND_DELTA(cimagf(yf), cimagf(ytest), _tol);                 \
}                                                                   \
                                                                    \
/* test complex logarithm */                                        \
void CQ(_test_clog)(float complex _xf,                              \
                    float         _tol)                             \
{                                                                   \
    /* convert to fixed-point and back to float */                  \
    CQ(_t) x = CQ(_float_to_fixed)(_xf);                            \
    _xf = CQ(_fixed_to_float)(x);                                   \
                                                                    \
    /* execute operation */                                         \
    CQ(_t) y = CQ(_clog)(x);                                        \
    float complex yf = clogf(_xf);                                  \
                                                                    \
    /* convert to floating-point */                                 \
    float complex ytest = CQ(_fixed_to_float)(y);                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("clog(%7.3f,%7.3f) = (%7.3f,%7.3f), expected: (%7.3f,%7.3f), e=(%7.3f,%7.3f)\n",\
                crealf(_xf),   cimagf(_xf),                         \
                crealf(ytest), cimagf(ytest),                       \
                crealf(yf),    cimagf(yf),                          \
                crealf(ytest-yf), cimagf(ytest-yf));                \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(crealf(yf), crealf(ytest), _tol);                 \
    CONTEND_DELTA(cimagf(yf), cimagf(ytest), _tol);                 \
}                                                                   \


// define autotest API
LIQUIDFPM_AUTOTEST_COMPLEX_TRIG_API(LIQUIDFPM_MANGLE_Q16, LIQUIDFPM_MANGLE_CQ16)
LIQUIDFPM_AUTOTEST_COMPLEX_TRIG_API(LIQUIDFPM_MANGLE_Q32, LIQUIDFPM_MANGLE_CQ32)


// 
// q16
//

void autotest_cq16_cexp()
{
    float tol = 2*expf(-sqrtf(q16_fracbits));

    // run specific tests
    cq16_test_cexp( -0.32091592 +   1.00546825*_Complex_I, tol);
    cq16_test_cexp( -1.54992478 +   0.95891523*_Complex_I, tol);
    cq16_test_cexp( -0.32752434 +   0.91907729*_Complex_I, tol);
    cq16_test_cexp(  1.31155180 +  -0.16143933*_Complex_I, tol);
    cq16_test_cexp(  0.80030766 +  -0.55737970*_Complex_I, tol);
    cq16_test_cexp( -1.13918412 +   2.52215849*_Complex_I, tol);
    cq16_test_cexp(  0.21570295 +  -1.54983320*_Complex_I, tol);
    cq16_test_cexp( -2.26060000 +  -0.15487264*_Complex_I, tol);
    cq16_test_cexp(  0.34047877 +   0.26361518*_Complex_I, tol);
    cq16_test_cexp(  0.76072814 +  -1.14659286*_Complex_I, tol);
}

void autotest_cq16_clog()
{
    float tol = 2*expf(-sqrtf(q16_fracbits));

    // run specific tests
    cq16_test_clog( -0.32873167 +  -0.30652276*_Complex_I, tol);
    cq16_test_clog(  2.01997100 +   0.74187958*_Complex_I, tol);
    cq16_test_clog(  0.91003273 +  -0.82007906*_Complex_I, tol);
    cq16_test_clog( -0.14075784 +  -0.22242021*_Complex_I, tol);
    cq16_test_clog( -1.38414348 +  -1.14966042*_Complex_I, tol);
    cq16_test_clog( -0.22969809 +   1.18546492*_Complex_I, tol);
    cq16_test_clog( -0.79253234 +   1.26513938*_Complex_I, tol);
    cq16_test_clog(  1.41184203 +  -0.67068770*_Complex_I, tol);
    cq16_test_clog(  2.32491679 +   0.50618527*_Complex_I, tol);
    cq16_test_clog(  1.04258573 +  -1.65468131*_Complex_I, tol);
}

// 
// q32
//

void autotest_cq32_cexp()
{
    float tol = 2*expf(-sqrtf(q32_fracbits));

    // run specific tests
    cq32_test_cexp( -0.90374735 +  -0.35289197*_Complex_I, tol);
    cq32_test_cexp( -0.03863707 +  -0.46340675*_Complex_I, tol);
    cq32_test_cexp(  3.09104298 +  -0.32755830*_Complex_I, tol);
    cq32_test_cexp( -0.52379339 +   1.02972071*_Complex_I, tol);
    cq32_test_cexp(  1.55655451 +  -0.87949754*_Complex_I, tol);
    cq32_test_cexp(  0.46198816 +  -1.22660447*_Complex_I, tol);
    cq32_test_cexp(  1.69870228 +  -0.07782690*_Complex_I, tol);
    cq32_test_cexp( -0.57139088 +  -1.58083906*_Complex_I, tol);
    cq32_test_cexp(  1.00685876 +  -1.73345613*_Complex_I, tol);
    cq32_test_cexp(  1.02328892 +  -0.38534601*_Complex_I, tol);
}

void autotest_cq32_clog()
{
    float tol = 2*expf(-sqrtf(q32_fracbits));

    // run specific tests
    cq32_test_clog( -0.12359964 +  -0.95961656*_Complex_I, tol);
    cq32_test_clog(  1.29193309 +  -0.06862404*_Complex_I, tol);
    cq32_test_clog( -0.01218027 +  -0.23820575*_Complex_I, tol);
    cq32_test_clog(  0.89919723 +   1.37154217*_Complex_I, tol);
    cq32_test_clog(  0.02859159 +  -0.59009684*_Complex_I, tol);
    cq32_test_clog( -0.96683876 +   0.55981945*_Complex_I, tol);
    cq32_test_clog(  1.15005638 +  -1.11234641*_Complex_I, tol);
    cq32_test_clog(  0.07439212 +  -0.25619313*_Complex_I, tol);
    cq32_test_clog( -0.72062350 +  -0.26901069*_Complex_I, tol);
    cq32_test_clog( -1.19676390 +   0.40063869*_Complex_I, tol);
}

