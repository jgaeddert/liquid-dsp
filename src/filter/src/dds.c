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
// 2nd-order integrating loop filter
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// defined:
//  DDS()           name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DECIM()
//  INTERP()
//  RESAMP()
//  PRINTVAL()      print macro

#define DDS(name)           LIQUID_CONCAT(dds_cccf,name)
#define T                   float complex
#define WINDOW(name)        LIQUID_CONCAT(cfwindow,name)
#define DECIM(name)         LIQUID_CONCAT(decim_cccf,name)
#define INTERP(name)        LIQUID_CONCAT(interp_cccf,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp_cccf,name)

struct DDS(_s) {
    int is_interp;
    float rate;

    // initial windowing buffer
    WINDOW() w;

    // initial decimation/interpolation stages
    DECIM() * decimators;
    INTERP() * interpolators;
    unsigned int num_stages;
    float halfband_rate;

    // arbitrary resampling clean-up stage
    RESAMP() frac_resamp;
    float frac_rate;

    // final mixing stage
    nco ncox;
};

DDS() DDS(_create)(float _fc_in,            // input carrier
                   float _fc_out,           // output carrier
                   float _rate)             // rate (output/input)
{
    // create object
    DDS() q = (DDS()) malloc(sizeof(struct DDS(_s)));
    q->rate = _rate;
    float log2rate;

    printf("creating dds...\n");

    if (q->rate <= 0.0f) {
        printf("error: dds_xxxt_create(), invalid rate %12.4e (must be greater than 0)\n", q->rate);
        exit(0);
    } else if (q->rate >= 1.0f) {
        // interpolation
        q->is_interp = 1;
        log2rate = log2f(q->rate);
        q->num_stages = floorf(log2rate);
        q->halfband_rate = (float)(1<<q->num_stages);
        q->frac_rate = q->rate / q->halfband_rate;
    } else {
        // decimation
        q->is_interp = 0;
        log2rate = -log2f(q->rate);
        q->num_stages = floorf(log2rate);
        q->halfband_rate = 1.0f / (float)(1<<q->num_stages);
        q->frac_rate = q->rate / q->halfband_rate;
    }

    printf("done!\n");

    return q;
}

void DDS(_destroy)(DDS() _q)
{
    free(_q);
}

void DDS(_print)(DDS() _q)
{
    printf("direct digital synthesizer (dds), rate : %12.4e\n", _q->rate);
    printf("    %u halfband %s stages, rate : %12.4e\n",
                    _q->num_stages,
                    _q->is_interp ? "interp" : "decim",
                    _q->halfband_rate);
    printf("    fractional stage rate : %12.8f\n", _q->frac_rate);
}

void DDS(_reset)(DDS() _q)
{
    // reset internal filter state variables
    unsigned int i;
    for (i=0; i<_q->num_stages; i++) {
        if (_q->is_interp) {
            //INTERP(_reset)(_q->interpolators[i]);
        } else {
            //DECIM(_reset)(_q->decimators[i]);
        }
    }

    //RESAMP(_reset)(_q->frac_resamp);
    
    nco_reset(_q->ncox);
}

// push input and compute output(s)
void DDS(_execute)(DDS() _q,
                   float complex _x,
                   float complex * _y,
                   unsigned int * _num_written)
{
    *_num_written = 0;
}

// 
// internal
//

// execute decimator
void DDS(_execute_decim)(DDS() _q,
                         T _x,
                         T * _y,
                         unsigned int * _num_written)
{
    *_num_written = 0;
}

// execute interpolator
void DDS(_execute_interp)(DDS() _q,
                          T _x,
                          T * _y,
                          unsigned int * _num_written)
{
    *_num_written = 0;
}

