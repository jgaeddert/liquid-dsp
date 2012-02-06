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

void autotest_q16_constants()
{
    // determine appropriate tolerance
    float tol = 0.001f;

    // math constants
    CONTEND_DELTA( q16_fixed_to_float(q16_E),        M_E,       tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_LOG2E),    M_LOG2E,   tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_LOG10E),   M_LOG10E,  tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_LN2),      M_LN2,     tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_LN10),     M_LN10,    tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_PI),       M_PI,      tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_PI_2),     M_PI_2,    tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_PI_4),     M_PI_4,    tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_1_PI),     M_1_PI,    tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_2_PI),     M_2_PI,    tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_2_SQRTPI), M_2_SQRTPI,tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_SQRT2),    M_SQRT2,   tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_SQRT1_2),  M_SQRT1_2, tol);

    // internals
    CONTEND_DELTA( q16_fixed_to_float(q16_ln2pi),   log(2*M_PI),tol);
    CONTEND_DELTA( q16_fixed_to_float(q16_inv_12),  1. / 12.,   tol);

    if (liquid_autotest_verbose) {
        printf("  q16 constants:\n");
        printf("    E        %12.8f (%12.8f) // e\n",          q16_fixed_to_float(q16_E),          M_E);
        printf("    LOG2E    %12.8f (%12.8f) // log2(e)\n",    q16_fixed_to_float(q16_LOG2E),      M_LOG2E);
        printf("    LOG10E   %12.8f (%12.8f) // log10(e)\n",   q16_fixed_to_float(q16_LOG10E),     M_LOG10E);
        printf("    LN2      %12.8f (%12.8f) // log(2)\n",     q16_fixed_to_float(q16_LN2),        M_LN2);
        printf("    LN10     %12.8f (%12.8f) // log(10)\n",    q16_fixed_to_float(q16_LN10),       M_LN10);
        printf("    PI       %12.8f (%12.8f) // pi\n",         q16_fixed_to_float(q16_PI),         M_PI);
        printf("    PI_2     %12.8f (%12.8f) // pi/2\n",       q16_fixed_to_float(q16_PI_2),       M_PI_2);
        printf("    PI_4     %12.8f (%12.8f) // pi/4\n",       q16_fixed_to_float(q16_PI_4),       M_PI_4);
        printf("    1_PI     %12.8f (%12.8f) // 1/pi\n",       q16_fixed_to_float(q16_1_PI),       M_1_PI);
        printf("    2_PI     %12.8f (%12.8f) // 2/pi\n",       q16_fixed_to_float(q16_2_PI),       M_2_PI);
        printf("    2_SQRTPI %12.8f (%12.8f) // 2/sqrt(pi)\n", q16_fixed_to_float(q16_2_SQRTPI),   M_2_SQRTPI);
        printf("    SQRT2    %12.8f (%12.8f) // sqrt(2)\n",    q16_fixed_to_float(q16_SQRT2),      M_SQRT2);
        printf("    SQRT1_2  %12.8f (%12.8f) // 1/sqrt(2)\n",  q16_fixed_to_float(q16_SQRT1_2),    M_SQRT1_2);

        printf("    ln2pi    %12.8f (%12.8f) // ln(2*pi)\n",   q16_fixed_to_float(q16_ln2pi),      log(2*M_PI));
        printf("    inv_12   %12.8f (%12.8f) // 1/12\n",       q16_fixed_to_float(q16_inv_12),     1. / 12.);
    }

}
void autotest_q32_constants()
{
    // determine appropriate tolerance
    float tol = 0.000001f;

    // math constants
    CONTEND_DELTA( q32_fixed_to_float(q32_E),        M_E,       tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_LOG2E),    M_LOG2E,   tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_LOG10E),   M_LOG10E,  tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_LN2),      M_LN2,     tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_LN10),     M_LN10,    tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_PI),       M_PI,      tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_PI_2),     M_PI_2,    tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_PI_4),     M_PI_4,    tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_1_PI),     M_1_PI,    tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_2_PI),     M_2_PI,    tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_2_SQRTPI), M_2_SQRTPI,tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_SQRT2),    M_SQRT2,   tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_SQRT1_2),  M_SQRT1_2, tol);

    // internals
    CONTEND_DELTA( q32_fixed_to_float(q32_ln2pi),   log(2*M_PI),tol);
    CONTEND_DELTA( q32_fixed_to_float(q32_inv_12),  1. / 12.,   tol);

    if (liquid_autotest_verbose) {
        printf("  q32 constants:\n");
        printf("    E        %12.8f (%12.8f) // e\n",          q32_fixed_to_float(q32_E),          M_E);
        printf("    LOG2E    %12.8f (%12.8f) // log2(e)\n",    q32_fixed_to_float(q32_LOG2E),      M_LOG2E);
        printf("    LOG10E   %12.8f (%12.8f) // log10(e)\n",   q32_fixed_to_float(q32_LOG10E),     M_LOG10E);
        printf("    LN2      %12.8f (%12.8f) // log(2)\n",     q32_fixed_to_float(q32_LN2),        M_LN2);
        printf("    LN10     %12.8f (%12.8f) // log(10)\n",    q32_fixed_to_float(q32_LN10),       M_LN10);
        printf("    PI       %12.8f (%12.8f) // pi\n",         q32_fixed_to_float(q32_PI),         M_PI);
        printf("    PI_2     %12.8f (%12.8f) // pi/2\n",       q32_fixed_to_float(q32_PI_2),       M_PI_2);
        printf("    PI_4     %12.8f (%12.8f) // pi/4\n",       q32_fixed_to_float(q32_PI_4),       M_PI_4);
        printf("    1_PI     %12.8f (%12.8f) // 1/pi\n",       q32_fixed_to_float(q32_1_PI),       M_1_PI);
        printf("    2_PI     %12.8f (%12.8f) // 2/pi\n",       q32_fixed_to_float(q32_2_PI),       M_2_PI);
        printf("    2_SQRTPI %12.8f (%12.8f) // 2/sqrt(pi)\n", q32_fixed_to_float(q32_2_SQRTPI),   M_2_SQRTPI);
        printf("    SQRT2    %12.8f (%12.8f) // sqrt(2)\n",    q32_fixed_to_float(q32_SQRT2),      M_SQRT2);
        printf("    SQRT1_2  %12.8f (%12.8f) // 1/sqrt(2)\n",  q32_fixed_to_float(q32_SQRT1_2),    M_SQRT1_2);

        printf("    ln2pi    %12.8f (%12.8f) // ln(2*pi)\n",   q32_fixed_to_float(q32_ln2pi),      log(2*M_PI));
        printf("    inv_12   %12.8f (%12.8f) // 1/12\n",       q32_fixed_to_float(q32_inv_12),     1. / 12.);
    }

}
