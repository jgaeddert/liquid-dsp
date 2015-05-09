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
// log2 implementations
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"
#include "liquidfpm.internal.h"

#define LIQUIDFPM_DEBUG_LOG2_FRAC 0

// natural logarithm
//    ln(x) = log2(x) / log2(e)
//          = log2(x) * log(2)
Q(_t) Q(_log_frac)(Q(_t) _x)
{
    return Q(_mul)(Q(_log2_frac)(_x), Q(_ln2));
}

// base-10 logarithm
//    log10(x) = log2(x) / log2(10)
//             = log2(x) * log10(2)
Q(_t) Q(_log10_frac)(Q(_t) _x)
{
    return Q(_mul)(Q(_log2_frac)(_x), Q(_log10_2));
}

// log(1+x)
Q(_t) Q(_log1p_frac)(Q(_t) _x)
{
    return Q(_log_frac)(_x + Q(_one));
}

// log2 using fractional table look-up; fast but not especially accurate
Q(_t) Q(_log2_frac)(Q(_t) _x)
{
    if (_x <= 0) {
        fprintf(stderr,"error: qtype_log2(), x must be > 0");
        exit(1);
    }

    // base index
    int b = liquid_msb_index(_x) - 1;

    // compute 8-bit fractional portion
    Q(_t) f = (b < 8) ? (_x<<(8-b)) : (_x>>(b-8));

    // apply 8-bit mask
    f &= 0x00ff;

    // normalize by decimal position
    b -= Q(_fracbits);

    Q(_t) frac = Q(_log2_frac_gentab)[f];
#if LIQUIDFPM_DEBUG_LOG2_FRAC
    printf("    log2(%f) = %d + %12.10f\n", Q(_fixed_to_float)(_x), b, Q(_fixed_to_float)(frac));
#endif

    if (b >= 0 ) {
        // resulting log will be positive
        return (b << Q(_fracbits)) + frac;
    } else {
        // resulting log will be negative
        return -(abs(b) << Q(_fracbits)) + frac;
    }

    return 0;
}

