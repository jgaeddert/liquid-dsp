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
// Complex fixed-point / floating-point conversion
//

#include "liquidfpm.internal.h"

// convert array of fixed-point values to floating point
//  _dst    :   destination address
//  _src    :   source address
//  _n      :   number of elements
void Q(_memmove_fixed_to_float)(float *      _x,
                                Q(_t) *      _y,
                                unsigned int _n)
{
    // slow method; just run individual conversion on each sample
    unsigned int i;
    for (i=0; i<_n; i++)
        _x[i] = Q(_fixed_to_float)(_y[i]);
}

// convert array of floating-point values to fixed point
//  _dst    :   destination address
//  _src    :   source address
//  _n      :   number of elements
void Q(_memmove_float_to_fixed)(Q(_t) *      _x,
                                float *      _y,
                                unsigned int _n)
{
    // slow method; just run individual conversion on each sample
    unsigned int i;
    for (i=0; i<_n; i++)
        _x[i] = Q(_float_to_fixed)(_y[i]);
}
