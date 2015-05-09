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

