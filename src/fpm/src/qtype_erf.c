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
// fixed-point erf(), erfc()
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define DEBUG_ERF 0

#if DEBUG_ERF
#   include <math.h>
#endif

//                       /x
// erf(x) = 2/sqrt(pi) * | exp(-t^2)dt
//                       /0
//                       
Q(_t) Q(_erf)(Q(_t) _x)
{
    // precision (number of iterations)
    unsigned int _precision = Q(_bits);

    // approximation:
    //                     [        {      4/pi + ax^2 }  ](1/2)
    //   erf(x) ~ sgn(x) * [ 1 - exp{ -x^2 ----------- }  ]
    //                     [        {       1   + ax^2 }  ]
    // where
    //   a = 8*(pi-3) / 3*pi(4-pi) ~ 0.140012288686666
    //
    //
    // small value approximation:
    //                                    [                                 ]
    //  erf(x) ~ 2/sqrt(pi) * exp(-x^2) * [ x + (2/3)*x^3 + (4/15)*x^5 + ...]
    //                                    [                                 ]
    //

    if (_x < 0)
        return -Q(_erf)(-_x);
    else if (Q(_intbits) > 2 && _x > (Q(_one)<<2))
        return Q(_one);

    // compute x^2
    Q(_at) x2 = ((Q(_at))_x * (Q(_at))_x) >> Q(_fracbits);

    // small signal approximation
    if (_x < Q(_one)>>1) {
        // compute x^3
        Q(_at) x3 = ((Q(_at))x2 * (Q(_at))_x) >> Q(_fracbits);

        // compute exp{-x^2}
        Q(_at) expx2 = Q(_exp_shiftadd)(-(Q(_t))x2, _precision);

        // compute sum
        Q(_at) series = (Q(_at))_x + ((x3 / 3)<<1);

        // 2/sqrt(pi)
        Q(_at) p = Q(_float_to_fixed)(1.12837916709551f);

        return (((p * expx2)>>Q(_fracbits))*series) >> Q(_fracbits);
    }

    // compute a*x^2
    Q(_at) a   = Q(_float_to_fixed)(0.140012288686666f);
    Q(_at) ax2 = (x2 * a) >> Q(_fracbits);

    // compute 4/pi + ax^2
    Q(_at) t0 = Q(_float_to_fixed)(1.27323954473516f) + ax2;
    
    // compute 1 + ax^2
    Q(_at) t1 = Q(_one) + ax2;
    
    // compute ratio (no need to shift)
    Q(_at) g = (x2 * t0) / t1;

    // compute exp{-g}
    Q(_t) expmg = Q(_exp_shiftadd)( -(Q(_t))g, _precision );

    // compute sqrt(1-exp{-g})
    Q(_t) y = Q(_sqrt_newton)( Q(_one) - expmg, _precision );


#if DEBUG_ERF
    float xf    = Q(_fixed_to_float)(_x);
    float g0f   = Q(_fixed_to_float)(g0);
    float g1f   = Q(_fixed_to_float)(g1);
    float g2f   = Q(_fixed_to_float)(g2);
    printf("  x     : %12.8f\n", xf);
    printf("  g0    : %12.8f\n", g0f);
    printf("  g1    : %12.8f\n", g1f);
    printf("  g2    : %12.8f\n", g2f);
    printf("  f(x)  : %12.8f\n", g0f + xf*(g2f-1));
    printf("  true  : %12.8f\n", logf(tgammaf(xf)));
#endif

    return y;
}

// erfc(x) = 1 - erf(x)
Q(_t) Q(_erfc)(Q(_t) _x)
{
    return Q(_one) - Q(_erf)(_x);
}

