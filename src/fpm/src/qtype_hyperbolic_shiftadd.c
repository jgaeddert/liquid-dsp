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
// hyperbolic functions using shift|add method
// 

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

void Q(_sinhcosh_shiftadd)(Q(_t)        _x,
                           Q(_t) *      _sinh,
                           Q(_t) *      _cosh,
                           unsigned int _precision)
{
    // compute exp{_x}, exp{-x}
    Q(_t) expxp = Q(_exp_shiftadd)( _x, _precision);
    Q(_t) expxm = Q(_exp_shiftadd)(-_x, _precision);

    //
    *_sinh = (expxp - expxm) >> 1;
    *_cosh = (expxp + expxm) >> 1;
}

// compute hyperbolic sine
Q(_t) Q(_sinh_shiftadd)(Q(_t) _x, unsigned int _precision)
{
    // compute exp{_x}, exp{-x}
    Q(_t) expxp = Q(_exp_shiftadd)( _x, _precision);
    Q(_t) expxm = Q(_exp_shiftadd)(-_x, _precision);

    //
    return (expxp - expxm) >> 1;
}

// compute hyperbolic cosine
Q(_t) Q(_cosh_shiftadd)(Q(_t) _x, unsigned int _precision)
{
    // compute exp{_x}, exp{-x}
    Q(_t) expxp = Q(_exp_shiftadd)( _x, _precision);
    Q(_t) expxm = Q(_exp_shiftadd)(-_x, _precision);

    //
    return (expxp + expxm) >> 1;
}

// compute hyperbolic tangent
Q(_t) Q(_tanh_shiftadd)(Q(_t)        _x,
                        unsigned int _precision)
{
    // compute exp{2x}
    Q(_t) exp2x = Q(_exp_shiftadd)(_x<<1, _precision);

    // tanh = ( exp{2x} - 1 ) / ( exp{2x} + 1 )
    return Q(_div_inline)( exp2x - Q(_one), exp2x + Q(_one) );
}

