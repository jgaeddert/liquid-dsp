/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
 *                                Institute & State University
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
// Complex fixed-point / floating-point conversion
//

#include "liquidfpm.internal.h"

// convert to floating-point precision
float complex CQ(_fixed_to_float)(CQ(_t) _x)
{
    return Q(_fixed_to_float)(_x.real) + 
           Q(_fixed_to_float)(_x.imag) * _Complex_I;
}

// convert to fixed-point precision
CQ(_t) CQ(_float_to_fixed)(float complex _x)
{
    CQ(_t) y;
    y.real = Q(_float_to_fixed)(crealf(_x));
    y.imag = Q(_float_to_fixed)(cimagf(_x));
    return y;
}


