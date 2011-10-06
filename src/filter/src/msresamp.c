/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// multi-stage arbitrary resampler
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

struct MSRESAMP(_s) {
    // user-defined parameters
    float rate;                         // re-sampling rate
    float As;                           // filter stop-band attenuation [dB]

    // half-band resampler parameters
    unsigned int num_halfband_stages;   // number of halfband stages
    enum {
        MSRESAMP_HALFBAND_INTERP=0,
        MSRESAMP_HALFBAND_DECIM
    } halfband_type;                    // run half-band resamplers as interp or decim
    RESAMP2() * halfband_resamp;        // halfband decimation/interpolation objects
    float rate_halfband;                // halfband rate

    // arbitrary resampler parameters
    RESAMP() arbitrary_resamp;          // arbitrary resampling object
    float rate_arbitrary;               // clean-up resampling rate, in (0.5, 2.0)

    // internal buffers
    unsigned int buffer_len;
    T * buffer0;
    T * buffer1;
    unsigned int buffer_index;

    // scaling factor
    float zeta;
};

// create msresamp object
//  _r              :   resampling rate [output/input]
//  _As             :   stop-band attenuation
MSRESAMP() MSRESAMP(_create)(float _r,
                             float _As)
{
    // validate input
    if (_r <= 0.0f) {
        fprintf(stderr,"error: msresamp_xxxf_create(), resampling rate must be greater than zero\n");
        exit(1);
    }

    // create object
    MSRESAMP() q = (MSRESAMP()) malloc(sizeof(struct MSRESAMP(_s)));
    q->rate = _r;
    q->As   = _As;

    // decimation or interpolation?
    q->halfband_type = (q->rate > 1.0f) ? MSRESAMP_HALFBAND_INTERP : MSRESAMP_HALFBAND_DECIM;

    // compute derived values
    q->rate_arbitrary = q->rate;
    q->rate_halfband  = 1.0f;
    q->num_halfband_stages = 0;
    switch(q->halfband_type) {
        case MSRESAMP_HALFBAND_INTERP:
            while (q->rate_arbitrary > 2.0f) {
                q->num_halfband_stages++;
                q->rate_halfband  *= 2.0f;
                q->rate_arbitrary *= 0.5f;
            }
            break;
        case MSRESAMP_HALFBAND_DECIM:
            while (q->rate_arbitrary < 0.5f) {
                q->num_halfband_stages++;
                q->rate_halfband  *= 0.5f;
                q->rate_arbitrary *= 2.0f;
            }
            break;
        default:;
    }

    // allocate memory for buffers
    q->buffer_len = 1<<q->num_halfband_stages;
    q->buffer0 = (T*) malloc( q->buffer_len*sizeof(T) );
    q->buffer1 = (T*) malloc( q->buffer_len*sizeof(T) );

    // create half-band resampler objects
    q->halfband_resamp = (RESAMP2()*) malloc(q->num_halfband_stages*sizeof(RESAMP()));
    unsigned int i;
    for (i=0; i<q->num_halfband_stages; i++) {
        // TODO : compute length based on filter requirements
        unsigned int m = 3;
        q->halfband_resamp[i] = RESAMP2(_create)(m, 0, q->As);
    }

    // create arbitrary resampler object
    q->arbitrary_resamp = RESAMP(_create)(q->rate_arbitrary, 7, 0.4f, q->As, 64);

    // set scaling factor
    q->zeta = (q->halfband_type == MSRESAMP_HALFBAND_INTERP) ? 1.0f : q->rate_halfband;

    // reset object
    MSRESAMP(_reset)(q);

    // return main object
    return q;
}

// destroy msresamp object, freeing all internally-allocated memory
void MSRESAMP(_destroy)(MSRESAMP() _q)
{
    // free buffers
    free(_q->buffer0);
    free(_q->buffer1);

    // destroy arbitrary resampler
    RESAMP(_destroy)(_q->arbitrary_resamp);

    // destroy/free half-band resampler objects
    unsigned int i;
    for (i=0; i<_q->num_halfband_stages; i++)
        RESAMP2(_destroy)(_q->halfband_resamp[i]);
    free(_q->halfband_resamp);

    // destroy main object
    free(_q);
}

// print msresamp object internals
void MSRESAMP(_print)(MSRESAMP() _q)
{
    printf("multi-stage resampler, rate : %f\n", _q->rate);
    printf("    type                :   %s\n", _q->halfband_type == MSRESAMP_HALFBAND_INTERP ? "interp" : "decim");
    printf("    num halfband stages :   %u\n", _q->num_halfband_stages);
    printf("    halfband rate       :   %s%u\n", _q->halfband_type == MSRESAMP_HALFBAND_INTERP ? "" : "1/",
                                                 1<<_q->num_halfband_stages);
    printf("    arbitrary rate      :   %12.10f\n", _q->rate_arbitrary);
    //printf("    buffer len          :   %u\n", _q->buffer_len);
}

// reset msresamp object internals, clear filters and nco phase
void MSRESAMP(_reset)(MSRESAMP() _q)
{
    // reset half-band resampler objects
    unsigned int i;
    for (i=0; i<_q->num_halfband_stages; i++)
        RESAMP2(_clear)(_q->halfband_resamp[i]);

    // reset arbitrary resampler
    RESAMP(_reset)(_q->arbitrary_resamp);

    // reset buffer write pointer
    _q->buffer_index = 0;
}

// execute multi-stage resampler
//  _q      :   msresamp object
//  _x      :   input sample array
//  _y      :   output sample array
//  _ny     :   number of samples written to _y
void MSRESAMP(_execute)(MSRESAMP() _q,
                        TI * _x,
                        unsigned int _nx,
                        TO * _y,
                        unsigned int * _ny)
{
    switch(_q->halfband_type) {
    case MSRESAMP_HALFBAND_INTERP:
        MSRESAMP(_interp_execute)(_q, _x, _nx, _y, _ny);
        break;
    case MSRESAMP_HALFBAND_DECIM:
        MSRESAMP(_decim_execute)(_q, _x, _nx, _y, _ny);
        break;
    default:;
    }
}

// get filter delay (output samples)
float MSRESAMP(_get_delay)(MSRESAMP() _q)
{
    // initialize delay
    float delay = 0;
    unsigned int i;

    if (_q->halfband_type == MSRESAMP_HALFBAND_DECIM) {
        // add half-band resampling delay
        for (i=0; i<_q->num_halfband_stages; i++) {
            // TODO : update when length changes for filter design requirements
            unsigned int m = 3; // filter semi-length
            delay += 2*m;
            delay *= 2;
        }

        // add arbitrary resampler delay
        delay += 7;
    } else {
        // TODO : check this
        // add arbitrary resampler delay
        delay += 7;
        
        // add half-band resampling delay
        for (i=0; i<_q->num_halfband_stages; i++) {
            // TODO : update when length changes for filter design requirements
            unsigned int m = 3; // filter semi-length
            delay *= 0.5f;
            delay += 2*m;
        }
    }

    return delay;
}

// 
// internal methods
//

// execute multi-stage resampler as interpolator
//  _q      :   msresamp object
//  _x      :   input sample array
//  _y      :   output sample array
//  _nw     :   number of samples written to _y
void MSRESAMP(_interp_execute)(MSRESAMP() _q,
                               TI * _x,
                               unsigned int _nx,
                               TO * _y,
                               unsigned int * _ny)
{
    // set output size to zero
    *_ny = 0;
    
    unsigned int k;     // number of inputs for this stage
    unsigned int s;     // half-band decimator stage counter
    unsigned int n;     // input counter
    unsigned int g;     // half-band resampler stage index (reversed)

    // buffer pointers (initialize BOTH to _q->buffer0);
    T * b0 = _q->buffer0;   // input buffer pointer
    T * b1 = _q->buffer0;   // output buffer pointer
    
    unsigned int nw;
    unsigned int nw_total=0;

    unsigned int j;     // 
    T resamp_buffer[4]; // arbitrary resampling buffer

    // write samples to buffer until full
    unsigned int i;
    for (i=0; i<_nx; i++) {
        // execute arbitrary resampler
        RESAMP(_execute)(_q->arbitrary_resamp, _x[i]*_q->zeta, resamp_buffer, &nw);

        // check if buffer is full
        for (j=0; j<nw; j++) {
            // set initial buffer value
            _q->buffer0[0] = resamp_buffer[j];
    
            // number of inputs for this stage
            k = 1;

            // run half-band decimation stage
            for (s=0; s<_q->num_halfband_stages; s++) {
                // set buffer pointers
                b0 = (s%2) == 0 ? _q->buffer0 : _q->buffer1;    // input buffer
                b1 = (s%2) == 1 ? _q->buffer0 : _q->buffer1;    // output buffer

                // execute half-band interpolator
                g = _q->num_halfband_stages - s - 1;
                for (n=0; n<k; n++)
                    RESAMP2(_interp_execute)(_q->halfband_resamp[s], b0[n], &b1[2*n]);

                // length doubles with each iteration
                k <<= 1;
            }

            // copy output data and increment counter
            // NOTE : k = 2^(num_halfband_stages)
            memmove(&_y[nw_total], b1, k*sizeof(TO));
            nw_total += k;
        }
    }

    *_ny = nw_total;
}

// execute multi-stage resampler as decimator
//  _q      :   msresamp object
//  _x      :   input sample array
//  _y      :   output sample array
//  _nw     :   number of samples written to _y
void MSRESAMP(_decim_execute)(MSRESAMP() _q,
                              TI * _x,
                              unsigned int _nx,
                              TO * _y,
                              unsigned int * _ny)
{
    // set output size to zero
    *_ny = 0;
    
    unsigned int k;     // number of inputs for this stage
    unsigned int s;     // half-band decimator stage counter
    unsigned int n;     // input counter
    unsigned int g;     // half-band resampler stage index (reversed)

    // buffer pointers (initialize BOTH to _q->buffer0);
    T * b0 = _q->buffer0;   // input buffer pointer
    T * b1 = _q->buffer0;   // output buffer pointer
    
    unsigned int nw;    // 
    unsigned int nw_total=0;

    // write samples to buffer until full
    unsigned int i;
    for (i=0; i<_nx; i++) {
        // push sample into buffer
        _q->buffer0[_q->buffer_index++] = _x[i];

        // check if buffer is full
        if (_q->buffer_index == _q->buffer_len) {
            // reset counter and run half-band decimators
            _q->buffer_index = 0;
    
            // number of inputs for this stage
            k = 1 << _q->num_halfband_stages;

            // run half-band decimation stage
            for (s=0; s<_q->num_halfband_stages; s++) {
                // length halves with each iteration
                k >>= 1;

                // set buffer pointers
                b0 = (s%2) == 0 ? _q->buffer0 : _q->buffer1;    // input buffer
                b1 = (s%2) == 1 ? _q->buffer0 : _q->buffer1;    // output buffer

                // execute half-band decimator
                g = _q->num_halfband_stages - s - 1;
                for (n=0; n<k; n++)
                    RESAMP2(_decim_execute)(_q->halfband_resamp[g], &b0[2*n], &b1[n]);
            }

            // execute arbitrary resampler
            RESAMP(_execute)(_q->arbitrary_resamp, b1[0]*_q->zeta, &_y[nw_total], &nw);
            nw_total += nw;
        }
    }

    *_ny = nw_total;
}

