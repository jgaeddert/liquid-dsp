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
// specifc fixed-point logarithmic/exponential algorithms
//  qtype_exp
//  qtype_exp2
//  qtype_expm1
//  qtype_log
//  qtype_log2
//  qtype_log10
//  qtype_log1p
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define precision (Q(_bits))

Q(_t) Q(_exp)(Q(_t) _x)
{
    return Q(_exp_shiftadd)(_x, precision);
}

Q(_t) Q(_exp2)(Q(_t) _x)
{
    return Q(_exp2_shiftadd)(_x, precision);
}

Q(_t) Q(_expm1)(Q(_t) _x)
{
    return Q(_expm1_shiftadd)(_x, precision);
}

Q(_t) Q(_log)(Q(_t) _x)
{
    return Q(_log_shiftadd)(_x, precision);
}

Q(_t) Q(_log2)(Q(_t) _x)
{
    return Q(_log2_shiftadd)(_x, precision);
}

Q(_t) Q(_log10)(Q(_t) _x)
{
    return Q(_log10_shiftadd)(_x, precision);
}

Q(_t) Q(_log1p)(Q(_t) _x)
{
    return Q(_log1p_shiftadd)(_x, precision);
}

