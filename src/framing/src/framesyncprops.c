/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
// framesyncprops.c
//
// Default and generic frame synchronizer properties
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "liquid.internal.h"

framesyncprops_s framesyncprops_default = {
    // automatic gain control
    1e-1f,      // agc_bw0
    1e-2f,      // agc_bw1
    1e-3f,      // agc_gmin
    1e4f,       // agc_gmax
    // symbol timing recovery
    0.01f,      // sym_bw0
    0.001f,     // sym_bw1
    // phase-locked loop
    0.020f,     // pll_bw0
    0.005f,     // pll_bw1
    // symbol timing recovery
    2,          // k
    32,         // npfb
    3,          // m
    0.7f,       // beta
    // squelch
    1,          // squelch_enabled
    0,          // autosquelch_enabled
    -35.0f,     // squelch_threshold
    // equalizer
    0,          // number of equalizer taps
    0.999       // RLS equalizer forgetting factor
};

void framesyncprops_init_default(framesyncprops_s * _props)
{
    memmove(_props, &framesyncprops_default, sizeof(framesyncprops_s));
}

