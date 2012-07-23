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
// hyperbolic sin/cos CORDIC
// 

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define DEBUG_SINHCOSH_CORDIC   0

// compute hyperbolic trig functions
// NOTE: the CORDIC algorithm is only valid for a limited range:
//   |_theta| < 1.118173015526...
void Q(_sinhcosh_cordic)(Q(_t)        _theta,
                         Q(_t) *      _sinh,
                         Q(_t) *      _cosh,
                         unsigned int _n)
{
#if DEBUG_SINHCOSH_CORDIC
    printf("  theta : %12.8f\n", Q(_fixed_to_float)(_theta));
#endif

    // TODO : validate that |_theta| < 1.118173015526...

    Q(_t) x = Q(_sinhcosh_cordic_kp_inv); // TODO : initialize with cordic_Kpinv_tab[_n-1];
    Q(_t) y = 0;
    Q(_t) z = _theta;
    Q(_t) d,tx,ty,tz;
    unsigned int k;
    unsigned int n = _n > Q(_bits) ? Q(_bits) : _n;
    unsigned int i = 4;

#if DEBUG_SINHCOSH_CORDIC
    printf("   n           x            y            z         -d*An\n");
    printf("init %12.8f %12.8f %12.8f %12.8f\n",
            Q(_fixed_to_float)(x),
            Q(_fixed_to_float)(y),
            Q(_fixed_to_float)(z),
            0.0);
#endif
    for (k=1; k<n; k++) {
        d = ( z>=0 ) ? 0 : -1;
        // d = z >> 31;
    loop1:
        tx = x + ((y>>k)^d)-d;
        ty = y + ((x>>k)^d)-d;
        tz = z - ((Q(_sinhcosh_cordic_Ak_tab)[k]^d)-d);
        x = tx;
        y = ty;
        z = tz;
#if DEBUG_SINHCOSH_CORDIC
        printf("%4u %12.8f %12.8f %12.8f %12.8f\n",
            k,
            Q(_fixed_to_float)(x),
            Q(_fixed_to_float)(y),
            Q(_fixed_to_float)(z),
            Q(_fixed_to_float)(Q(_sinhcosh_cordic_Ak_tab)[k])*(z>=0?1.0:-1.0));
#endif
        if (k==i) {
            i = 3*i+1;
            goto loop1;
        }
    }

    // 
    *_cosh = x;
    *_sinh = y;
}

