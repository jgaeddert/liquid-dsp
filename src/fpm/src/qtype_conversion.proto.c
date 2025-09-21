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
