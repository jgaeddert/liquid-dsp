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
// multi-stage arbitrary resampler
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

// 
// forward declaration of internal methods
//

// execute multi-stage interpolation
void MSRESAMP(_interp_execute)(MSRESAMP()    _q,
                              TI *           _x,
                              unsigned int   _nx,
                              TO *           _y,
                              unsigned int * _num_written);

// execute multi-stage decimation
void MSRESAMP(_decim_execute)(MSRESAMP()     _q,
                              TI *           _x,
                              unsigned int   _nx,
                              TO *           _y,
                              unsigned int * _num_written);


struct MSRESAMP(_s) {
    // user-defined parameters
    float rate;                         // re-sampling rate
    float As;                           // filter stop-band attenuation [dB]

    // type: interpolator or decimator
    int type;                           // run half-band resamplers as interp or decim

    // half-band resampler parameters
    unsigned int num_halfband_stages;   // number of halfband stages
    MSRESAMP2() halfband_resamp;        // multi-stage halfband resampler
    float rate_halfband;                // halfband rate

    // arbitrary resampler parameters
    RESAMP() arbitrary_resamp;          // arbitrary resampling object
    float rate_arbitrary;               // clean-up resampling rate, in (0.5, 2.0)

    // internal buffers (ping-pong)
    unsigned int buffer_len;            // length of each buffer
    T * buffer;                         // buffer[0]
    unsigned int buffer_index;          // index of buffer
};

// create msresamp object
//  TODO: add signal bandwidth parameter?
//  TODO: add center frequency parameter; facilitates DDS object synthesis
//  _r              :   resampling rate [output/input]
//  _As             :   stop-band attenuation
MSRESAMP() MSRESAMP(_create)(float _r,
                             float _As)
{
    // validate input
    if (_r <= 0.0f) {
        fprintf(stderr,"error: msresamp_%s_create(), resampling rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    // create object
    MSRESAMP() q = (MSRESAMP()) malloc(sizeof(struct MSRESAMP(_s)));

    // set internal properties
    q->rate = _r;       // composite rate
    q->As   = _As;      // stop-band suppression

    // decimation or interpolation?
    q->type = (q->rate > 1.0f) ? LIQUID_RESAMP_INTERP : LIQUID_RESAMP_DECIM;

    // compute derived values
    q->rate_arbitrary = q->rate;
    q->rate_halfband  = 1.0f;
    q->num_halfband_stages = 0;
    switch(q->type) {
        case LIQUID_RESAMP_INTERP:
            while (q->rate_arbitrary > 2.0f) {
                q->num_halfband_stages++;
                q->rate_halfband  *= 2.0f;
                q->rate_arbitrary *= 0.5f;
            }
            break;
        case LIQUID_RESAMP_DECIM:
            while (q->rate_arbitrary < 0.5f) {
                q->num_halfband_stages++;
                q->rate_halfband  *= 0.5f;
                q->rate_arbitrary *= 2.0f;
            }
            break;
        default:;
    }

    // allocate memory for buffer
    q->buffer_len = 4 + (1 << q->num_halfband_stages);
    q->buffer = (T*) malloc( q->buffer_len*sizeof(T) );

    // create single multi-stage half-band resampler object
    // TODO: compute appropriate cut-off frequency
    q->halfband_resamp = MSRESAMP2(_create)(q->type,
                                            q->num_halfband_stages,
                                            0.4f, 0.0f, q->As);

    // create arbitrary resampler object
    // TODO: compute appropriate parameters
    q->arbitrary_resamp = RESAMP(_create)(q->rate_arbitrary, 7, 0.4f, q->As, 64);

    // reset object
    MSRESAMP(_reset)(q);

    // return main object
    return q;
}

// destroy msresamp object, freeing all internally-allocated memory
void MSRESAMP(_destroy)(MSRESAMP() _q)
{
    // free buffer
    free(_q->buffer);

    // destroy arbitrary resampler
    RESAMP(_destroy)(_q->arbitrary_resamp);

    // destroy multi-stage half-band resampler object
    MSRESAMP2(_destroy)(_q->halfband_resamp);

    // destroy main object
    free(_q);
}

// print msresamp object internals
void MSRESAMP(_print)(MSRESAMP() _q)
{
    printf("multi-stage resampler\n");
    printf("    composite rate      : %12.10f\n", _q->rate);
    printf("    type                : %s\n", _q->type == LIQUID_RESAMP_INTERP ? "interp" : "decim");
    printf("    num halfband stages : %u\n", _q->num_halfband_stages);
    printf("    halfband rate       : %s%u\n", _q->type == LIQUID_RESAMP_INTERP ? "" : "1/",
                                                 1<<_q->num_halfband_stages);
    printf("    arbitrary rate      : %12.10f\n", _q->rate_arbitrary);
    printf("    stages:\n");
    
    //float delay_halfband  = MSRESAMP2(_get_delay)(_q->halfband_resamp);
    //float delay_arbitrary = RESAMP(_get_delay)(_q->arbitrary_resamp);

    // print each stage
    unsigned int stage=0;
    float r = 1.0f; // accumulated rate
    
    // arbitrary (interpolator)
    if (_q->type == LIQUID_RESAMP_INTERP) {
        float rate = _q->rate_arbitrary;
        r *= rate;  // update rate
        printf("    [%2u, r=%11.7f] : arbitrary, r=%12.8f\n", stage, r, rate);
        stage++;
    }

    // print stages of half-band resampling
    unsigned int i;
    for (i=0; i<_q->num_halfband_stages; i++) {
        float rate = _q->type == LIQUID_RESAMP_INTERP ? 2.0f : 0.5f;
        r *= rate;
        printf("    [%2u, r=%11.7f] : halfband,  r=%5.1f\n", stage, r, rate);
        stage++;
    }

    // arbitrary (decimator)
    if (_q->type == LIQUID_RESAMP_DECIM) {
        float rate = _q->rate_arbitrary;
        r *= rate;
        printf("    [%2u, r=%11.7f] : arbitrary, r=%12.8f\n", stage, r, rate);
        stage++;
    }
}

// reset msresamp object internals, clear filters and nco phase
void MSRESAMP(_reset)(MSRESAMP() _q)
{
    // reset multi-stage half-band resampler object
    MSRESAMP2(_reset)(_q->halfband_resamp);

    // reset arbitrary resampler
    RESAMP(_reset)(_q->arbitrary_resamp);

    // reset buffer write pointer
    _q->buffer_index = 0;

    // TODO: clear internal buffers?
}

// get filter delay (output samples)
float MSRESAMP(_get_delay)(MSRESAMP() _q)
{
    float delay_halfband = MSRESAMP2(_get_delay)(_q->halfband_resamp);
    float delay_arbitrary = RESAMP(_get_delay)(_q->arbitrary_resamp);

#if 0
    printf("halfband    : rate=%12.8f, delay=%12.8f\n", (float)(1<<_q->num_halfband_stages), delay_halfband);
    printf("arbitrary   : rate=%12.8f, delay=%12.8f\n", _q->rate_arbitrary, delay_arbitrary);
#endif

    // compute delay based on interpolation or decimation type
    if (_q->num_halfband_stages == 0) {
        // no half-band stages; just arbitrary resampler delay
        return delay_arbitrary;
    } else if (_q->type == LIQUID_RESAMP_INTERP) {
        // interpolation
        return delay_halfband / _q->rate_arbitrary + delay_arbitrary;
    } else {
        // decimation
        unsigned int M = 1 << _q->num_halfband_stages;
        return delay_halfband + M*delay_arbitrary;
    }

    return 0.0f;
}

// execute multi-stage resampler
//  _q      :   msresamp object
//  _x      :   input sample array
//  _y      :   output sample array
//  _ny     :   number of samples written to _y
void MSRESAMP(_execute)(MSRESAMP()     _q,
                        TI *           _x,
                        unsigned int   _nx,
                        TO *           _y,
                        unsigned int * _ny)
{
    switch(_q->type) {
    case LIQUID_RESAMP_INTERP:
        MSRESAMP(_interp_execute)(_q, _x, _nx, _y, _ny);
        break;
    case LIQUID_RESAMP_DECIM:
        MSRESAMP(_decim_execute)(_q, _x, _nx, _y, _ny);
        break;
    default:;
    }
}

// 
// internal methods
//

// execute multi-stage resampler as interpolator
//  _q      :   msresamp object
//  _x      :   input sample array
//  _y      :   output sample array
//  _nw     :   number of samples written to _y
void MSRESAMP(_interp_execute)(MSRESAMP()     _q,
                               TI *           _x,
                               unsigned int   _nx,
                               TO *           _y,
                               unsigned int * _ny)
{
    unsigned int i;
    unsigned int nw;
    unsigned int ny = 0;

    // operate one sample at a time so that we don't overflow the internal
    // buffer
    for (i=0; i<_nx; i++) {
        // run arbitrary resampler
        RESAMP(_execute)(_q->arbitrary_resamp, _x[i], _q->buffer, &nw);

        // run multi-stage half-band resampler on each output sample
        unsigned int k;
        for (k=0; k<nw; k++) {
            MSRESAMP2(_execute)(_q->halfband_resamp, &_q->buffer[k], &_y[ny]);

            // increase output counter by halfband interpolation rate
            ny += 1 << _q->num_halfband_stages;
        }
    }

    // set return value for number of samples written
    *_ny = ny;
}

// execute multi-stage resampler as decimator
//  _q      :   msresamp object
//  _x      :   input sample array
//  _y      :   output sample array
//  _nw     :   number of samples written to _y
void MSRESAMP(_decim_execute)(MSRESAMP()     _q,
                              TI *           _x,
                              unsigned int   _nx,
                              TO *           _y,
                              unsigned int * _ny)
{
    unsigned int i;
    unsigned int M = 1 << _q->num_halfband_stages;
    unsigned int nw;        // number of samples written for arbitrary resamp
    unsigned int ny = 0;    // running counter of output samples
    TO halfband_output;     // single half-band decimator output sample

    // write samples to buffer until it contains 2^num_halfband_stages
    // TODO: write block of samples
    for (i=0; i<_nx; i++) {
        // push sample into buffer
        _q->buffer[_q->buffer_index++] = _x[i];

        // check if buffer has 'M' elements
        if (_q->buffer_index == M) {
            // run half-band decimation, producing a single output
            MSRESAMP2(_execute)(_q->halfband_resamp, _q->buffer, &halfband_output);

            // run resulting sample through arbitrary resampler
            RESAMP(_execute)(_q->arbitrary_resamp, halfband_output, &_y[ny], &nw);

            // increment output counter
            ny += nw;

            // reset buffer index
            _q->buffer_index = 0;
        }
    }

    // set return value for number of samples written
    *_ny = ny;
}

