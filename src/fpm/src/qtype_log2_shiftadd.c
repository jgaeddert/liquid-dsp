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
// log2 shift|add
//
// Algorithm from fxtbook, ch. 31
// 

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"
#include "liquidfpm.internal.h"

#define DEBUG_LOG2_SHIFTADD     0

// break infinite loop condition (only an issue for debugging and
// limit testing)
#define FAILSAFE_LOG2_SHIFTADD  1

// natural logarithm
//    ln(x) = log2(x) / log2(e)
//          = log2(x) * log(2)
Q(_t) Q(_log_shiftadd)(Q(_t) _x, unsigned int _n)
{
    return Q(_mul)(Q(_log2_shiftadd)(_x,_n), Q(_ln2));
}

// base-10 logarithm
//    log10(x) = log2(x) / log2(10)
//             = log2(x) * log10(2)
Q(_t) Q(_log10_shiftadd)(Q(_t) _x, unsigned int _n)
{
    return Q(_mul)(Q(_log2_shiftadd)(_x,_n), Q(_log10_2));
}

// log(1+x)
Q(_t) Q(_log1p_shiftadd)(Q(_t) _x, unsigned int _n)
{
    return Q(_log_shiftadd)(_x + Q(_one), _n);
}

// Computes y = log2(x) by pre-shifting the input _x such
// that _x is in [1,2), and then performing the iterative
// shift|add operation on the result.
Q(_t) Q(_log2_shiftadd)(Q(_t) _x,
                        unsigned int _n)
{
    // base index
    int b = liquid_msb_index(_x) - 1;

    // compute shift amount
    int s = (int)Q(_fracbits) - b;

    // pre-shift input (left : s>0, right : s<0)
    Q(_t) x_hat = s>0 ? _x<<s : _x>>(-s);

#if DEBUG_LOG2_SHIFTADD
    printf("x : %12.8f >> %12.8f\n", Q(_fixed_to_float)(_x), 
                                     Q(_fixed_to_float)(x_hat));
    printf("s : %12.8f\n", Q(_fixed_to_float)(-s<<Q(_fracbits)));
#endif

    // compute the fractional portion using the iterative
    // shift|add algorithm.
    Q(_t) yfrac = Q(_log2_shiftadd_base)(x_hat,_n);

    // compute the integer portion: simply the integer
    // representation of the base index of the original
    // input value _x
    Q(_t) yint = (-s) << (Q(_fracbits));

    return yint + yfrac;
}

// computes y=log2(x) where x >= 1
//
// For values of x < 1, it is necessary to pre-shift x by its
// most-significant bit.  The algorithm will NOT converge for
// x < 1, nor will it validate that the input is in this range.
Q(_t) Q(_log2_shiftadd_base)(Q(_t) _x,
                             unsigned int _n)
{
    Q(_t) tn = 0;
    Q(_t) en = Q(_one);
    Q(_t) un = 0;
    Q(_t) vn = Q(_one);
    Q(_t) x = _x;
    int dn;
    unsigned int n = _n;
    if (n>Q(_log2_shiftadd_nmax))
        n = Q(_log2_shiftadd_nmax);
    unsigned int i;
#if DEBUG_LOG2_SHIFTADD
    printf("   n           un           tn           en           An\n");
    printf("init            - %12.8f %12.8f %12.8f\n",
            Q(_fixed_to_float)(tn),
            Q(_fixed_to_float)(en),
            Q(_fixed_to_float)(Q(_log2_shiftadd_Ak_tab)[0]));
#endif

#if FAILSAFE_LOG2_SHIFTADD
    unsigned int failsafe = 100;
#endif

    for (i=1; i<n; i++) {
        vn >>= 1;
        while (1) {
            //un = en + Q(_mul)(en,vn);
            un = en;
            un += en>>i;
            dn = (un <= x);
#if DEBUG_LOG2_SHIFTADD
            printf("%4u %12.8f %12.8f %12.8f %12.4e\n",
                    i,
                    Q(_fixed_to_float)(un),
                    Q(_fixed_to_float)(tn),
                    Q(_fixed_to_float)(en),
                    Q(_fixed_to_float)(Q(_log2_shiftadd_Ak_tab)[i]));
#endif
            if (dn == 0) break;
            tn += Q(_log2_shiftadd_Ak_tab)[i];
            en = un;

#if FAILSAFE_LOG2_SHIFTADD
            // failsafe condition
            failsafe--;
            if (failsafe == 0) {
                fprintf(stderr,"warning: qtype_log2_shiftadd_base() failed to converge\n");
                return 0;
            }
#endif
        }
    }

    return tn;
}


