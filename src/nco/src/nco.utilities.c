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

//
// nco.utilities.c
//
// Numerically-controlled oscillator (nco) utilities
//

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// unwrap phase of array (basic)
void liquid_unwrap_phase(float * _theta,
                         unsigned int _n)
{
    unsigned int i;
    for (i=1; i<_n; i++) {
        while ( (_theta[i] - _theta[i-1]) >  M_PI ) _theta[i] -= 2*M_PI;
        while ( (_theta[i] - _theta[i-1]) < -M_PI ) _theta[i] += 2*M_PI;
    }
}

// unwrap phase of array (advanced)
void liquid_unwrap_phase2(float * _theta,
                          unsigned int _n)
{
    fprintf(stderr,"warning: liquid_unwrap_phase2() has not yet been tested!\n");

    unsigned int i;

    // make an initial estimate of phase difference
    float dphi = 0.0f;
    for (i=1; i<_n; i++)
        dphi += _theta[i] - _theta[i-1];

    dphi /= (float)(_n-1);

    for (i=1; i<_n; i++) {
        while ( (_theta[i] - _theta[i-1]) >  M_PI+dphi ) _theta[i] -= 2*M_PI;
        while ( (_theta[i] - _theta[i-1]) < -M_PI+dphi ) _theta[i] += 2*M_PI;
    }
}



