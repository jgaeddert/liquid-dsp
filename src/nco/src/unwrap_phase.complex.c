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
// unwrap_phase.c
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>

#include "liquid.internal.h"

// unwrap linearly time-spaced phase
//  _g          :   linearly-spaced complex gain samples
//  _n          :   size of input
//  _method     :   phase-unwrapping method
void liquid_unwrapcf(float complex * _g,
                     unsigned int _n,
                     int _method)
{
    float t[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        t[i] = (float)i;

    liquid_unwrapcf_nonlinear(t,_g,_n,_method);
}


// unwrap non-linearly time-spaced phase
//  _t          :   time samples
//  _g          :   linearly-spaced complex gain samples
//  _n          :   size of inputs _t, _g
//  _method     :   phase-unwrapping method
void liquid_unwrapcf_nonlinear(float * _t,
                               float complex * _g,
                               unsigned int _n,
                               int _method)
{
    // switch algorithm based on _method
    switch (_method) {
    case LIQUID_UNWRAP_FLAT:
        liquid_unwrapcf_flat(_t, _g, _n);
        break;
    case LIQUID_UNWRAP_ITERATIVE:
        liquid_unwrapcf_iterative(_t, _g, _n);
        break;
    default:
        fprintf(stderr,"error: liquid_unwrapcf_nonlinear(), unknown method: %d\n", _method);
        exit(1);
    }
}

// estimate phase parameters using first-order polynomial fit
//  _t          :   time vector
//  _g          :   complex gain input (unwrapped)
//  _n          :   length of _t, _g
//  _dphi_hat   :   frequency estimate (phase slope)
//  _phi_hat    :   phase estimate
void liquid_estimate_phase(float * _t,
                           float complex * _g,
                           unsigned int _n,
                           float * _dphi_hat,
                           float * _phi_hat)
{
    float phi[_n];

    unsigned int i;
    for (i=0; i<_n; i++)
        phi[i] = cargf(_g[i]);

    // use first-order (2-parameter) polynomial curve-fit
    float p[2];
    polyf_fit(_t, phi, _n, p, 2);

    *_phi_hat  = p[0];  // y-intercept point
    *_dphi_hat = p[1];  // phase slope
}




// 
// internal methods
//

// unwrap phase using flat derivative method
//  _t      :   time vector
//  _g      :   complex input
//  _n      :   size of inputs _t, _g
void liquid_unwrapcf_flat(float * _t,
                          float complex * _g,
                          unsigned int _n)
{
#if 0
    unsigned int i;
    float dphi;
    for (i=1; i<_n; i++) {
        // observe phase difference
        dphi = cargf(_theta[i] * conjf(_theta[i-1])) / (_t[i] - _t[i-1]);

        // de-rotate input to be in [-pi,pi)
        while ( (_theta[i] - _theta[i-1]) >  M_PI ) _theta[i] -= 2*M_PI;
        while ( (_theta[i] - _theta[i-1]) < -M_PI ) _theta[i] += 2*M_PI;
    }
#endif
}

// unwrap phase using iterative derivative estimation
//  _t      :   time vector
//  _g      :   complex input
//  _n      :   size of inputs _t, _g
void liquid_unwrapcf_iterative(float * _t,
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

#if 1
        // print results to screen
        printf("%3u : %12.8f (e=%12.8f)\n", n, dphi_hat, err);
#endif

        n++;
    } while (fabsf(err) > tol && n < n_max);

    // ...
    memmove(_g, g_hat, _n*sizeof(float complex));
}

// iterative derivative estimation step
//  _t      :   time vector
//  _g      :   complex input
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
        dphi = cargf( _g_hat[i] * conjf(_g_hat[i-1]) );

        // constrain dphi to be in [-pi,pi)
        while ( dphi >  M_PI ) dphi -= 2*M_PI;
        while ( dphi < -M_PI ) dphi += 2*M_PI;

        dphi_mean += dphi;
    }

    dphi_mean /= (float)(_n-1);

    return dphi_mean;
}

