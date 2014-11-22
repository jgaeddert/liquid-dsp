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
//
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// Gauss
float randnf()
{
    // generate two uniform random numbers
    float u1, u2;

    // ensure u1 does not equal zero
    do {
        u1 = randf();
    } while (u1 == 0.0f);

    u2 = randf();

    return sqrtf(-2*logf(u1)) * sinf(2*M_PI*u2);
    //return sqrtf(-2*logf(u1)) * cosf(2*M_PI*u2);
}

void awgn(float *_x, float _nstd)
{
    *_x += randnf()*_nstd;
}

// Complex Gauss
void crandnf(float complex * _y)
{
    // generate two uniform random numbers
    float u1, u2;

    // ensure u1 does not equal zero
    do {
        u1 = randf();
    } while (u1 == 0.0f);

    u2 = randf();

    *_y = sqrtf(-2*logf(u1)) * cexpf(_Complex_I*2*M_PI*u2);
}

// Internal complex Gauss (inline)
float complex icrandnf()
{
    float complex y;
    crandnf(&y);
    return y;
}

void cawgn(float complex *_x, float _nstd)
{
    *_x += icrandnf()*_nstd*0.707106781186547f;
}

// Gauss random number probability distribution function
float randnf_pdf(float _x,
                 float _eta,
                 float _sig)
{
    // validate input
    if (_sig <= 0.0f) {
        fprintf(stderr,"error: randnf_pdf(), standard deviation must be greater than zero\n");
        exit(1);
    }

    float t  = _x - _eta;
    float s2 = _sig * _sig;
    return expf(-t*t/(2.0f*s2)) / sqrtf(2.0f*M_PI*s2);
}

// Gauss random number cumulative distribution function
float randnf_cdf(float _x,
                 float _eta,
                 float _sig)
{
    return 0.5 + 0.5*erff( M_SQRT1_2*(_x-_eta)/_sig );
}

