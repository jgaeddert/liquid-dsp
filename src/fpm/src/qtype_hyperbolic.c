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
// specifc fixed-point hyperbolic trigonometric algorithms
//  qtype_cosh
//  qtype_sinh
//  qtype_tanh
//  qtype_acosh
//  qtype_asinh
//  qtype_atanh
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define precision (Q(_bits))

Q(_t) Q(_cosh)(Q(_t) _x)
{
    return Q(_cosh_shiftadd)(_x, precision);
}

Q(_t) Q(_sinh)(Q(_t) _x)
{
    return Q(_sinh_shiftadd)(_x, precision);
}

Q(_t) Q(_tanh)(Q(_t) _x)
{
    return Q(_tanh_shiftadd)(_x, precision);
}

Q(_t) Q(_acosh)(Q(_t) _x)
{
    fprintf(stderr,"warning: qtype_acosh(), not yet implemented\n");
    return 0;
}

Q(_t) Q(_asinh)(Q(_t) _x)
{
    fprintf(stderr,"warning: qtype_asinh(), not yet implemented\n");
    return 0;
}

Q(_t) Q(_atanh)(Q(_t) _x)
{
    fprintf(stderr,"warning: qtype_atanh(), not yet implemented\n");
    return 0;
}

