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
// sin/cos using look-up table
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "liquid.internal.h"
#include "liquidfpm.internal.h"

#define DEBUG_SINCOS_TAB 0

Q(_t) Q(_sin_tab)(Q(_t) _theta)
{
    Q(_t) sine;
    Q(_t) cosine;
    Q(_sincos_tab)(_theta,&sine,&cosine);
    return sine;
}

Q(_t) Q(_cos_tab)(Q(_t) _theta)
{
    Q(_t) sine;
    Q(_t) cosine;
    Q(_sincos_tab)(_theta,&sine,&cosine);
    return cosine;
}

void Q(_sincos_tab)(Q(_t)   _theta,
                    Q(_t) * _sin,
                    Q(_t) * _cos)
{
    // Ensure theta >= 0
    // Because the qtype representation of angles is [-2*pi,2*pi] we can guarantee
    // adding 2*pi will make theta non-negative
    if (_theta < 0)
        _theta += Q(_2pi);

    // _theta now has the form : 0QQB BBBB BBBx xxxx xxxx xxxx xxxx x...
    // Q : 2-bit quadrant
    // B : 8-bit phase resolution
    // x : remaining bits phase resolution, ignored
    unsigned int quad  = (_theta >> (Q(_bits)-3))  & 0x00000003;
    unsigned int index = (_theta >> (Q(_bits)-11)) & 0x000000ff;

    // ensure index is in [0,255]
    assert(index >= 0 && index < 256);

    // extract from table
    Q(_t) t0 = Q(_sintab)[    index];
    Q(_t) t1 = Q(_sintab)[255-index];

    // compute appropriate sin|cos from quadrant
    //  1 | 0
    //  --+--
    //  3 | 2
    switch (quad) {
    case 0: *_cos =  t1; *_sin =  t0;  break;
    case 1: *_cos = -t0; *_sin =  t1;  break;
    case 2: *_cos = -t1; *_sin = -t0;  break;
    case 3: *_cos =  t0; *_sin = -t1;  break;
    default:;
    }

#if DEBUG_SINCOS_TAB
    // print results
    float thetaf = Q(_angle_fixed_to_float)(_theta);
    printf("theta=%12.8f (0x%.8x)[%1u:%3u]:%12.8f(%12.8f),%12.8f(%12.8f)\n",
            thetaf, _theta, quad, index,
            cosf(thetaf), Q(_fixed_to_float)(*_cos),
            sinf(thetaf), Q(_fixed_to_float)(*_sin));
#endif
}

