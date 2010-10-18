/*
 * Copyright (c) 2010 Joseph Gaeddert
 * Copyright (c) 2010 Virginia Polytechnic Institute & State University
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
// carrier_estimation.c
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>

#include "liquid.internal.h"

#define DEBUG_CARRIER_ESTIMATION (0)

// estimate carrier frequency, phase parameters using first-order
// polynomial fit
//  _g          :   complex sinusoidal input
//  _n          :   length of _g
//  _dphi_hat   :   output frequency estimate (phase slope)
//  _phi_hat    :   output phase estimate
void liquid_estimate_carrier(float complex * _g,
                             unsigned int _n,
                             float * _dphi_hat,
                             float * _phi_hat)
{
    float t[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        t[i] = (float)i;

    liquid_estimate_carrier_nonlinear(t, _g, _n, _dphi_hat, _phi_hat);
}

// estimate carrier frequency, phase parameters using first-order
// polynomial fit (non-linear time step)
//  _t          :   time vector
//  _g          :   complex sinusoidal input
//  _n          :   length of _t, _g
//  _dphi_hat   :   output frequency estimate (phase slope)
//  _phi_hat    :   output phase estimate
void liquid_estimate_carrier_nonlinear(float * _t,
                                       float complex * _g,
                                       unsigned int _n,
                                       float * _dphi_hat,
                                       float * _phi_hat)
{
    // make initial estimate of carrier frequency
    float dphi_hat = liquid_estimate_carrier_frequency(_t, _g, _n);

    // unwrap complex phase
    float phi[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        phi[i] = cargf(_g[i]*cexpf(-_Complex_I*dphi_hat*_t[i])) + dphi_hat*_t[i];

    // use first-order (2-parameter) polynomial curve-fit
    float p[2];
    polyf_fit(_t, phi, _n, p, 2);

    *_phi_hat  = p[0];  // y-intercept point
    *_dphi_hat = p[1];  // phase slope
}




// 
// internal methods
//


// estimate carrier frequency by unwrapping phase using iterative
// derivative estimation
//  _t      :   time vector
//  _g      :   complex input
//  _n      :   size of inputs _t, _g
//
// returns initial frequency estimate
float liquid_estimate_carrier_frequency(float * _t,
                                        float complex * _g,
                                        unsigned int _n)
{
    // exit criteria
    float tol = 0.001f;         // error tolerance
    unsigned int n_max = 15;    // maximum number of iterations

    float complex g_hat[_n];    // unwrapped input array
    float dphi_hat = 0.0f;      // phase gradient estimate
    float err = 0.0f;           // phase gradient error
    unsigned int n=0;           // iteration counter
    unsigned int i;

    do {
        // de-rotate input by phase gradient estimate
        for (i=0; i<_n; i++)
            g_hat[i] = _g[i]*cexpf(-_Complex_I*dphi_hat*_t[i]);

        // estimate step
        err = liquid_unwrapcf_iterative_step(_t, g_hat, _n);

        // increment estimate by difference
        dphi_hat += err;

#if DEBUG_CARRIER_ESTIMATION
        // print results to screen
        printf("%3u : %12.8f (e=%12.8f)\n", n, dphi_hat, err);
#endif

        n++;
    } while (fabsf(err) > tol && n < n_max);

    // ...
    //memmove(_g, g_hat, _n*sizeof(float complex));

    return dphi_hat;
}

// iterative derivative estimation step
//  _t      :   time vector
//  _g_hat  :   complex input
//  _n      :   size of inputs _t, _g
float liquid_unwrapcf_iterative_step(float * _t,
                                     float complex * _g_hat,
                                     unsigned int _n)
{
    // find mean of phase difference
    float dphi;
    float dphi_mean=0.0f;
    unsigned int i;
    for (i=1; i<_n; i++) {
        // unwrap phase
        dphi = cargf( _g_hat[i] * conjf(_g_hat[i-1]) ) / (_t[i] - _t[i-1]);

        // constrain dphi to be in [-pi,pi)
        while ( dphi >  M_PI ) dphi -= 2*M_PI;
        while ( dphi < -M_PI ) dphi += 2*M_PI;

        dphi_mean += dphi;
    }

    dphi_mean /= (float)(_n-1);

    return dphi_mean;
}


