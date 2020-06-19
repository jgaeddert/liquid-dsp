/*
 * Copyright (c) 2007 - 2020 Joseph Gaeddert
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
// cube root approximations
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define DEBUG_CBRT_NEWTON 0

// use iterative process via Newton's method
Q(_t) Q(_cbrt_newton)( Q(_t) _x, unsigned int _n)
{
#if DEBUG_CBRT_NEWTON
    printf("cbrt_newton(%12.8f, %4u):\n", Q(_fixed_to_float)(_x), _n);
#endif
    if (_x < 0) {
        return -Q(_cbrt_newton)(-_x, _n);
    } else if (_x == 0) {
        return 0;
    }

#if 1
    // initial guess: x0 = _x
    Q(_t) x0 = _x;
#else
    // initial guess: set x0 to 2^floor(msb_index/2)
    // NOTE: we need to compensate for the decimal position of q32
    Q(_t) x0 = 1 << (((liquid_msb_index(_x)-Q(_fracbits))/2)+Q(_fracbits));
#endif

    Q(_t) x1 = _x;

    unsigned int i;
    for (i=0; i<_n; i++) {
        // x1 = (1/3) * ( _x/x0^2 + 2*x0 )

        // compute x0^2
        Q(_t) x0x0 = Q(_mul)(x0,x0);
#if DEBUG_CBRT_NEWTON
        printf(" %4u : x0=%12.8f, x0^2=%12.8f\n", i, Q(_fixed_to_float)(x0), Q(_fixed_to_float)(x0x0));
#endif
        if (x0x0 == 0)
            break;

        // update estimate
        x1 = ( Q(_div)(_x, x0x0) + (x0<<1) ) / 3;

        // break if change in this iteration is negligible
        if (abs(x1-x0) < 1)
            break;

        x0 = x1;
    }

    //return x1 << (Q(_fracbits)/2);
    return Q(_abs)(x1);
}

