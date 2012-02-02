/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
 *                                Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// inverse approximation
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"
#include "liquidfpm.internal.h"

#define DEBUG_INV_NEWTON 0

// computes x = inv(d) = 1/d using iterative Newtonian method:
//   x[k+1] = x[k] + x[k]*(1 - d*x[k])
Q(_t) Q(_inv_newton)(Q(_t) _x,
                     unsigned int _precision)
{
    int negate = (_x < 0);
    _x = Q(_abs)(_x);

    // initial guess: x0 = 2^-floor(log2(|_x|))
    int b = liquid_msb_index(_x) - 1;          // base index
    int s = (int)Q(_fracbits) - b - 1;  // shift amount
    Q(_t) x0 = s>0 ? Q(_one)<<s : Q(_one)>>(-s);

    Q(_t) x1=0;
    Q(_t) y0=0;
    Q(_t) d=_x;
    Q(_t) dx0=0;
    Q(_t) x0y0=0;

    unsigned int i;
#if DEBUG_INV_NEWTON
    printf("   x : %12.8f\n", Q(_fixed_to_float)(_x));
    printf("  x0 : %12.8f\n", Q(_fixed_to_float)(x0));
    printf("   n :         d*x0           y0        x0*y0           x1\n");
#endif
    for (i=0; i<_precision; i++) {
#if DEBUG_INV_NEWTON
        printf("%4u : %12.8f %12.8f %12.8f %12.8f\n", i,
                                 Q(_fixed_to_float)(dx0),
                                 Q(_fixed_to_float)(y0),
                                 Q(_fixed_to_float)(x0y0),
                                 Q(_fixed_to_float)(x1));
#endif
        dx0  = Q(_mul)(d,x0);
        y0   = Q(_one) - dx0;
        x0y0 = Q(_mul)(x0,y0);
        x1   = x0 + x0y0;

        // break if multiplier is zero: all further
        // iterations will result in the same value
        // for x1
        if (!y0 || !x0y0) break;

        x0 = x1;
    }

    return negate ? -x1 : x1;
}

