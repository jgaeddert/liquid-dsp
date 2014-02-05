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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

#define LIQUID_VALIDATE_INPUT

float compress_mulaw(float _x, float _mu)
{
#ifdef LIQUID_VALIDATE_INPUT
    if ( _mu <= 0.0f ) {
        printf("error: compress_mulaw(), mu out of range\n");
        exit(1);
    }
#endif
    float y = logf(1 + _mu*fabsf(_x)) / logf(1 + _mu);
    return copysignf(y, _x);
}

float expand_mulaw(float _y, float _mu)
{
#ifdef LIQUID_VALIDATE_INPUT
    if ( _mu <= 0.0f ) {
        printf("error: expand_mulaw(), mu out of range\n");
        exit(1);
    }
#endif
    float x = (1/_mu)*( powf(1+_mu,fabsf(_y)) - 1);
    return copysign(x, _y);
}

void compress_cf_mulaw(float complex _x, float _mu, float complex * _y)
{
#ifdef LIQUID_VALIDATE_INPUT
    if ( _mu <= 0.0f ) {
        printf("error: compress_mulaw(), mu out of range\n");
        exit(1);
    }
#endif
    *_y = cexpf(_Complex_I*cargf(_x)) * logf(1 + _mu*cabsf(_x)) / logf(1 + _mu);
}

void expand_cf_mulaw(float complex _y, float _mu, float complex * _x)
{
#ifdef LIQUID_VALIDATE_INPUT
    if ( _mu <= 0.0f ) {
        printf("error: expand_mulaw(), mu out of range\n");
        exit(1);
    }
#endif
    *_x = cexpf(_Complex_I*cargf(_y)) * (1/_mu)*( powf(1+_mu,cabsf(_y)) - 1);
}

/*
float compress_alaw(float _x, float _a)
{

}

float expand_alaw(float _x, float _a)
{

}
*/

