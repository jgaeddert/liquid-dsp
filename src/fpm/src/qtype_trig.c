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
// specifc fixed-point trigonometric algorithms
//  qtype_sin
//  qtype_cos
//  qtype_tan
//  qtype_acos
//  qtype_asin
//  qtype_atan
//  qtype_atan2
//  qtype_sincos
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define precision (Q(_bits))

Q(_t) Q(_sin)(Q(_t) _theta)
{
    return Q(_sin_cordic)(_theta, precision);
}

Q(_t) Q(_cos)(Q(_t) _theta)
{
    return Q(_cos_cordic)(_theta, precision);
}

Q(_t) Q(_tan)(Q(_t) _theta)
{
    Q(_t) sin;
    Q(_t) cos;
    Q(_sincos_cordic)(_theta, &sin, &cos, precision);

    // TODO : check this method
    return Q(_div)(sin, cos);
}

Q(_t) Q(_acos)(Q(_t) _x)
{
    fprintf(stderr,"warning: qtype_acos() not implemented\n");
    return 0;
}

Q(_t) Q(_asin)(Q(_t) _x)
{
    fprintf(stderr,"warning: qtype_asin() not implemented\n");
    return 0;
}

Q(_t) Q(_atan)(Q(_t) _y)
{
    Q(_t) r;
    Q(_t) theta;
    Q(_atan2_cordic)(_y, Q(_one), &r, &theta, precision);
    return theta;
}

Q(_t) Q(_atan2)(Q(_t) _y,
                Q(_t) _x)
{
    Q(_t) r;
    Q(_t) theta;
    Q(_atan2_cordic)(_y, _x, &r, &theta, precision);
    return theta;
}

void Q(_sincos)(Q(_t)   _theta,
                Q(_t) * _sin,
                Q(_t) * _cos)
{
    Q(_sincos_cordic)(_theta, _sin, _cos, precision);
}

