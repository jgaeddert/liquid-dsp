/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
 *                                      Institute & State University
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

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.internal.h"

// 
// AUTOTEST: basic dot product
//
void autotest_dotprod_rrrq16_basic()
{
    float tol = expf(-sqrtf(q16_fracbits));

    float hf[16] = {
    -0.050565, -0.952580,  0.274320,  1.232400, 
     1.268200,  0.565770,  0.800830,  0.923970, 
     0.517060, -0.530340, -0.378550, -1.127100, 
     1.123100, -1.006000, -1.483800, -0.062007
    };

    float xf[16] = {
    -0.384280, -0.812030,  0.156930,  1.919500, 
     0.564580, -0.123610, -0.138640,  0.004984, 
    -1.100200, -0.497620,  0.089977, -1.745500, 
     0.463640,  0.592100,  1.150000, -1.225400
    };

    float yf_test = 3.66411513609863;

    // convert to fixed-point arrays
    q16_t h[16];
    q16_t x[16];
    q16_t y;
    unsigned int i;
    for (i=0; i<16; i++) {
        h[i] = q16_float_to_fixed(hf[i]);
        x[i] = q16_float_to_fixed(xf[i]);
    }

    // compoute dot product
    dotprod_rrrq16_run(h,x,16,&y);

    // convert result
    float yf = q16_fixed_to_float(y);
    if (liquid_autotest_verbose) {
        printf("  dotprod_rrrq16 : %12.8f (expected %12.8f), e=%12.8f, tol=%12.8f\n",
                yf, yf_test, yf-yf_test, tol);
    }

    CONTEND_DELTA(yf, yf_test, tol);
}

