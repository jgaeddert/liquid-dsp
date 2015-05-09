/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

