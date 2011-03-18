/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
//
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// Rice-K
float randricekf(float _K, float _omega)
{
    float complex x, y;
    float s = sqrtf((_omega*_K)/(_K+1));
    float sig = sqrtf(0.5f*_omega/(_K+1));
    crandnf(&x);
    y = _Complex_I*( crealf(x)*sig + s ) +
                   ( cimagf(x)*sig     );
    return cabsf(y);
}


// Rice-K random number probability distribution function
float randricekf_pdf(float _x,
                     float _K,
                     float _omega)
{
    if (_x < 0.0f)
        return 0.0f;

    float s = sqrtf((_omega*_K)/(_K+1));
    float sig = sqrtf(0.5f*_omega/(_K+1));

    float t = _x*_x + s*s;

    float sig2 = sig*sig;

    return (_x / sig2) * expf(-t / (2*sig2)) * besseli_0(_x*s/sig2);
}

// Rice-K random number cumulative distribution function
// TODO : check this computation
float randricekf_cdf(float _x,
                     float _K,
                     float _omega)
{
    if (_x < 0.0f)
        return 0.0f;

    float s = sqrtf((_omega*_K)/(_K+1));
    float sig = sqrtf(0.5f*_omega/(_K+1));

    return 1.0f - liquid_MarcumQ1(s/sig, _x/sig);
}


