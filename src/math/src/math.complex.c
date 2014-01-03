/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

//
// Complex math functions (trig, log, exp, etc.)
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// complex square root
float complex liquid_csqrtf(float complex _z)
{
    float r = cabsf(_z);            // magnitude of _z
    float a = crealf(_z);           // real component of _z

    float re = sqrtf(0.5f*(r+a));   // real component of return value
    float im = sqrtf(0.5f*(r-a));   // imag component of return value

    // return value, retaining sign of imaginary component
    return cimagf(_z) > 0 ? re + _Complex_I*im :
                            re - _Complex_I*im;
}

// complex exponent
float complex liquid_cexpf(float complex _z)
{
    float r = expf( crealf(_z) );
    float re = cosf( cimagf(_z) );
    float im = sinf( cimagf(_z) );

    return r * ( re + _Complex_I*im );
}

// complex logarithm
float complex liquid_clogf(float complex _z)
{
    return logf(cabsf(_z)) + _Complex_I*cargf(_z);
}

// complex arcsin
float complex liquid_casinf(float complex _z)
{
    return 0.5f*M_PI - liquid_cacosf(_z);
}

// complex arccos
float complex liquid_cacosf(float complex _z)
{
    // return based on quadrant
    int sign_i = crealf(_z) > 0;
    int sign_q = cimagf(_z) > 0;

    if (sign_i == sign_q) {
        return -_Complex_I*liquid_clogf(_z + liquid_csqrtf(_z*_z - 1.0f));
    } else {
        return -_Complex_I*liquid_clogf(_z - liquid_csqrtf(_z*_z - 1.0f));
    }

    // should never get to this state
    return 0.0f;
}

// complex arctan
float complex liquid_catanf(float complex _z)
{
    float complex t0 = 1.0f - _Complex_I*_z;
    float complex t1 = 1.0f + _Complex_I*_z;

    return 0.5f*_Complex_I*liquid_clogf( t0 / t1 );
}

// approximation to cargf() but faster
float liquid_cargf_approx(float complex _x)
{
    float theta;
    float xi = crealf(_x);
    float xq = cimagf(_x);

    if (xi == 0.0f) {
        if (xq == 0.0f)
            return 0.0f;
        return xq > 0.0f ? M_PI_2 : -M_PI_2;
    } else {
        theta = xq / fabsf(xi);
    }

    if (theta >  M_PI_2)
        theta =  M_PI_2;
    else if (theta < -M_PI_2)
        theta = -M_PI_2;
    return theta;
}

