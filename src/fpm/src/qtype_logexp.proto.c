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

