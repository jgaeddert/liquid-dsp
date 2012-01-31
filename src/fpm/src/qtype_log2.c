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
// log2 implementations
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"
#include "liquidfpm.internal.h"

#define LIQUIDFPM_DEBUG_LOG2_FRAC 1

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

