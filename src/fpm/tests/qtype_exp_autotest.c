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
#define LIQUIDFPM_AUTOTEST_EXP_API(Q)                               \
void Q(_test_exp2_shiftadd)(float        _xf,                       \
                            unsigned int _precision,                \
                            float        _tol)                      \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_exp2_shiftadd)(x,_precision);                      \
    float yf = exp2f(_xf);                                          \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("exp2(%12.8f) = %12.8f (%12.8f), e = %12.8f\n",      \
                     _xf,      ytest,  yf,          ytest-yf);      \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(yf,ytest,_tol);                                   \
}

// define autotest APIs
LIQUIDFPM_AUTOTEST_EXP_API(LIQUIDFPM_MANGLE_Q16)
LIQUIDFPM_AUTOTEST_EXP_API(LIQUIDFPM_MANGLE_Q32)

void autotest_q16_exp2_shiftadd()
{
    unsigned int precision = 16;
    unsigned int num_steps = 77;

    // compute lower range
    float xmin0= -q16_fixed_to_float(q16_max);
    float xmin1= -0.99f*(float)(q16_fracbits);
    float xmin = xmin0 > xmin1 ? xmin0 : xmin1;

    // upper range
    float xmax0 = (float)(q16_intbits-1)*0.99f;
    float xmax1 = 0.99f*(float)(1<<q16_intbits);
    float xmax  = xmax0 < xmax1 ? xmax0 : xmax1;

    float dx   = (xmax - xmin)/((float)(num_steps-1));
    //float tol  = expf(-sqrtf(q16_fracbits));
    float tol = 0.7f;   // TODO: check proper tolerance
    
    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q16_test_exp2_shiftadd(x,precision,tol);

        // increment input parameter
        x += dx;
    }
}

void autotest_q32_exp2_shiftadd()
{
    unsigned int precision = 16;
    unsigned int num_steps = 77;
    
    // compute lower range
    float xmin0 = -q32_fixed_to_float(q32_max);
    float xmin1 = -0.99f*(float)(q32_fracbits);
    float xmin  = xmin0 > xmin1 ? xmin0 : xmin1;

    // upper range
    float xmax0 = (float)(q32_intbits-1)*0.99f;
    float xmax1 = 0.99f*(float)(1<<q32_intbits);
    float xmax  = xmax0 < xmax1 ? xmax0 : xmax1;

    float dx   = (xmax - xmin)/((float)(num_steps-1));
    float tol  = expf(-sqrtf(q32_fracbits));

    unsigned int i;
    float x = xmin;
    for (i=0; i<num_steps; i++) {
        // run test
        q32_test_exp2_shiftadd(x,precision,tol);

        // increment input parameter
        x += dx;
    }
}

