/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
 *                                      Institute & State University
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
// gamma functions
//
// liquid_lngammaf()        :   log( Gamma(z) )
// liquid_gammaf()          :   Gamma(z)
// liquid_lnlowergammaf()   :   log( gamma(z,a) ), lower incomplete
// liquid_lnuppergammaf()   :   log( Gamma(z,a) ), upper incomplete
// liquid_lowergammaf()     :   gamma(z,a), lower incomplete
// liquid_uppergammaf()     :   Gamma(z,a), upper incomplete
// liquid_factorialf()      :   z!
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

#define NUM_LNGAMMA_ITERATIONS (64)
#define EULER_GAMMA            (0.57721566490153286)
float liquid_lngammaf(float _z)
{
    float g;
    if (_z < 0) {
        fprintf(stderr,"error: liquid_lngammaf(), undefined for z <= 0\n");
        exit(1);
    } else if (_z < 1.0f) {
#if 0
    } else if (_z < 1.0f) {
        // low value approximation
        g = -logf(_z) - EULER_GAMMA*_z;
        unsigned int n;
        float z_by_n;   // value of z/n
        for (n=1; n<NUM_LNGAMMA_ITERATIONS; n++) {
            z_by_n = _z / (float)n;
            g += -logf(1.0f + z_by_n) + z_by_n;
        }
#else
        g = liquid_lngammaf(1.0f + _z) - logf(_z);

        // apply correction factor
        //g += 0.0405f*(1.0f - tanhf(0.5f*logf(_z)));
#endif
    } else {
        // high value approximation
        g = 0.5*( logf(2*M_PI)-log(_z) );
        g += _z*( logf(_z+(1/(12.0f*_z-0.1f/_z)))-1);
    }
    return g;
}

float liquid_gammaf(float _z)
{
    if (_z < 0) {
        // use identities
        //  (1) gamma(z)*gamma(-z) = -pi / (z*sin(pi*z))
        //  (2) z*gamma(z) = gamma(1+z)
        //
        // therefore:
        //  gamma(z) = pi / ( gamma(1-z) * sin(pi*z) )
        float t0 = liquid_gammaf(1.0 - _z);
        float t1 = sinf(M_PI*_z);
        if (t0==0 || t1==0)
            fprintf(stderr,"warning: liquid_gammaf(), divide by zero\n");
        return M_PI / (t0 * t1);
    } else {
        return expf( liquid_lngammaf(_z) );
    }
}

// ln( gamma(z,alpha) ) : lower incomplete gamma function
#define NUM_LOWERGAMMA_ITERATIONS 32
float liquid_lnlowergammaf(float _z, float _alpha)
{
    float t0 = _z * logf(_alpha);
    float t1 = liquid_lngammaf(_z);
    float t2 = -_alpha;
    float t3 = 0.0f;

    unsigned int k;
#if 0
    float alpha_k = 1.0f;
    for (k=0; k<NUM_LOWERGAMMA_ITERATIONS; k++) {
        // accumulate
        t3 += alpha_k / liquid_gammaf(_z + (float)k + 1.0f);

        // update alpha^k
        alpha_k += _alpha;
    }
#else
    float log_alpha = logf(_alpha);
    for (k=0; k<NUM_LOWERGAMMA_ITERATIONS; k++) {
        // compute log( alpha^k / Gamma(_z + k + 1) )
        //         = k*log(alpha) - lnGamma(_z + k + 1)
        float t = k*log_alpha - liquid_lngammaf(_z + (float)k + 1.0f);

        // TODO : check value of t and break if below threshold

        // accumulate e^t
        t3 += expf(t);
    }
#endif

    return t0 + t1 + t2 + logf(t3);
}

// ln( Gamma(z,alpha) ) : upper incomplete gamma function
float liquid_lnuppergammaf(float _z, float _alpha)
{
    return logf( liquid_gammaf(_z) - liquid_lowergammaf(_z,_alpha) );
}


// gamma(z,alpha) : lower incomplete gamma function
float liquid_lowergammaf(float _z, float _alpha)
{
    return expf( liquid_lnlowergammaf(_z,_alpha) );
}

// Gamma(z,alpha) : upper incomplete gamma function
float liquid_uppergammaf(float _z, float _alpha)
{
    return expf( liquid_lnuppergammaf(_z,_alpha) );
}

// compute _n!
float liquid_factorialf(unsigned int _n) {
    return fabsf(liquid_gammaf((float)(_n+1)));
}

