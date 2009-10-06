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
//
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define randf_inline() ((float) rand() / (float) RAND_MAX)

float randf() {
    return randf_inline();
}

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

// Weibull
float rand_weibullf(float _alpha, float _beta, float _gamma)
{
#ifdef LIQUID_VALIDATE_INPUT
    // validate input
    if (_alpha <= 0) {
        printf("error: rand_weibullf(), alpha must be greater than zero\n");
        return 0.0f;
    } else if (_beta <= 0) {
        printf("error: rand_weibullf(), beta must be greater than zero\n");
        return 0.0f;
    }
#endif

    return _gamma + powf(-_beta/_alpha*logf(randf()), 1/_beta);
}

// Rice-K
float rand_ricekf(float _K, float _omega)
{
    float complex x, y;
    float s = sqrtf((_omega*_K)/(_K+1));
    float sig = sqrtf(0.5f*_omega/(_K+1));
    crandnf(&x);
    y = _Complex_I*( crealf(x)*sig + s ) +
                   ( cimagf(x)*sig     );
    return cabsf(y);
}


