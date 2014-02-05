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
// Useful mathematical formulae (trig)
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

#if 0
#define LIQUID_SINF_POLYORD (4)
static float liquid_sinf_poly[LIQUID_SINF_POLYORD] = {
  -0.113791698328739f,
  -0.069825754521815f,
   1.026821728423492f,
   0.000000000000000f
};
#endif

void liquid_sincosf(float _x,
                    float * _sinf,
                    float * _cosf)
{
    * _sinf = sinf(_x);
    * _cosf = cosf(_x);
}

float liquid_sinf(float _x)
{
    float s, c;
    liquid_sincosf(_x,&s,&c);
    return s;
}

float liquid_cosf(float _x)
{
    float s, c;
    liquid_sincosf(_x,&s,&c);
    return c;
}

float liquid_tanf(float _x)
{
    float s, c;
    liquid_sincosf(_x,&s,&c);
    return s/c;
}

float liquid_expf(float _x)
{
    return expf(_x);
}

float liquid_logf(float _x)
{
    return logf(_x);
}

