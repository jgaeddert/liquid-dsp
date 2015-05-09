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

// convert array of fixed-point values to floating point
//  _dst    :   destination address
//  _src    :   source address
//  _n      :   number of elements
void CQ(_memmove_fixed_to_float)(float complex * _x,
                                 CQ(_t) *        _y,
                                 unsigned int    _n)
{
    // slow method; just run individual conversion on each sample
    unsigned int i;
    for (i=0; i<_n; i++)
        _x[i] = CQ(_fixed_to_float)(_y[i]);
}

// convert array of floating-point values to fixed point
//  _dst    :   destination address
//  _src    :   source address
//  _n      :   number of elements
void CQ(_memmove_float_to_fixed)(CQ(_t) *        _x,
                                 float complex * _y,
                                 unsigned int    _n)
{
    // slow method; just run individual conversion on each sample
    unsigned int i;
    for (i=0; i<_n; i++)
        _x[i] = CQ(_float_to_fixed)(_y[i]);
}

