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
// fixed-point gamma functions
//

#include <stdio.h>
#include <stdlib.h>

#include "liquidfpm.internal.h"

#define DEBUG_GAMMA 0

#if DEBUG_GAMMA
#   include <math.h>
#endif

// lgamma(z) = log(gamma(z))
Q(_t) Q(_lgamma)(Q(_t) _z)
{
    // precision (number of iterations)
    unsigned int _precision = Q(_bits);

    // approximation:
    //   ln(gamma(z)) ~ 0.5*[ln(2*pi) - ln(z)] +
    //                    z*[log(z + 1/(12*z + 0.1/z)) - 1]
    //                ~ 0.5*[ln(2*pi) - ln(z)] + z*[log(z + 1/(12*z)) - 1] for z >> 1
    //
    // NOTE: this approximation is inferior:
    //   ln(gamma(z)) ~ (z-0.5)*ln(z) - z + ln(2*pi)/2 + 1/(12*(z+1))
    //

    // gamma identities:
    //  (1) gamma(1+z) =  z*gamma(z)  ->  lgamma(z)  = lgamma(1+z) - ln(z)
    //  (2) gamma(1-z) = -z*gamma(z)  ->  lgamma(-z) = lgamma(1-z) - ln(-z)
    //if (_z < 0)
    //    return lgamma(1-z) - log(-z);

    // NOTE: because this approximation is not particularly good for z < 1,
    //       it is useful to invoke identity (1) above to improve its
    //       accuracy.  However, this is really only necessary if z < 1.
    if (_z < Q(_one))
        return Q(_lgamma)(_z + Q(_one)) - Q(_log_shiftadd)(_z,_precision);

    // variables:
    //   g0 : 0.5*[ln(2*pi) - ln(z)]
    //   g1 : 1/(12*z)
    //   g2 : ln(z + g1)
    Q(_t) g0 = (Q(_ln2pi) - Q(_log_shiftadd)(_z,_precision))>>1;
#if 0
    Q(_t) g1 = z1 < Q(_one) ? Q(_inv_newton)( (_z<<3) + (_z<<2), _precision )   // inv(z*12)
                            : Q(_mul)(Q(_inv_12),Q(_inv_newton)(_z,_precision)); // inv(z)*inv(12)
#else
    Q(_t) g1 = Q(_mul)(Q(_inv_12),Q(_inv_newton)(_z,_precision)); // inv(z)*inv(12)
#endif
    Q(_t) g2 = Q(_log_shiftadd)(_z + g1, _precision);

#if DEBUG_GAMMA
    float zf    = Q(_fixed_to_float)(_z);
    float g0f   = Q(_fixed_to_float)(g0);
    float g1f   = Q(_fixed_to_float)(g1);
    float g2f   = Q(_fixed_to_float)(g2);
    printf("  z     : %12.8f\n", zf);
    printf("  g0    : %12.8f\n", g0f);
    printf("  g1    : %12.8f\n", g1f);
    printf("  g2    : %12.8f\n", g2f);
    printf("  f(z)  : %12.8f\n", g0f + zf*(g2f-1));
    printf("  true  : %12.8f\n", logf(tgammaf(zf)));
#endif

    return g0 + Q(_mul)(_z,g2-Q(_one));
}

// tgamma(z) = exp(lgamma(z))
Q(_t) Q(_tgamma)(Q(_t) _z)
{
    // precision (number of iterations)
    unsigned int _precision = Q(_bits);

    // compute exp(lgamma(_z)) and return
    return Q(_exp_shiftadd)( Q(_lgamma)(_z), _precision );
}

