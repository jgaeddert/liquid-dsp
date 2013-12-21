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
    unsigned int num_stages;    // number of half-band stages
    float        fc;            // composite cut-off frequency
    float        f0;            // composite center frequency
    float        As;            // composite stop-band attenuation

    // half-band resamplers
    RESAMP2() * resamp2;        // array of half-band resamplers
    T * buffer0;                // buffer[0]
    T * buffer1;                // buffer[1]
    unsigned int buffer_index;  // index of buffer
    float zeta;                 // scaling factor
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
    } else if (_num_stages > 10) {
        fprintf(stderr,"error: msresamp2_%s_create(), number of stages should not exceed 10\n", EXTENSION_FULL);
        exit(1);
    }

    // ensure cut-off frequency is valid
    if ( _fc > 0.5f / (float)(1<<_num_stages) ) {
        fprintf(stderr,"warning: msresamp2_%s_create(), cut-off frequency out of range relative to resampling rate\n", EXTENSION_FULL);
        fprintf(stderr,"         (fc = %12.8f, maximum = %12.8f, setting to %12.8f)\n",
                                 _fc, 0.5f / (float)(1<<_num_stages), 0.25f / (float)(1<<_num_stages));
        _fc = 0.25f / (float)(1<<_num_stages);
    }

    // check center frequency
    if ( _f0 != 0. ) {
        fprintf(stderr,"warning: msresamp2_%s_create(), non-zero center frequency not yet supported\n", EXTENSION_FULL);
        _f0 = 0.;
    }

    // create object
    MSRESAMP2() q = (MSRESAMP2()) malloc(sizeof(struct MSRESAMP2(_s)));

    // set internal properties
    q->num_stages = _num_stages;
    q->fc         = _fc;
    q->f0         = _f0;
    q->As         = _As;

    // allocate memory for buffers
    q->buffer0 = (T*) malloc( (1 << q->num_stages)*sizeof(T) );
    q->buffer1 = (T*) malloc( (1 << q->num_stages)*sizeof(T) );

    // create half-band resampler objects
    q->resamp2 = (RESAMP2()*) malloc(q->num_stages*sizeof(RESAMP2()));
    unsigned int i;
    for (i=0; i<q->num_stages; i++) {
#if 0
        // TODO: compute length based on filter requirements
        // comptue required filter semi-length, h_len = 4*m + 1
        float h_len = estimate_req_filter_len_Herrmann(df, q->As);
        float m_hat = (h_len-1.0f)/4.0f;
        unsigned int m = m_hat < 2.0f ? 2 : (unsigned int)m_hat;
#else
        // for now just fix half-band filter length
        unsigned int m = 3;
#endif

        // create half-band resampler
        q->resamp2[i] = RESAMP2(_create)(m, 0, q->As);
    }

    // reset object
    MSRESAMP2(_reset)(q);

    // return main object
    return q;
}

// destroy msresamp2 object, freeing all internally-allocated memory
void MSRESAMP2(_destroy)(MSRESAMP2() _q)
{
    // free buffers
    free(_q->buffer0);
    free(_q->buffer1);

    // destroy/free half-band resampler objects
    unsigned int i;
    for (i=0; i<_q->num_stages; i++)
        RESAMP2(_destroy)(_q->resamp2[i]);

    // free half-band resampler array
    free(_q->resamp2);

    // destroy main object
    free(_q);
}

// print msresamp2 object internals
void MSRESAMP2(_print)(MSRESAMP2() _q)
{
    printf("multi-stage half-band resampler:\n");
    printf("    number of stages        : %u stages\n",   _q->num_stages);
    printf("    cut-off frequency, fc   : %12.8f / Fs\n", _q->fc);
    printf("    center frequency, f0    : %12.8f / Fs\n", _q->f0);
    printf("    stop-band attenuation   : %.2f dB\n",     _q->As);

    // TODO: print each stage
    unsigned int i;
    for (i=0; i<_q->num_stages; i++)
        RESAMP2(_print)(_q->resamp2[i]);
}

// reset msresamp2 object internals, clear filters and nco phase
void MSRESAMP2(_reset)(MSRESAMP2() _q)
{
    // reset half-band resampler objects
    /*
    unsigned int i;
    for (i=0; i<_q->num_halfband_stages; i++)
        RESAMP2(_clear)(_q->resamp2[i]);
    */

    // reset buffer write pointer
    _q->buffer_index = 0;
    
    // TODO: clear internal buffers?
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

