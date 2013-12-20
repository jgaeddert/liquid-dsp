/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
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
// multi-stage half-band resampler
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

// 
// forward declaration of internal methods
//

struct MSRESAMP2(_s) {
    // user-defined parameters
    unsigned int num_stages;            // number of half-band stages
};

// create multi-stage half-band resampler
//  _num_stages : number of resampling stages
//  _fc         : filter cut-off frequency 0 < _fc < 0.5
//  _f0         : filter center frequency
//  _As         : stop-band attenuation [dB]
MSRESAMP2() MSRESAMP2(_create)(unsigned int _num_stages,
                               float        _fc,
                               float        _f0,
                               float        _As)
{
    // validate input
    if (_num_stages == 0) {
        fprintf(stderr,"error: msresamp2_%s_create(), number of stages must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_num_stages > 12) {
        fprintf(stderr,"error: msresamp2_%s_create(), number of stages should not exceed 12\n", EXTENSION_FULL);
        exit(1);
    }

    // create object
    MSRESAMP2() q = (MSRESAMP2()) malloc(sizeof(struct MSRESAMP2(_s)));

    // return main object
    return q;
}

// destroy msresamp2 object, freeing all internally-allocated memory
void MSRESAMP2(_destroy)(MSRESAMP2() _q)
{
    // destroy main object
    free(_q);
}

// print msresamp2 object internals
void MSRESAMP2(_print)(MSRESAMP2() _q)
{
    printf("multi-stage half-band resampler, rate : %u stages\n", _q->num_stages);
}

// reset msresamp2 object internals, clear filters and nco phase
void MSRESAMP2(_reset)(MSRESAMP2() _q)
{
}

// get filter delay (output samples)
float MSRESAMP2(_get_delay)(MSRESAMP2() _q)
{
    return 0.0f;
}

// execute multi-stage resampler as decimator               
//  _q      : msresamp object                               
//  _x      : input sample array  [size: 2^_num_stages x 1] 
//  _y      : output sample pointer                         
void MSRESAMP2(_decim_execute)(MSRESAMP2() _q,
                               TI *        _x,
                               TO *        _y)
{
}

// execute multi-stage resampler as interpolator            
//  _q      : msresamp object                               
//  _x      : input sample                                  
//  _y      : output sample array  [size:2^_num_stages x 1] 
void MSRESAMP2(_interp_execute)(MSRESAMP2() _q,
                                TI          _x,
                                TO *        _y)
{
}

