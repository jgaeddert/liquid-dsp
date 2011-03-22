/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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

//
// Bessel Functions
//
// liquid_besseli       :   modified Bessel function of the first kind
// liquid_besselj_0     :   Bessel function of the first kind (order 0)
// liquid_besseli_0     :   modified Bessel function of the first kind (order 0)
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// log(I_v(z)) : log Modified Bessel function of the first kind
#define NUM_BESSELI_ITERATIONS 64
float liquid_lnbesseli(float _nu,
                       float _z)
{
    // TODO : validate input

    float t0 = _nu*logf(0.5f*_z);
    float t1 = 0.0f;
    float t2 = 0.0f;
    float t3 = 0.0f;
    float y = 0.0f;

    unsigned int k;
    for (k=0; k<NUM_BESSELI_ITERATIONS; k++) {
        // compute log( (z^2/4)^k )
        t1 = 2.0f * k * logf(0.5f*_z);

        // compute: log( k! * Gamma(nu + k +1) )
        t2 = liquid_lngammaf((float)k + 1.0f);
        t3 = liquid_lngammaf(_nu + (float)k + 1.0f);

        // accumulate y
        y += expf( t1 - t2 - t3 );
    }

    return t0 + logf(y);
}

// I_v(z) : Modified Bessel function of the first kind
float liquid_besseli(float _nu,
                     float _z)
{
    return expf( liquid_lnbesseli(_nu, _z) );
}

// J_0(z) : Bessel function of the first kind (order zero)
#define NUM_BESSELJ0_ITERATIONS 16
float liquid_besselj_0(float _z)
{
    // large signal approximation, see
    // Gross, F. B "New Approximations to J0 and J1 Bessel Functions,"
    //   IEEE Trans. on Antennas and Propagation, vol. 43, no. 8,
    //   August, 1995
    if (fabsf(_z) > 10.0f)
        return sqrtf(2/(M_PI*fabsf(_z)))*cosf(fabsf(_z)-M_PI/4);

    unsigned int k;
    float t, y=0.0f;
    for (k=0; k<NUM_BESSELJ0_ITERATIONS; k++) {
        t = powf(_z/2, (float)k) / tgamma((float)k+1);
        y += (k%2) ? -t*t : t*t;
    }
    return y;
}

// I_0(z) : Modified bessel function of the first kind (order zero)
#define NUM_BESSELI0_ITERATIONS 32
float liquid_besseli_0(float _z)
{
    unsigned int k;
    float t, y=0.0f;
    for (k=0; k<NUM_BESSELI0_ITERATIONS; k++) {
#if 0
        t = powf(_z/2, (float)k) / tgamma((float)k+1);
        y += t*t;
#else
        t = k * logf(0.5f*_z) - liquid_lngammaf((float)k + 1.0f);
        y += expf(2*t);
#endif
    }
    return y;
}

