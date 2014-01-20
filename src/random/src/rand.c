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
// Uniform random number generator definitions
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// uniform random number generator
float randf() {
    return randf_inline();
}

// uniform random number probability distribution function
float randf_pdf(float _x)
{
    return (_x < 0.0f || _x > 1.0f) ? 0.0f : 1.0f;
}

// uniform random number cumulative distribution function
float randf_cdf(float _x)
{
    if (_x < 0.0f)
        return 0.0f;
    else if (_x > 1.0f)
        return 1.0f;

    return _x;
}

