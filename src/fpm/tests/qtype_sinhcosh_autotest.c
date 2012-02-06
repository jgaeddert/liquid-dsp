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
#define LIQUIDFPM_AUTOTEST_SINHCOSH_API(Q)                          \
                                                                    \
/* test sin|cos using CORDIC method */                              \
void Q(_test_sinhcosh_cordic)(float        _xf,                     \
                              unsigned int _precision,              \
                              float        _tol)                    \
{                                                                   \
    /* convert to fixed-point and back to float */                  \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
    Q(_t) qsinh;                                                    \
    Q(_t) qcosh;                                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_sinhcosh_cordic)(x, &qsinh, &qcosh, _precision);             \
    float fsinh = sinhf(_xf);                                       \
    float fcosh = coshf(_xf);                                       \
                                                                    \
    /* convert to floating-point */                                 \
    float qsinh_test = Q(_fixed_to_float)(qsinh);                   \
    float qcosh_test = Q(_fixed_to_float)(qcosh);                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("  sinh,cosh(%11.8f) = %11.8f(%11.8f), %11.8f(%11.8f)\n",\
                _xf,                                                \
                qsinh_test, fsinh,                                  \
                qcosh_test, fcosh);                                 \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(qsinh_test, fsinh, _tol);                         \
    CONTEND_DELTA(qcosh_test, fcosh, _tol);                         \
}                                                                   \
                                                                    \
/* test sin|cos using shift|add method */                           \
void Q(_test_sinhcosh_shiftadd)(float        _xf,                   \
                                unsigned int _precision,            \
                                float        _tol)                  \
{                                                                   \
    /* convert to fixed-point and back to float */                  \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    _xf = Q(_fixed_to_float)(x);                                    \
    Q(_t) qsinh;                                                    \
    Q(_t) qcosh;                                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_sinhcosh_shiftadd)(x, &qsinh, &qcosh, _precision);           \
    float fsinh = sinhf(_xf);                                       \
    float fcosh = coshf(_xf);                                       \
                                                                    \
    /* convert to floating-point */                                 \
    float qsinh_test = Q(_fixed_to_float)(qsinh);                   \
    float qcosh_test = Q(_fixed_to_float)(qcosh);                   \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("  sinh,cosh(%11.8f) = %11.8f(%11.8f), %11.8f(%11.8f)\n",\
                _xf,                                                \
                qsinh_test, fsinh,                                  \
                qcosh_test, fcosh);                                 \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(qsinh_test, fsinh, _tol);                         \
    CONTEND_DELTA(qcosh_test, fcosh, _tol);                         \
}                                                                   \

// define autotest API
LIQUIDFPM_AUTOTEST_SINHCOSH_API(LIQUIDFPM_MANGLE_Q16)
LIQUIDFPM_AUTOTEST_SINHCOSH_API(LIQUIDFPM_MANGLE_Q32)

// 
// q16
//

void autotest_q16_sinhcosh_cordic()
{
    // options
    unsigned int precision = q16_bits;
    unsigned int num_steps = 57;
    float tol = 0.1f;
    
    // determine qtype bounds
    float qmin = q16_fixed_to_float(q16_min);
    float qmax = q16_fixed_to_float(q16_max);

    // reduce lower bound until output is within qtype range
    float xmin = -1.1180f; //qmin;
    while ( sinh(xmin) > qmax || cosh(xmin) > qmax )
        xmin *= 0.99f;

    // reduce upper bound until output is within qtype range
    float xmax = 1.1180f; //qmax;
    while ( sinh(xmax) > qmax || cosh(xmax) > qmax )
        xmax *= 0.99f;

    if (liquid_autotest_verbose) {
        printf("  q16 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, sinhf(xmin), sinhf(xmax));
        printf("               [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, coshf(xmin), coshf(xmax));
    }

    unsigned int i;
    float dx = (xmax - xmin) / (float)(num_steps-1);
    for (i=0; i<num_steps; i++) {
        // compute input
        float x = xmin + i*dx;

        // run test
        q16_test_sinhcosh_cordic(x, precision, tol);
    }
}

void autotest_q16_sinhcosh_shiftadd()
{
    // options
    unsigned int precision = q16_bits;
    unsigned int num_steps = 57;
    float tol = 0.02f;
    
    // determine qtype bounds
    float qmin = q16_fixed_to_float(q16_min);
    float qmax = q16_fixed_to_float(q16_max);

#if 0
    // reduce lower bound until output is within qtype range
    float xmin = -qmax;
    while ( fabsf(sinh(xmin)) > qmax || fabsf(cosh(xmin)) > qmax )
        xmin *= 0.99f;

    // reduce upper bound until output is within qtype range
    float xmax = qmax;
    while ( sinh(xmax) > qmax || cosh(xmax) > qmax )
        xmax *= 0.99f;
#else
    float xmin = -1.5f;
    float xmax =  1.5f;
    AUTOTEST_WARN("hard-coding range");
#endif

    if (liquid_autotest_verbose) {
        printf("  q16 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, sinhf(xmin), sinhf(xmax));
        printf("               [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, coshf(xmin), coshf(xmax));
    }

    unsigned int i;
    float dx = (xmax - xmin) / (float)(num_steps-1);
    for (i=0; i<num_steps; i++) {
        // compute input
        float x = xmin + i*dx;

        // run test
        q16_test_sinhcosh_shiftadd(x, precision, tol);
    }
}

// 
// q32
//

void autotest_q32_sinhcosh_cordic()
{
    // options
    unsigned int precision = q32_bits;
    unsigned int num_steps = 57;
    float tol = 0.1f;
    
    // determine qtype bounds
    float qmin = q32_fixed_to_float(q32_min);
    float qmax = q32_fixed_to_float(q32_max);

    // reduce lower bound until output is within qtype range
    float xmin = -1.1180f; //qmin;
    while ( sinh(xmin) > qmax || cosh(xmin) > qmax )
        xmin *= 0.99f;

    // reduce upper bound until output is within qtype range
    float xmax = 1.1180f; //qmax;
    while ( sinh(xmax) > qmax || cosh(xmax) > qmax )
        xmax *= 0.99f;

    if (liquid_autotest_verbose) {
        printf("  q32 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, sinhf(xmin), sinhf(xmax));
        printf("               [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, coshf(xmin), coshf(xmax));
    }

    unsigned int i;
    float dx = (xmax - xmin) / (float)(num_steps-1);
    for (i=0; i<num_steps; i++) {
        // compute input
        float x = xmin + i*dx;

        // run test
        q32_test_sinhcosh_cordic(x, precision, tol);
    }
}

void autotest_q32_sinhcosh_shiftadd()
{
    // options
    unsigned int precision = q32_bits;
    unsigned int num_steps = 57;
    float tol = 0.001f;
    
    // determine qtype bounds
    float qmin = q32_fixed_to_float(q32_min);
    float qmax = q32_fixed_to_float(q32_max);

#if 0
    // reduce lower bound until output is within qtype range
    float xmin = -qmax;
    while ( fabsf(sinh(xmin)) > qmax || fabsf(cosh(xmin)) > qmax )
        xmin *= 0.99f;

    // reduce upper bound until output is within qtype range
    float xmax = qmax;
    while ( sinh(xmax) > qmax || cosh(xmax) > qmax )
        xmax *= 0.99f;
#else
    float xmin = -1.5f;
    float xmax =  1.5f;
    AUTOTEST_WARN("hard-coding range");
#endif

    if (liquid_autotest_verbose) {
        printf("  q32 bounds:  [%12.8f, %12.8f]\n", qmin, qmax);
        printf("  test bounds: [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, sinhf(xmin), sinhf(xmax));
        printf("               [%12.8f, %12.8f] > [%12.8f,%12.8f]\n", xmin, xmax, coshf(xmin), coshf(xmax));
    }

    unsigned int i;
    float dx = (xmax - xmin) / (float)(num_steps-1);
    for (i=0; i<num_steps; i++) {
        // compute input
        float x = xmin + i*dx;

        // run test
        q32_test_sinhcosh_shiftadd(x, precision, tol);
    }
}

