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
// utilities.c : n-dimensional utility functions
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// n-dimensional Rosenbrock utility function, minimum at _v = {1,1,1...}
float liquid_rosenbrock(void *       _userdata,
                        float *      _v,
                        unsigned int _n)
{
    if (_n == 0) {
        fprintf(stderr,"error: liquid_rosenbrock(), input vector length cannot be zero\n");
        exit(1);
    } else if (_n == 1) {
        return (1.0f-_v[0])*(1.0f-_v[0]);
    }

    float u=0.0f;
    unsigned int i;
    for (i=0; i<_n-1; i++)
        u += powf(1-_v[i],2) + 100*powf( _v[i+1] - powf(_v[i],2), 2);

    return u;
}

// n-dimensional inverse Gauss utility function (minimum at _v = {1,1,1...}
//  _userdata   :   user-defined data structure (convenience)
//  _v          :   input vector [size: _n x 1]
//  _n          :   input vector size
float liquid_invgauss(void *       _userdata,
                      float *      _v,
                      unsigned int _n)
{
    if (_n == 0) {
        fprintf(stderr,"error: liquid_invgauss(), input vector length cannot be zero\n");
        exit(1);
    }

    float t = 0.0f;
    float sigma = 1.0f;
    unsigned int i;
    for (i=0; i<_n; i++) {
        t += (_v[i]-1.0f)*(_v[i]-1.0f) / (sigma*sigma);

        // increase variance along this dimension
        sigma *= 1.5f;
    }

    return 1 - expf(-t);
}

// n-dimensional multimodal utility function (minimum at _v = {0,0,0...}
//  _userdata   :   user-defined data structure (convenience)
//  _v          :   input vector [size: _n x 1]
//  _n          :   input vector size
float liquid_multimodal(void *       _userdata,
                        float *      _v,
                        unsigned int _n)
{
    if (_n == 0) {
        fprintf(stderr,"error: liquid_multimodal(), input vector length cannot be zero\n");
        exit(1);
    }

    float t0 = 1.0f;
    float t1 = 0.0f;
    float sigma = 4.0f;

    unsigned int i;
    for (i=0; i<_n; i++) {
        t0 *= 0.5f + 0.5f*cosf(2*M_PI*_v[i]);
        t1 += _v[i]*_v[i] / (sigma*sigma);
    }
    //t0 = powf(t0, 1.0f / (float)_n);

    return 1.0f - t0*expf(-t1);
}

// n-dimensional spiral utility function (minimum at _v = {0,0,0...}
//  _userdata   :   user-defined data structure (convenience)
//  _v          :   input vector [size: _n x 1]
//  _n          :   input vector size
float liquid_spiral(void *       _userdata,
                    float *      _v,
                    unsigned int _n)
{
    if (_n == 0) {
        fprintf(stderr,"error: liquid_rosenbrock(), input vector length cannot be zero\n");
        exit(1);
    } else if (_n == 1) {
        return _v[0]*_v[0];
    }

    // n is at least 2
    float r_hat     = sqrtf(_v[0]*_v[0] + _v[1]*_v[1]);
    float theta_hat = atan2f(_v[1], _v[0]);

    float delta = theta_hat - r_hat * 10;
    while (delta >  M_PI) delta -= 2*M_PI;
    while (delta < -M_PI) delta += 2*M_PI;

    delta = delta / M_PI;

    float u = 1 - delta*delta*expf(-r_hat*r_hat/10);

    // additional error...
    unsigned int i;
    for (i=2; i<_n; i++)
        u += _v[i]*_v[i];

    return u;
}

