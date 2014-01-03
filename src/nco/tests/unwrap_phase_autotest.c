/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#include <complex.h>
#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: regular phase-unwrapping
//
void autotest_nco_unwrap_phase()
{
    unsigned int n=32;  // number of steps
    float tol = 1e-6f;  // error tolerance
    
    // initialize data arrays
    float phi[n];       // original array
    float theta[n];     // wrapped array
    float phi_hat[n];   // unwrapped array

    float phi0 = 3.0f;  // initial phase
    float dphi = 0.1f;  // phase step

    unsigned int i;
    for (i=0; i<n; i++) {
        // phase input
        phi[i] = phi0 + i*dphi;

        // wrapped array
        theta[i] = phi[i];
        while (theta[i] >  M_PI) theta[i] -= 2*M_PI;
        while (theta[i] < -M_PI) theta[i] += 2*M_PI;

        // initialize output
        phi_hat[i] = theta[i];
    }

    // unwrap phase
    liquid_unwrap_phase(phi_hat, n);

    // compare input to output
    for (i=0; i<n; i++)
        CONTEND_DELTA( phi[i], phi_hat[i], tol );
}

