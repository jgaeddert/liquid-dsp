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
#define LIQUIDFPM_AUTOTEST_ATAN_API(Q)                              \
                                                                    \
/* test atan2 using CORDIC method */                                \
void Q(_test_atan2_cordic)(float        _yf,                        \
                           float        _xf,                        \
                           unsigned int _precision,                 \
                           float        _tol)                       \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    Q(_t) y = Q(_float_to_fixed)(_yf);                              \
    Q(_t) r;                                                        \
    Q(_t) theta;                                                    \
                                                                    \
    /* execute operation */                                         \
    Q(_atan2_cordic)(y, x, &r, &theta, _precision);                 \
    float rf = sqrtf(_xf*_xf + _yf*_yf);                            \
    float thetaf = atan2f(_yf, _xf);                                \
                                                                    \
    /* convert to floating-point */                                 \
    float r_test = Q(_fixed_to_float)(r);                           \
    float theta_test = Q(_angle_fixed_to_float)(theta);             \
                                                                    \
    /* TODO : unwrap phase terms */                                 \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("(%12.8f,%12.8f): r, theta = %12.8f(%12.8f), %12.8f(%12.8f)\n",\
                _yf, _xf,                                           \
                r_test, rf,                                         \
                theta_test, thetaf);                                \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(r_test,     rf,     _tol);                        \
    CONTEND_DELTA(theta_test, thetaf, _tol);                        \
}                                                                   \
                                                                    \
/* test atan2 using look-up table method */                         \
void Q(_test_atan2_frac)(float _yf,                                 \
                         float _xf,                                 \
                         float _tol)                                \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
    Q(_t) y = Q(_float_to_fixed)(_yf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) theta = Q(_atan2_frac)(y, x);                             \
    float thetaf = atan2f(_yf, _xf);                                \
                                                                    \
    /* convert to floating-point */                                 \
    float theta_test = Q(_angle_fixed_to_float)(theta);             \
                                                                    \
    /* TODO : unwrap phase terms */                                 \
    while ( (theta_test-thetaf) >  M_PI ) thetaf += 2*M_PI;         \
    while ( (theta_test-thetaf) < -M_PI ) thetaf -= 2*M_PI;         \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("(%12.8f,%12.8f): theta = %12.8f(%12.8f), e=%12.8f\n",\
                _yf, _xf,                                           \
                theta_test, thetaf,                                 \
                theta_test -thetaf);                                \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(theta_test, thetaf, _tol);                        \
}

// define autotest API
LIQUIDFPM_AUTOTEST_ATAN_API(LIQUIDFPM_MANGLE_Q32)
LIQUIDFPM_AUTOTEST_ATAN_API(LIQUIDFPM_MANGLE_Q16)

// 
// q16
//

void autotest_q16_atan2_cordic()
{
    unsigned int precision = 16;
    unsigned int num_steps = 37;
    float theta_min = -2*M_PI;
    float theta_max =  2*M_PI;
    float dtheta = (theta_max - theta_min)/((float)(num_steps-1));
    float rmin      =  0.1f;
    float rmax      =  1.0f;
    float dr = (rmax - rmin)/((float)(num_steps-1));
    float tol = expf(-sqrtf(q16_fracbits));

    // testing variables
    unsigned int i;
    float theta = theta_min;
    float r = rmin;
    for (i=0; i<num_steps; i++) {
        // compute "true" (floating-point) value(s)
        float x = r*cosf(theta);
        float y = r*sinf(theta);

        // run test
        q16_test_atan2_cordic(x,y,precision,tol);

        // increment input parameter(s)
        theta += dtheta;
        r     += dr;
    }
}

void autotest_q16_atan2_frac()
{
    unsigned int num_steps = 79;
    float theta_min = -2*M_PI;
    float theta_max =  2*M_PI;
    float dtheta = (theta_max - theta_min)/((float)(num_steps-1));
    float rmin      =  0.1f;
    float rmax      =  1.0f;
    float dr = (rmax - rmin)/((float)(num_steps-1));
    float tol = 0.25f;  // error determined by look-up table precision

    // testing variables
    unsigned int i;
    float theta = theta_min;
    float r = rmin;
    for (i=0; i<num_steps; i++) {
        // compute "true" (floating-point) value(s)
        float x = r*cosf(theta);
        float y = r*sinf(theta);

        // run test
        q16_test_atan2_frac(x,y,tol);

        // increment input parameter(s)
        theta += dtheta;
        r     += dr;
    }
#if 0
    // manual tests
    float r     = 1.0f;
    float theta = 0.9f;

    float x = r*cosf(theta);
    float y = r*sinf(theta);

    // run test
    float tol = 0.1f;
    q16_test_atan2_frac(x,y,tol);
#endif
}


// 
// q32
//

void autotest_q32_atan2_cordic()
{
    unsigned int precision = 16;
    unsigned int num_steps = 37;
    float theta_min = -2*M_PI;
    float theta_max =  2*M_PI;
    float dtheta = (theta_max - theta_min)/((float)(num_steps-1));
    float rmin      =  0.1f;
    float rmax      =  1.0f;
    float dr = (rmax - rmin)/((float)(num_steps-1));
    float tol = expf(-sqrtf(q32_fracbits));

    // testing variables
    unsigned int i;
    float theta = theta_min;
    float r = rmin;
    for (i=0; i<num_steps; i++) {
        // compute "true" (floating-point) value(s)
        float x = r*cosf(theta);
        float y = r*sinf(theta);

        // run test
        q32_test_atan2_cordic(x,y,precision,tol);

        // increment input parameter(s)
        theta += dtheta;
        r     += dr;
    }
}

void autotest_q32_atan2_frac()
{
    unsigned int num_steps = 79;
    float theta_min = -2*M_PI;
    float theta_max =  2*M_PI;
    float dtheta = (theta_max - theta_min)/((float)(num_steps-1));
    float rmin      =  0.1f;
    float rmax      =  1.0f;
    float dr = (rmax - rmin)/((float)(num_steps-1));
    float tol = 0.25f;  // error determined by look-up table precision

    // testing variables
    unsigned int i;
    float theta = theta_min;
    float r = rmin;
    for (i=0; i<num_steps; i++) {
        // compute "true" (floating-point) value(s)
        float x = r*cosf(theta);
        float y = r*sinf(theta);

        // run test
        q32_test_atan2_frac(x,y,tol);

        // increment input parameter(s)
        theta += dtheta;
        r     += dr;
    }
}


