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
#define LIQUIDFPM_AUTOTEST_LOG2_FRAC_API(Q)                         \
void Q(_test_log2_frac)(float _xf,                                  \
                        float _tol)                                 \
{                                                                   \
    /* convert to fixed-point */                                    \
    Q(_t) x = Q(_float_to_fixed)(_xf);                              \
                                                                    \
    /* execute operation */                                         \
    Q(_t) y = Q(_log2_frac)(x);                                     \
    float yf = log2f(_xf);                                          \
                                                                    \
    /* convert to floating-point */                                 \
    float ytest = Q(_fixed_to_float)(y);                            \
                                                                    \
    if (liquid_autotest_verbose) {                                  \
        printf("log2(%12.8f) = %12.8f (%12.8f), e = %12.8f\n",      \
                     _xf,      ytest,  yf,          ytest-yf);      \
    }                                                               \
                                                                    \
    /* run comparison */                                            \
    CONTEND_DELTA(yf,ytest,_tol);                                   \
}

// define autotest API
LIQUIDFPM_AUTOTEST_LOG2_FRAC_API(LIQUIDFPM_MANGLE_Q16)
LIQUIDFPM_AUTOTEST_LOG2_FRAC_API(LIQUIDFPM_MANGLE_Q32)

void autotest_q16_log2_frac()
{
    unsigned int num_steps = 37;
    float xmin = q16_fixed_to_float( 1 << 8  ); // min input
    float xmax = q16_fixed_to_float( q16_max ); // max input
    float ymin = q16_fixed_to_float(-q16_max ); // min output
    //float ymax = q16_fixed_to_float( q16_max ); // max output
    
    // adjust input accordingly
    if (xmin < exp2f(ymin)) xmin = exp2f(ymin);
    //if (xmax > exp2f(ymax)) xmax = exp2f(ymax);

    //float dx = (xmax - xmin)/((float)(num_steps-1));
    float sigma = powf(xmin/xmax,-(1.0f - 1e-6f)/(float)(num_steps-1));
    //float tol = expf(-sqrtf(q16_fracbits));
    float tol = 0.01f;

    // testing variables
    float xf;

    unsigned int i;
    xf = xmin;
    for (i=0; i<num_steps; i++) {
        if (xf > q16_fixed_to_float(q16_max))
            continue;

        // run test
        q16_test_log2_frac(xf,tol);

        // increment input parameter
        //xf += dx;
        xf *= sigma;
    }
}

void autotest_q32_log2_frac()
{
    unsigned int num_steps = 37;
    float xmin = q32_fixed_to_float( 1 << 8  ); // min input
    float xmax = q32_fixed_to_float( q32_max ); // max input
    float ymin = q32_fixed_to_float(-q32_max ); // min output
    //float ymax = q32_fixed_to_float( q32_max ); // max output
    
    // adjust input accordingly
    if (xmin < exp2f(ymin)) xmin = exp2f(ymin);
    //if (xmax > exp2f(ymax)) xmax = exp2f(ymax);

    //float dx = (xmax - xmin)/((float)(num_steps-1));
    float sigma = powf(xmin/xmax,-(1.0f - 1e-6f)/(float)(num_steps-1));
    //float tol = expf(-sqrtf(q32_fracbits));
    float tol = 0.01f;

    // testing variables
    float xf;

    unsigned int i;
    xf = xmin;
    for (i=0; i<num_steps; i++) {
        if (xf > q32_fixed_to_float(q32_max))
            continue;

        // run test
        q32_test_log2_frac(xf,tol);

        // increment input parameter
        //xf += dx;
        xf *= sigma;
    }
}

