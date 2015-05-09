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
// specifc fixed-point 'power' algorithms
//  qtype_sqrt
//  qtype_cbrt
//  qtype_hypot
//  qtype_pow
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define precision (Q(_bits))

Q(_t) Q(_sqrt)(Q(_t) _x)
{
    return Q(_sqrt_newton)(_x, precision);
}

Q(_t) Q(_cbrt)(Q(_t) _x)
{
    fprintf(stderr,"warning: qtype_cbrt(), not yet implemented\n");
    return 0;
}

Q(_t) Q(_hypot)(Q(_t) _x,
                Q(_t) _y)
{
    fprintf(stderr,"warning: qtype_hypot(), not yet implemented\n");
    return 0;
}

Q(_t) Q(_pow)(Q(_t) _b,
              Q(_t) _x)
{
    return Q(_pow_shiftadd)(_b, _x, precision);
}

