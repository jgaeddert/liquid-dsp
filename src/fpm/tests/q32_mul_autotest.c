/*
 * Copyright (c) 2008, 2009, 2012 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2012 Virginia Polytechnic
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

#include "autotest/autotest.h"
#include "liquidfpm.internal.h"

// helper function to keep code base small
void q32_test_mul(float _xf,
                 float _yf,
                 float _tol)
{
    // convert to fixed-point
    q32_t x = q32_float_to_fixed(_xf);
    q32_t y = q32_float_to_fixed(_yf);

    // execute operation
    q32_t z = q32_mul(x,y);
    float zf = _xf * _yf;

    // convert to floating-point
    float ztest = q32_fixed_to_float(z);

    // run comparison
    CONTEND_DELTA(zf,ztest,_tol);

    if (liquid_autotest_verbose) {
        printf("%12.8f * %12.8f = %12.8f (%12.8f)\n",
                _xf,     _yf,     ztest,  zf);
    }
}

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

