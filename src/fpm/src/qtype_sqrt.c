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
// square root approximation
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define DEBUG_SQRT_NEWTON 0

// use iterative process via Newton's method
Q(_t) Q(_sqrt_newton)( Q(_t) _x, unsigned int _n)
{
    if (_x < 0) {
        fprintf(stderr,"warning: qxx_sqrt_newton(), x must be >= 0");
        return 0;
    }

    if (_x == 0)
        return 0;

    // initial guess: x0 = _x
    //Q(_t) x0 = _x;

    // initial guess: set x0 to 2^floor(msb_index/2)
    // NOTE: we need to compensate for the decimal position of q32
    Q(_t) x0 = 1 << (((liquid_msb_index(_x)-Q(_fracbits))/2)+Q(_fracbits));

    Q(_t) x1=0;

    unsigned int i;
    for (i=0; i<_n; i++) {
#if DEBUG_SQRT_NEWTON
        printf("%4u : %18.14f\n", i, Q(_fixed_to_float)(x0));
#endif
        x1 = (x0 + Q(_div)(_x,x0)) / 2;

        // break if change in this iteration is negligible
        if (abs(x1-x0) < 1)
            break;

        x0 = x1;
    }

    //return x1 << (Q(_fracbits)/2);
    return Q(_abs)(x1);
}

