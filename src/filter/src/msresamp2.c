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

    // derived values
    unsigned int M;             // integer resampling rate: 2^num_stages

    // half-band resamplers
    float * fc_stage;           // cut-off frequency for each stage
    float * f0_stage;           // center frequency for each stage
    float * As_stage;           // stop-band attenuation for each stage
    unsigned int * m_stage;     // filter semi-length for each stage
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

    unsigned int i;

    // create object
    MSRESAMP2() q = (MSRESAMP2()) malloc(sizeof(struct MSRESAMP2(_s)));

    // set internal properties
    q->num_stages = _num_stages;
    q->fc         = _fc;
    q->f0         = _f0;
    q->As         = _As;

    // derived values
    q->M    = 1 << q->num_stages;
    q->zeta = 1.0f / (float)(q->M);

    // allocate memory for buffers
    q->buffer0 = (T*) malloc( q->M * sizeof(T) );
    q->buffer1 = (T*) malloc( q->M * sizeof(T) );

    // determine half-band resampler parameters
    q->fc_stage = (float*)        malloc(q->num_stages*sizeof(float)       );
    q->f0_stage = (float*)        malloc(q->num_stages*sizeof(float)       );
    q->As_stage = (float*)        malloc(q->num_stages*sizeof(float)       );
    q->m_stage  = (unsigned int*) malloc(q->num_stages*sizeof(unsigned int));
    for (i=0; i<q->num_stages; i++) {
        // TODO: compute parameters based on filter requirements;
        //       for now just fix parameters
        q->fc_stage[i] = 0.25f;
        q->f0_stage[i] = 0.0f;
        q->As_stage[i] = 60.0f;
        q->m_stage[i]  = 3 + i;
    }

    // create half-band resampler objects
    q->resamp2 = (RESAMP2()*) malloc(q->num_stages*sizeof(RESAMP2()));
    for (i=0; i<q->num_stages; i++) {
        // create half-band resampler
        q->resamp2[i] = RESAMP2(_create)(q->m_stage[i],
                                         q->f0_stage[i],
                                         q->As_stage[i]);
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

    // free half-band resampler design parameter arrays
    free(_q->fc_stage);
    free(_q->f0_stage);
    free(_q->As_stage);
    free(_q->m_stage);

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

    // print each stage
    unsigned int i;
    for (i=0; i<_q->num_stages; i++) {
        printf("    stage[%2u]  {m=%3u, As=%6.2f dB, fc=%6.3f, f0=%6.3f}\n",
                    i, _q->m_stage[i], _q->As_stage[i], _q->fc_stage[i], _q->f0_stage[i]);
    }
}

// reset msresamp2 object internals, clear filters and nco phase
void MSRESAMP2(_reset)(MSRESAMP2() _q)
{
    // reset half-band resampler objects
    unsigned int i;
    for (i=0; i<_q->num_stages; i++)
        RESAMP2(_clear)(_q->resamp2[i]);

    // reset buffer write pointer
    _q->buffer_index = 0;
    
    // NOTE: not necessary to clear internal buffers
}

// get filter delay (output samples)
float MSRESAMP2(_get_delay)(MSRESAMP2() _q)
{
    return 0.0f;
}

// execute multi-stage resampler as interpolator            
//  _q      : msresamp object                               
//  _x      : input sample                                  
//  _y      : output sample array  [size:2^_num_stages x 1] 
void MSRESAMP2(_interp_execute)(MSRESAMP2() _q,
                                TI          _x,
                                TO *        _y)
{
    // buffer pointers (initialize BOTH to _q->buffer0);
    T * b0 = _q->buffer0;   // input buffer pointer
    T * b1 = _q->buffer1;   // output buffer pointer

    // set input sample in first buffer
    b0[0] = _x;

    unsigned int s;         // half-band decimator stage counter
    unsigned int k;         // number of inputs for this stage
    for (s=0; s<_q->num_stages; s++) {
        // compute number of inputs for this stage
        k = 1 << s;

        // set final stage output as supplied output pointer
        if (s == _q->num_stages-1)
            b1 = _y;

        // run half-band stages as interpolators
        unsigned int i;
        unsigned int g = _q->num_stages-s-1;    // reversed resampler index
        for (i=0; i<k; i++)
            RESAMP2(_interp_execute)(_q->resamp2[g], b0[i], &b1[2*i]);

        // toggle output buffer pointers
        b0 = (s % 2) == 0 ? _q->buffer1 : _q->buffer0;
        b1 = (s % 2) == 0 ? _q->buffer0 : _q->buffer1;
    }
}

// execute multi-stage resampler as decimator               
//  _q      : msresamp object                               
//  _x      : input sample array  [size: 2^_num_stages x 1] 
//  _y      : output sample pointer                         
void MSRESAMP2(_decim_execute)(MSRESAMP2() _q,
                               TI *        _x,
                               TO *        _y)
{
    // buffer pointers (initialize BOTH to _q->buffer0);
    T * b0 = _x;            // input buffer pointer
    T * b1 = _q->buffer1;   // output buffer pointer

    unsigned int s;         // half-band decimator stage counter
    unsigned int k;         // number of inputs for this stage
    for (s=0; s<_q->num_stages; s++) {
        // compute number of outputs for this stage
        k = 1 << (_q->num_stages - s - 1);

        // run half-band stages as decimators
        unsigned int i;
        for (i=0; i<k; i++)
            RESAMP2(_decim_execute)(_q->resamp2[s], &b0[2*i], &b1[i]);

        // toggle output buffer pointers
        b0 = (s % 2) == 0 ? _q->buffer1 : _q->buffer0;
        b1 = (s % 2) == 0 ? _q->buffer0 : _q->buffer1;
    }

    // set single output sample and scale appropriately
    *_y = b0[0] * _q->zeta;
}

