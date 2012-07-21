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
// mdctch.c
//
// modified discrete cosine transform channelizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_MDCTCH            1

struct mdctch_s {
    unsigned int M;         // number of channels
    int type;               // synthesizer/analyzer

    // transform buffers
    float * x;              // input transform buffer
    float * y;              // output transform buffer
    float * w;              // transform window

    // transform object specifiers
    int mdct_type;          // transform type
    int mdct_flags;         // transform flags
    fftplan mdct;           // transform object
};

mdctch mdctch_create(unsigned int _num_channels,
                     int _type,
                     int _wtype,
                     float _beta)
{
    // validate input

    mdctch q = (mdctch) malloc(sizeof(struct mdctch_s));

    q->M = _num_channels;
    q->type = _type;

    // allocate buffers and create transform
    q->x = (float*) malloc( (2*q->M)*sizeof(float) );
    q->y = (float*) malloc( (2*q->M)*sizeof(float) );
    q->w = (float*) malloc( (2*q->M)*sizeof(float) );

    // initialize window
    unsigned int i;
    for (i=0; i<2*q->M; i++) {
#if 0
        // shaped pulse
        float t0 = sinf(M_PI/(2*q->M)*(i+0.5));
        q->w[i] = sinf(M_PI*0.5f*t0*t0);
#else
        q->w[i] = liquid_kbd(i,2*q->M,10.0f);
#endif
    }

    // create transform
    q->mdct_type = (q->type == LIQUID_ANALYZER) ? FFT_MDCT : FFT_IMDCT;
    q->mdct_flags = 0;
    q->mdct = fft_create_plan_mdct(q->M, q->x, q->y, q->mdct_type, q->mdct_flags);

    // reset object
    mdctch_clear(q);

    return q;
}

void mdctch_destroy(mdctch _q)
{
    // free internal buffers
    free(_q->x);
    free(_q->y);
    free(_q->w);

    // destroy transform object
    fft_destroy_plan(_q->mdct);

    // free main object memory
    free(_q);
}

void mdctch_clear(mdctch _q)
{
    // clear internal buffers
    unsigned int i;
    for (i=0; i<2*_q->M; i++) {
        _q->x[i] = 0.0f;
        _q->y[i] = 0.0f;
    }
}

void mdctch_execute(mdctch _q, 
                    float * _x,
                    float * _y)
{
    if (_q->type == LIQUID_ANALYZER)
        mdctch_execute_analyzer(_q, _x, _y);
    else
        mdctch_execute_synthesizer(_q, _x, _y);
}

void mdctch_execute_analyzer(mdctch _q,
                             float * _x,
                             float * _y)
{
    unsigned int M = _q->M;

    // copy last half of buffer to first half
    memmove(_q->x, &_q->x[M], M*sizeof(float));

    // copy input block to last half of buffer
    memmove(&_q->x[M], _x, M*sizeof(float));

    // run transform
    mdct(_q->x, _q->y, _q->w, M);

    // copy result to output
    memmove(_y, _q->y, M);
}

void mdctch_execute_synthesizer(mdctch _q,
                                float * _y,
                                float * _x)
{
    //unsigned int M = _q->M;
}

