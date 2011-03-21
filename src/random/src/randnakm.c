/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// Nakagami-m distribution
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

float randnakmf(float _m,
                float _omega)
{
    // validate input
    if (_m < 0.5f) {
        fprintf(stderr,"error: randnakmf_pdf(), m cannot be less than 0.5\n");
        exit(1);
    } else if (_omega <= 0.0f) {
        fprintf(stderr,"error: randnakmf_pdf(), omega must be greater than zero\n");
        exit(1);
    }

    // generate Gamma random variable
    float alpha = _m;
    float beta  = _omega / _m;
    float x = randgammaf(alpha,beta);

    // sqrt(x) ~ Nakagami(m,omega)
    return sqrtf(x);
}

float randnakmf_pdf(float _x,
                    float _m,
                    float _omega)
{
    // validate input
    if (_m < 0.5f) {
        fprintf(stderr,"error: randnakmf_pdf(), m cannot be less than 0.5\n");
        exit(1);
    } else if (_omega <= 0.0f) {
        fprintf(stderr,"error: randnakmf_pdf(), omega must be greater than zero\n");
        exit(1);
    }

    if (_x <= 0.0f)
        return 0.0f;

    float t0 = liquid_gammaf(_m);
    float t1 = powf( _m/_omega, _m );
    float t2 = powf( _x, 2*_m - 1.0f );
    float t3 = expf( -(_m/_omega)*_x*_x );

    return 2.0f * t1 * t2 * t3 / t0;
}

// Nakagami-m distribution cumulative distribution function
float randnakmf_cdf(float _x,
                    float _m,
                    float _omega)
{
    return 0.0f;
}


