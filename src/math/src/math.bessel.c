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
// Useful mathematical formulae
//
// References:
//  [Kaiser:1980] James F. Kaiser and Ronald W. Schafer, "On
//      the Use of I0-Sinh Window for Spectrum Analysis,"
//      IEEE Transactions on Acoustics, Speech, and Signal
//      Processing, vol. ASSP-28, no. 1, pp. 105--107,
//      February, 1980.
//  [harris:1978] frederic j. harris, "On the Use of Windows for Harmonic
//      Analysis with the Discrete Fourier Transform," Proceedings of the
//      IEEE, vol. 66, no. 1, January, 1978.
//  [Helstrom:1960] Helstrom, C. W. Statistical Theory of Signal
//      Detection. New York: Pergamon, 1960
//  [Proakis:2001] Proakis, J. Digital Communications. New York:
//      McGraw-Hill, 2001

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// Bessel function of the first kind
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

// Modified bessel function of the first kind
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

// Modified bessel function of the first kind
// TODO : check this computation
#define NUM_BESSELI_ITERATIONS 16
float liquid_besseli(float _nu,
                     float _z)
{
    unsigned int k;
    float t0 = powf(_z*0.5f, _nu);
    float t1 = 1.0f;
    float y=0.0f;
    for (k=0; k<NUM_BESSELI_ITERATIONS; k++) {
        // compute: k! * Gamma(nu + k +1)
        float t2 = liquid_factorialf(k) * liquid_gammaf(_nu + (float)k + 1.0f);

        // accumulate y
        y += t1 / t2;

        // update t1 = (0.25*z^2)^k
        t1 *= 0.25f*_z*_z;
    }
    return t0 * y;
}

