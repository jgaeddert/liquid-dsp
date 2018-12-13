/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
    liquid_resamp_type type;    // resampler type (LIQUID_RESAMP_INTERP, LIQUID_RESAMP_DECIM)
    unsigned int    num_stages; // number of half-band stages
    float           fc;         // initial cut-off frequency
    float           f0;         // initial center frequency
    float           As;         // stop-band attenuation

    // derived values
    unsigned int    M;          // integer resampling rate: 2^num_stages

    // half-band resamplers
    float *         fc_stage;   // cut-off frequency for each stage
    float *         f0_stage;   // center frequency for each stage
    float *         As_stage;   // stop-band attenuation for each stage
    unsigned int *  m_stage;    // filter semi-length for each stage
    RESAMP2() *     resamp2;    // array of half-band resamplers
    T *             buffer0;    // buffer[0]
    T *             buffer1;    // buffer[1]
    unsigned int    buffer_index;  // index of buffer
    float           zeta;       // scaling factor
};

// execute multi-stage resampler as interpolator
//  _q      : msresamp object
//  _x      : input sample
//  _y      : output sample array  [size:2^_num_stages x 1]
void MSRESAMP2(_interp_execute)(MSRESAMP2() _q,
                                TI          _x,
                                TO *        _y);

// execute multi-stage resampler as decimator
//  _q      : msresamp object
//  _x      : input sample array  [size: 2^_num_stages x 1]
//  _y      : output sample pointer
void MSRESAMP2(_decim_execute)(MSRESAMP2() _q,
                               TI *        _x,
                               TO *        _y);

// create multi-stage half-band resampler
//  _type       : resampler type (e.g. LIQUID_RESAMP_DECIM)
//  _num_stages : number of resampling stages
//  _fc         : filter cut-off frequency 0 < _fc < 0.5
//  _f0         : filter center frequency
//  _As         : stop-band attenuation [dB]
MSRESAMP2() MSRESAMP2(_create)(int          _type,
                               unsigned int _num_stages,
                               float        _fc,
                               float        _f0,
                               float        _As)
{
    // validate input
    if (_num_stages > 16) {
        fprintf(stderr,"error: msresamp2_%s_create(), number of stages should not exceed 16\n", EXTENSION_FULL);
        exit(1);
    }

    // ensure cut-off frequency is valid
    if ( _fc <= 0.0f || _fc >= 0.5f ) {
        fprintf(stderr,"error: msresamp2_%s_create(), cut-off frequency must be in (0,0.5)\n", EXTENSION_FULL);
        exit(1);
    } else if ( _fc > 0.499f ) {
        fprintf(stderr,"warning: msresamp2_%s_create(), cut-off frequency greater than 0.499\n", EXTENSION_FULL);
        fprintf(stderr,"    >> truncating to 0.499\n");
        _fc = 0.499f;
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
    q->type       = _type == LIQUID_RESAMP_INTERP ? LIQUID_RESAMP_INTERP : LIQUID_RESAMP_DECIM;
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

    // allocate arrays for half-band resampler parameters
    q->fc_stage = (float*)        malloc(q->num_stages*sizeof(float)       );
    q->f0_stage = (float*)        malloc(q->num_stages*sizeof(float)       );
    q->As_stage = (float*)        malloc(q->num_stages*sizeof(float)       );
    q->m_stage  = (unsigned int*) malloc(q->num_stages*sizeof(unsigned int));

    // determine half-band resampler parameters
    float fc = q->fc;   // cut-off frequency
    float f0 = q->f0;   // center frequency
    for (i=0; i<q->num_stages; i++) {
        // compute parameters based on filter requirements;
        fc = (i==1) ? (0.5-fc)/2.0f : 0.5f*fc;  // cut-off frequency
        f0 = 0.5f*f0;                           // center frequency
        float ft = 2*(0.25f - fc);              // two-sided transition bandwidth

        // estimate required filter length
        unsigned int h_len = estimate_req_filter_len(ft, q->As);
        unsigned int m = ceilf( (float)(h_len-1) / 4.0f );

        //printf(" >>> fc: %8.6f, ft: %8.6f, h_len : %u (m=%u)\n", fc, ft, h_len, m);
        q->fc_stage[i] = fc;            // filter pass-band
        q->f0_stage[i] = f0;            // filter center frequency
        q->As_stage[i] = q->As;         // filter stop-band attenuation
        q->m_stage[i]  = m < 3 ? 3 : m; // minimum 3
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
    printf("    type                    : %s\n", _q->type == LIQUID_RESAMP_DECIM ? "decimator" : "interpolator");
    printf("    number of stages        : %u stage%s\n", _q->num_stages, _q->num_stages == 1 ? "" : "s");
    printf("    cut-off frequency, fc   : %12.8f Fs\n",  _q->fc);
    printf("    center frequency, f0    : %12.8f Fs\n",  _q->f0);
    printf("    stop-band attenuation   : %.2f dB\n",    _q->As);
    printf("    delay (total)           : %.3f samples\n", MSRESAMP2(_get_delay)(_q));

    // print each stage
    unsigned int i;
    for (i=0; i<_q->num_stages; i++) {
        // get index of stage (reversed for decimator)
        unsigned int g = _q->type == LIQUID_RESAMP_INTERP ? i : _q->num_stages-i-1;

        // print stage information
        printf("    stage[%2u]  {m=%3u, As=%6.2f dB, fc=%6.3f, f0=%6.3f}\n",
                    i, _q->m_stage[g], _q->As_stage[g], _q->fc_stage[g], _q->f0_stage[g]);
    }
}

// reset msresamp2 object internals, clear filters and nco phase
void MSRESAMP2(_reset)(MSRESAMP2() _q)
{
    // reset half-band resampler objects
    unsigned int i;
    for (i=0; i<_q->num_stages; i++)
        RESAMP2(_reset)(_q->resamp2[i]);

    // reset buffer write pointer
    _q->buffer_index = 0;
    
    // NOTE: not necessary to clear internal buffers
}

// Get multi-stage half-band resampling rate
float MSRESAMP2(_get_rate)(MSRESAMP2() _q)
{
    return _q->type == LIQUID_RESAMP_INTERP ? (float)(_q->M) : 1.0f/(float)(_q->M);
}

// Get number of half-band resampling stages in object
unsigned int MSRESAMP2(_get_num_stages)(MSRESAMP2() _q)
{
    return _q->num_stages;
}

// Get resampling type (LIQUID_RESAMP_DECIM, LIQUID_RESAMP_INTERP)
int MSRESAMP2(_get_type)(MSRESAMP2() _q)
{
    return _q->type;
}

// get group delay (number of output samples)
float MSRESAMP2(_get_delay)(MSRESAMP2() _q)
{
    // initialize delay
    float delay = 0;
    unsigned int i;

    // add half-band resampling delay
    if (_q->type == LIQUID_RESAMP_INTERP) {
        // interpolator
        for (i=0; i<_q->num_stages; i++) {
            // filter semi-length
            unsigned int m = _q->m_stage[_q->num_stages-i-1];

            delay *= 0.5f;
            delay += m;
        }
    } else {
        // decimator
        for (i=0; i<_q->num_stages; i++) {
            // filter semi-length
            unsigned int m = _q->m_stage[i];

            delay *= 2;
            delay += 2*m - 1;
        }
    }

    return delay;
}

// execute multi-stage resampler as interpolator            
//  _q      : msresamp object                               
//  _x      : input sample                                  
//  _y      : output sample array  [size:2^_num_stages x 1] 
void MSRESAMP2(_execute)(MSRESAMP2() _q,
                         TI *        _x,
                         TO *        _y)
{
    // switch resampling method based on type
    if (_q->num_stages == 0) {
        // pass through
        _y[0] = _x[0];
        return;
    } else if (_q->type == LIQUID_RESAMP_INTERP) {
        // execute multi-stage resampler as interpolator
        MSRESAMP2(_interp_execute)(_q, _x[0], _y);
    } else {
        // execute multi-stage resampler as decimator
        MSRESAMP2(_decim_execute)(_q, _x, _y);
    }
}

//
// internal methods
//

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
        for (i=0; i<k; i++)
            RESAMP2(_interp_execute)(_q->resamp2[s], b0[i], &b1[2*i]);

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
    unsigned int k;         // number of outputs for this stage
    for (s=0; s<_q->num_stages; s++) {
        // compute number of outputs for this stage
        k = 1 << (_q->num_stages - s - 1);

        // run half-band stages as decimators
        unsigned int i;
        unsigned int g = _q->num_stages-s-1;    // reversed resampler index
        for (i=0; i<k; i++)
            RESAMP2(_decim_execute)(_q->resamp2[g], &b0[2*i], &b1[i]);

        // toggle output buffer pointers
        b0 = (s % 2) == 0 ? _q->buffer1 : _q->buffer0;
        b1 = (s % 2) == 0 ? _q->buffer0 : _q->buffer1;
    }

    // set single output sample and scale appropriately
    *_y = b0[0] * _q->zeta;
}

