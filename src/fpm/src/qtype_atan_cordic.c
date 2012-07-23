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
// atan using CORDIC
// 

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define DEBUG_ATAN_CORDIC 0

void Q(_atan2_cordic)(Q(_t) _y,
                      Q(_t) _x,
                      Q(_t) * _r,
                      Q(_t) * _theta,
                      unsigned int _n)
{
    // negate calculated angle if _y < 0
    int negate = (_y < 0);

    // subtract calculated angle from pi if _x < 0
    int sub_from_pi = (_x < 0);

    // take absolute value as we are only interested in absolute ratio
    // at this point.
    _y = abs(_y);
    _x = abs(_x);

    Q(_atan2_cordic_base)(_y,_x,_r,_theta,_n);
    
    if (sub_from_pi)
        *_theta = Q(_pi) - *_theta;
    if (negate)
        *_theta = -(*_theta);
}

void Q(_atan2_cordic_base)(Q(_t) _y,
                           Q(_t) _x,
                           Q(_t) * _r,
                           Q(_t) * _theta,
                           unsigned int _n)
{

    Q(_t) x = _x;
    Q(_t) y = _y;
    Q(_t) z = 0;
    Q(_t) d,tx,ty,tz;
    unsigned int i;
    unsigned int n = _n > Q(_bits) ? Q(_bits) : _n;

#if DEBUG_ATAN_CORDIC
    printf("   n            x            y            z        -d*An\n");
    printf("init %12.8f %12.8f %12.8f %12.8f\n",
            Q(_fixed_to_float)(x),
            Q(_fixed_to_float)(y),
            Q(_fixed_to_float)(z),
            0.0);
#endif
    for (i=0; i<n; i++) {
        d = ( y>=0 ) ? -1 : 0;

        tx = x - ((y>>i)^d)-d;
        ty = y + ((x>>i)^d)-d;
        tz = z - ((Q(_cordic_Ak_tab)[i]^d)-d);
        x = tx;
        y = ty;
        z = tz;
#if DEBUG_ATAN_CORDIC
        printf("%4u %12.8f %12.8f %12.8f %12.8f\n",
            i,
            Q(_fixed_to_float)(x),
            Q(_fixed_to_float)(y),
            Q(_angle_fixed_to_float)(z),
            Q(_fixed_to_float)(Q(_cordic_Ak_tab[i]))*(y>=0?-1.0:1.0));
#endif
    }
    //Q(_t) x = Q(_cordic_k_inv); // TODO : initialize with cordic_Kinv_tab[_n-1];
    *_r = Q(_mul)(x,Q(_cordic_k_inv));
    *_theta = z;
}

// compute hypotenuse of right triangle
Q(_t) Q(_hypot_cordic)(Q(_t) _y,
                       Q(_t) _x,
                       unsigned int _precision)
{
    // create temporary variables
    Q(_t) r;        // hypotenuse
    Q(_t) theta;    // angle

    // compute hypotenuse using atan2 method (above)
    Q(_atan2_cordic)(_y, _x, &r, &theta, _precision);

    // return hypotenuse
    return r;
}
