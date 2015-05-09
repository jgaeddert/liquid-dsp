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
// qtype_arithmetic.port.c
//
// Portable qtype_t fixed-point arithmetic implementations
//

#include "liquidfpm.internal.h"

// multiplication
Q(_t) Q(_mul)(Q(_t) _x, Q(_t) _y)
{
#if 0
    // This method loses significant precision by pre-shifting
    // out the least significant bits of each argument

    unsigned int s0 = Q(_fracbits) >> 1;
    unsigned int s1 = Q(_fracbits) - s0;

    return (_x >> s0) * (_y >> s1);
#else
    // compute multiplication using high-order accumulator,
    // then post-shift the result and return

    Q(_at) z = (Q(_at))(_x) * (Q(_at))(_y);

    return (Q(_t)) (z >> Q(_fracbits));
#endif
}

// division
Q(_t) Q(_div)(Q(_t) _x, Q(_t) _y)
{
#if 0
    // define data types of double precision
    // (e.g. int64_t for 32-bit fixed-point)
    Q(_at) a = _x;
    Q(_at) b = _y;

    // pre-shift numerator by qtype_fracbits
    a <<= Q(_fracbits);

    return (Q(_t))(a / b);
#else
    // compute division by pre-shifting numerator using
    // high-order accumulator, then dividing by denominator

    Q(_at) z = ((Q(_at))(_x)<<Q(_fracbits)) / _y;

    return (Q(_t)) (z);
#endif
}

// ceiling
Q(_t) Q(_ceil)(Q(_t) _x)
{
    return (_x == 0) ? 0 : Q(_floor)(_x) + Q(_one);
}

// floor
Q(_t) Q(_floor)(Q(_t) _x)
{
    return Q(_intpart)(_x) << Q(_fracbits);
}

