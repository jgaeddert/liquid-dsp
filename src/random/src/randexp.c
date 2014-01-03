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
// Exponential distribution
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// Exponential
float randexpf(float _lambda)
{
    // validate input
    if (_lambda <= 0) {
        fprintf(stderr,"error: randexpf(), lambda must be greater than zero\n");
        return 0.0f;
    }

    // compute a non-zero uniform random variable in (0,1]
    float u;
    do {
        u = randf();
    } while (u==0.0f);

    // perform variable transformation
    return -logf( u ) / _lambda;
}

// Exponential random number probability distribution function
float randexpf_pdf(float _x,
                   float _lambda)
{
    // validate input
    if (_lambda <= 0) {
        fprintf(stderr,"error: randexpf(), lambda must be greater than zero\n");
        return 0.0f;
    }

    if (_x < 0.0f)
        return 0.0f;

    return _lambda * expf(-_lambda*_x);
}

// Exponential random number cumulative distribution function
float randexpf_cdf(float _x,
                   float _lambda)
{
    // validate input
    if (_lambda <= 0) {
        fprintf(stderr,"error: randexpf(), lambda must be greater than zero\n");
        return 0.0f;
    }

    if (_x < 0.0f)
        return 0.0f;

    return 1.0f - expf(-_lambda*_x);
}

