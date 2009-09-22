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
//  RESAMP2()       halfband resampler
//  RESAMP()        arbitrary resampler
//  PRINTVAL()      print macro

#define DDS(name)           LIQUID_CONCAT(dds_cccf,name)
#define T                   float complex
#define WINDOW(name)        LIQUID_CONCAT(cfwindow,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2_cccf,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp_cccf,name)

struct DDS(_s) {
    int is_interp;
    float rate;
    float fc0;
    float fc1;

    // initial decimation/interpolation stages
    RESAMP2() * halfband_resamp;
    unsigned int num_stages;
    float halfband_rate;
    float * fc;             // filter center frequency
    float * slsl;           // filter sidelobe suppression level
    unsigned int * h_len;   // filter length
    T * buffer;
    unsigned int buffer_len;
    T ** b;

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
    q->fc0 = _fc_in;
    q->fc1 = _fc_out;
    float log2rate;

    if (q->rate <= 0.0f) {
        printf("error: dds_xxxt_create(), invalid rate %12.4e (must be greater than 0)\n", q->rate);
        exit(0);
    } else if (q->rate >= 1.0f) {
        // interpolation
        q->is_interp = 1;
        log2rate = log2f(q->rate);
        q->num_stages = roundf(log2rate); // floorf(log2rate);
        q->halfband_rate = (float)(1<<q->num_stages);
    } else {
        // decimation
        q->is_interp = 0;
        log2rate = -log2f(q->rate);
        q->num_stages = roundf(log2rate); // floorf(log2rate);
        q->halfband_rate = 1.0f / (float)(1<<q->num_stages);
    }
    // compute fractional rate
    q->frac_rate = q->rate / q->halfband_rate;

    // allocate memory for filter properties
    q->fc    = (float*) malloc((q->num_stages)*sizeof(float));
    q->slsl  = (float*) malloc((q->num_stages)*sizeof(float));
    q->h_len = (unsigned int*) malloc((q->num_stages)*sizeof(unsigned int));
    unsigned int i;
    float fc;
    if (q->is_interp) {
        fc = q->fc0;
        for (i=0; i<q->num_stages; i++) {
            q->fc[i] = fc;
            q->slsl[i] = 60.0f;
            q->h_len[i] = i==0 ? 16 : q->h_len[i-1]/2;
        }
    } else {
        for (i=0; i<q->num_stages; i++) {
            q->fc[i] = 0.0f;
            q->slsl[i] = 60.0f;
            q->h_len[i] = i==0 ? 8 : q->h_len[i-1]*2;
        }
    }

    // allocate memory for buffering
    q->buffer_len = 1<<(q->num_stages+1);
    printf("buffer length : %u\n", q->buffer_len);
    q->buffer = (T*) malloc((q->buffer_len)*sizeof(T));

    q->b = (T**) malloc((q->num_stages+1)*sizeof(T*));
    unsigned int k, n=0;
    if (q->is_interp) {
        k = 1;
        for (i=0; i<q->num_stages+1; i++) {
            printf("n : %u\n", n);
            q->b[i] = q->buffer + n;
            n += k;
            k <<= 1;
        }
    } else {
        k = 1<<q->num_stages;
        for (i=0; i<q->num_stages+1; i++) {
            printf("n : %u\n", n);
            q->b[i] = q->buffer + n;
            n += k;
            k >>= 1;
        }
    }

    // TODO : compute resampler parameters
    // TODO : generate resamplers

    // allocate memory for resampler pointers
    q->halfband_resamp = (RESAMP2()*) malloc((q->num_stages)*sizeof(RESAMP()*));
    for (i=0; i<q->num_stages; i++) {
        q->halfband_resamp[i] = RESAMP2(_create)(q->h_len[i],
                                                 q->fc[i],
                                                 q->slsl[i]);
    }

    return q;
}

void DDS(_destroy)(DDS() _q)
{
    free(_q->h_len);
    free(_q->fc);

    // destroy buffer, buffer pointers
    free(_q->buffer);
    free(_q->b);

    // destroy halfband resampler objects
    unsigned int i;
    for (i=0; i<_q->num_stages; i++)
        RESAMP2(_destroy)(_q->halfband_resamp[i]);
    free(_q->halfband_resamp);

    // destroy DDS object
    free(_q);
}

void DDS(_print)(DDS() _q)
{
    printf("direct digital synthesizer (dds), rate : %12.4e\n", _q->rate);
    printf("      fc0 : %12.4e\n", _q->fc0);
    printf("      fc1 : %12.4e\n", _q->fc1);
    printf("    halfband stages : %3u %s, rate : %12.4e\n",
                    _q->num_stages,
                    _q->is_interp ? "interp" : "decim ",
                    _q->halfband_rate);
    unsigned int i;
    for (i=0; i<_q->num_stages; i++) {
        printf("      [%3u] : fc = %12.4e, %3u taps\n",
                    i,
                    _q->fc[i],
                    _q->h_len[i]);
    }
    printf("    fractional stage,             rate : %12.4e\n",
                    _q->frac_rate);
    printf("      16 filters\n");
}

void DDS(_reset)(DDS() _q)
{
    // reset internal filter state variables
    unsigned int i;
    for (i=0; i<_q->num_stages; i++) {
        RESAMP2(_clear)(_q->halfband_resamp[i]);
    }

    RESAMP(_reset)(_q->frac_resamp);
    
    nco_reset(_q->ncox);
}

// push input and compute output(s)
void DDS(_execute)(DDS() _q,
                   float complex _x,
                   float complex * _y,
                   unsigned int * _num_written)
{
    if (_q->is_interp)
        DDS(_execute_interp)(_q, _x, _y, _num_written);
    else
        DDS(_execute_decim)(_q, _x, _y, _num_written);
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
    // TODO : decimator needs to wait until enough samples are written into the buffer
    *_num_written = 0;
}

// execute interpolator
void DDS(_execute_interp)(DDS() _q,
                          T _x,
                          T * _y,
                          unsigned int * _num_written)
{
    // TODO : increment NCO

    // set initial buffer value
    _q->buffer[0] = _x;

    unsigned int s, i;
    unsigned int k=1; // number of inputs for this stage
    T * x0 = NULL, * x1 = NULL;
    for (s=0; s<_q->num_stages; s++) {
        //printf("stage %3u, k = %3u\n", s, k);
        x0 = _q->b[s];      // input buffer
        x1 = _q->b[s+1];    // output buffer
        for (i=0; i<k; i++) {
            RESAMP2(_interp_execute)(_q->halfband_resamp[s], x0[i], &x1[2*i]);
        }
        k <<= 1;
    }

    // TODO : execute arbitrary resampler
    

    *_num_written = 1<<_q->num_stages;

    for (i=0; i<(1<<_q->num_stages); i++)
        _y[i] = x1[i];
}

