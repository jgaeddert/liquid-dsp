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
        fprintf(stderr,"error: randnakmf(), m cannot be less than 0.5\n");
        exit(1);
    } else if (_omega <= 0.0f) {
        fprintf(stderr,"error: randnakmf(), omega must be greater than zero\n");
        exit(1);
    }

    // generate Gamma random variable
    float alpha = _m;
    float beta  = _omega / _m;
    float x = randgammaf(alpha,beta);

    // sqrt(x) ~ Nakagami(m,omega)
    return sqrtf(x);
}

// Nakagami-m distribution probability distribution function
// Nakagami-m
//  f(x) = (2/Gamma(m)) (m/omega)^m x^(2m-1) exp{-(m/omega)x^2}
// where
//      m       : shape parameter, m >= 0.5
//      omega   : spread parameter, omega > 0
//      Gamma(z): regular complete gamma function
//      x >= 0
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

    float t0 = liquid_lngammaf(_m);
    float t1 = _m * logf(_m/_omega);
    float t2 = (2*_m - 1.0f) * logf(_x);
    float t3 = -(_m/_omega)*_x*_x;

    return 2.0f * expf( -t0 + t1 + t2 + t3 );
}

// Nakagami-m distribution cumulative distribution function
//  F(x) = gamma(m, x^2 m / omega) / Gamma(m)
//  where
//      gamma(z,a) = lower incomplete gamma function
//      Gamma(z)   = regular gamma function
//
float randnakmf_cdf(float _x,
                    float _m,
                    float _omega)
{
    // validate input
    if (_m < 0.5f) {
        fprintf(stderr,"error: randnakmf_cdf(), m cannot be less than 0.5\n");
        exit(1);
    } else if (_omega <= 0.0f) {
        fprintf(stderr,"error: randnakmf_cdf(), omega must be greater than zero\n");
        exit(1);
    }

    if (_x <= 0.0f)
        return 0.0f;

    float t0 = liquid_lnlowergammaf(_m, _x*_x*_m/_omega);
    float t1 = liquid_lngammaf(_m);
    return expf( t0 - t1 );
}


