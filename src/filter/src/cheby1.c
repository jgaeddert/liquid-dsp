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
// Chebeshev type-I filter design
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.internal.h"

void cheby1f(unsigned int _n, float _ep, float * _b, float * _a)
{
    // poles
    float complex s[_n];

    float e = _ep;
    float e2 = e*e;
    float nf = (float) _n;

    float b = 0.5*powf( sqrtf(1+1/e2) + 1/e,  1/nf) +
              0.5*powf( sqrtf(1-1/e2) + 1/e, -1/nf);

    float a = 0.5*powf( sqrtf(1+1/e2) + 1/e,  1/nf) -
              0.5*powf( sqrtf(1-1/e2) + 1/e, -1/nf);

    printf("ep : %12.8f\n", e);
    printf("b  : %12.8f\n", b);
    printf("a  : %12.8f\n", a);

}
