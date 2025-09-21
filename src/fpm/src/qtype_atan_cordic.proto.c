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
