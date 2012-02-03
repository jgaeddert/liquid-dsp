/*
 * Copyright (c) 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2008, 2009, 2010 Virginia Polytechnic
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
// atan piecewise poly
// 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "liquidfpm.internal.h"

#define DEBUG_ATAN_FRAC 0

Q(_t) Q(_atan2_frac)( Q(_t) _y, Q(_t) _x )
{
#if DEBUG_ATAN_FRAC
    printf("atan2_frac(%12.8f,%12.8f):\n", Q(_fixed_to_float)(_y), Q(_fixed_to_float)(_x));
#endif
    // check for extreme conditions
    if (_y==0)      return (_x >= 0) ? 0           :  Q(_pi);
    else if (_x==0) return (_y >= 0) ? Q(_pi_by_2) : -Q(_pi_by_2);

    // For all practical purposes we are observing r = y/x so the decimal
    // place in fixed point math can be ignored and we can treat y and x
    // as integers so long as we don't lose resolution

    // negate calculated angle if _y < 0
    int negate = (_y < 0);

    // subtract calculated angle from pi if _x < 0
    int sub_from_pi = (_x < 0);

    // subtract calculated angle from pi/2 if abs(y/x) < 1, or invert
    // y/x to by x/y so that log2 of ratio is positive
    int invert = 0;

    // take absolute value as we are only interested in absolute ratio
    // at this point.
    _y = abs(_y);
    _x = abs(_x);

    // compare x, y; place larger value in y, smaller in x, thus ratio
    // r = y/x is always greater than or equal to 1; keeps log2(y/x)
    // positive
    if (_y < _x) {
        // swap _x and _y values
        Q(_t) tmp = _x;
        _x = _y;
        _y = tmp;
        invert = 1;
    }

    // Ultimately we want to calculate log2(abs(y/x)).  The following
    // block of code achieves this with simple bit shifts, comparisons
    // and a 32-value lookup table.

    // TODO : explain what is going on here to compute log2(abs(y/x))

    // Extract location of MSB for _y and _x; keep values in register as
    // operations on them will be computed very quickly

    assert(_x >   0);
    assert(_y >= _x);

    // 
    // compute logs
    //

    // base indices
    int bx = liquid_msb_index(_x) - 1;
    int by = liquid_msb_index(_y) - 1;

    // compute 8-bit fractional portions
    Q(_t) fx = (bx < 8) ? (_x<<(8-bx)) : (_x>>(bx-8));
    Q(_t) fy = (by < 8) ? (_y<<(8-by)) : (_y>>(by-8));

    // apply 8-bit mask (resolution) and extract fractional value from
    // look-up table
    fx = Q(_log2_frac_gentab)[fx & 0x00ff];
    fy = Q(_log2_frac_gentab)[fy & 0x00ff];

    // compute log2(y) - log2(x) to 8 significant digits
    // (resolution of atan2 look-up table)
    int   b = by - bx;
    Q(_t) f = fy - fx;
    Q(_t) logdiff;
    if (b >= 0 ) {
        // resulting log will be positive
        logdiff = (b << Q(_fracbits)) + f;
    } else {
        // resulting log will be negative
        logdiff = -(abs(b) << Q(_fracbits)) + f;
    }

    // ensure fractional portion is in [0,1)
    if (f < 0) {
        f += Q(_one);
        b--;
    }

#if DEBUG_ATAN_FRAC
    float yf = Q(_fixed_to_float)(_y);
    float xf = Q(_fixed_to_float)(_x);
    printf("    log2(%12.8f) = %3d + %12.10f (y)\n", Q(_fixed_to_float)(_y), by - Q(_fracbits), Q(_fixed_to_float)(fy));
    printf("    log2(%12.8f) = %3d + %12.10f (x)\n", Q(_fixed_to_float)(_x), bx - Q(_fracbits), Q(_fixed_to_float)(fx));
    printf("    logdiff            = %3d + %12.10f (%12.8f)\n", b, Q(_fixed_to_float)(f), log2f(yf/xf));
#endif

    // look-up table can be indexed from 'b' and 'f' relatively easily
    assert(b >= 0);
    assert(f >= 0);

    // check boundary conditions...

    if ( b >= 8 ) {
        // use high ratio approximation
#if DEBUG_ATAN_FRAC
        printf("  high ratio approximation\n");
#endif

        // four conditions exist
        if (invert) return (sub_from_pi ?  Q(_pi)      : 0          );
        else        return (negate      ? -Q(_pi_by_2) : Q(_pi_by_2));
    }

    assert(Q(_fracbits) >= 3);
    unsigned int index = (b << 5) | ( (f >> (Q(_fracbits)-3)) & 0x1f );
    assert(index < 256);

#if DEBUG_ATAN_FRAC
    printf("    index = %4u\n", index);
#endif

    // look-up table
    Q(_t) phi = Q(_atan2tab)[index];

    // invert if necessary
    if (invert) phi = Q(_pi_by_2) - phi;

    // if in quadrants 3, 4 subtract from pi
    if (sub_from_pi) phi = Q(_pi) - phi;

    // negate if necessary
    if (negate) phi = -phi;

    return phi;
}

