/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

float lngammaf(float _z) {
    float g = 0.5*( logf(2*M_PI)-log(_z) );
    g += _z*( logf(_z+(1/(12.0f*_z-0.1f/_z)))-1);
    return g;
}

float gammaf(float _z) {
    return expf( lngammaf(_z) );
}

float factorialf(unsigned int _n) {
    return fabsf(gammaf((float)(_n+1)));
}

// Bessel function of the first kind
#define NUM_BESSELJ0_ITERATIONS 16
float besselj_0(float _z)
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
        t = powf(_z/2, (float)k) / gammaf((float)k+1);
        y += (k%2) ? -t*t : t*t;
    }
    return y;
}

// Modified bessel function of the first kind
#define NUM_BESSELI0_ITERATIONS 16
float besseli_0(float _z)
{
    unsigned int k;
    float t, y=0.0f;
    for (k=0; k<NUM_BESSELI0_ITERATIONS; k++) {
        t = powf(_z/2, (float)k) / gammaf((float)k+1);
        y += t*t;
    }
    return y;
}

//
float sincf(float _x) {
    // _x ~ 0 approximation
    //if (fabsf(_x) < 0.01f)
    //    return expf(-lngammaf(1+_x) - lngammaf(1-_x));

    // _x ~ 0 approximation
    // from : http://mathworld.wolfram.com/SincFunction.html
    // sinc(z) = \prod_{k=1}^{\infty} { cos(\pi z / 2^k) }
    if (fabsf(_x) < 0.01f)
        return cosf(M_PI*_x/2.0f)*cosf(M_PI*_x/4.0f)*cosf(M_PI*_x/8.0f);

    return sinf(M_PI*_x)/(M_PI*_x);
}

// 
// Windowing functions
//

// Kaiser window
float kaiser(unsigned int _n, unsigned int _N, float _beta, float _mu)
{
    // TODO add reference
    float t = (float)_n - (float)(_N-1)/2 + _mu;
    float r = 2.0f*t/(float)(_N);
    float a = besseli_0(_beta*sqrtf(1-r*r));
    float b = besseli_0(_beta);
    return a / b;
}

// Hamming window
float hamming(unsigned int _n, unsigned int _N)
{
    // TODO add reference
    return 0.53836 - 0.46164*cosf( (2*M_PI*(float)_n) / ((float)(_N-1)) );
}

// Hann window
float hann(unsigned int _n, unsigned int _N)
{
    // TODO test this function
    // TODO add reference
    return 0.5f - 0.5f*cosf( (2*M_PI*(float)_n) / ((float)(_N-1)) );
}

// Blackman-harris window
float blackmanharris(unsigned int _n, unsigned int _N)
{
    // TODO test this function
    // TODO add reference
    float a0 = 0.35875;
    float a1 = 0.48829;
    float a2 = 0.14128;
    float a3 = 0.01168;
    float t = 2*M_PI*(float)_n / ((float)(_N-1));

    return a0 - a1*cosf(t) + a2*cosf(2*t) - a3*cosf(3*t);
}


