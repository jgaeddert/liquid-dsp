/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
// specifc fixed-point 'power' algorithms
//  qtype_sqrt
//  qtype_cbrt
//  qtype_hypot
//  qtype_pow
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define precision (Q(_bits))

Q(_t) Q(_sqrt)(Q(_t) _x)
{
    return Q(_sqrt_newton)(_x, precision);
}

Q(_t) Q(_cbrt)(Q(_t) _x)
{
    fprintf(stderr,"warning: qtype_cbrt(), not yet implemented\n");
    return 0;
}

Q(_t) Q(_hypot)(Q(_t) _x,
                Q(_t) _y)
{
    fprintf(stderr,"warning: qtype_hypot(), not yet implemented\n");
    return 0;
}

Q(_t) Q(_pow)(Q(_t) _b,
              Q(_t) _x)
{
    return Q(_pow_shiftadd)(_b, _x, precision);
}

