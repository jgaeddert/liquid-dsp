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

