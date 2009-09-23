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
    // user-defined parameters
    unsigned int num_stages;    // number of halfband stages
    float fc0;                  // high-rate center frequency (-0.5,0.5)
    float bw0;                  // low-rate bandwidth (range?)
    float slsl0;                // filter sidelobe levels [dB]

    // derived values
    unsigned int rate;  // re-sampling rate (2^num_stages)

    // halfband decimation/interpolation stages
    RESAMP2() * halfband_resamp;
    float * fc;             // filter center frequency
    float * bw;             // filter bandwidth
    float * slsl;           // filter sidelobe suppression level
    unsigned int * h_len;   // filter length

    // TODO : this buffering method is confusing; clean it up
    T * buffer;
    unsigned int buffer_len;
    T ** b;

    // low-rate mixing stage
    nco ncox;
};

DDS() DDS(_create)(unsigned int _num_stages,// number of halfband stages
                   float _fc,               // input carrier
                   float _bw,               // input signal bandwidth
                   float _slsl)             // sidelobe suppression level
{
    // create object
    DDS() q = (DDS()) malloc(sizeof(struct DDS(_s)));
    q->num_stages = _num_stages;
    q->rate = 1<<(q->num_stages);
    q->fc0 = _fc;
    q->bw0 = _bw;
    q->slsl0 = _slsl;

    // error checking
    if (q->fc0 > 0.5f || q->fc0 < -0.5f) {
        printf("error: dds_xxxf_create(), frequency %12.4e is out of range (-0.5,0.5)\n", q->fc0);
        exit(0);
    }

    // allocate memory for filter properties
    q->fc    = (float*) malloc((q->num_stages)*sizeof(float));
    q->bw    = (float*) malloc((q->num_stages)*sizeof(float));
    q->slsl  = (float*) malloc((q->num_stages)*sizeof(float));
    q->h_len = (unsigned int*) malloc((q->num_stages)*sizeof(unsigned int));
    unsigned int i;
    float fc, fd;
    fc = 0.5*(1<<q->num_stages)*q->fc0;
    fd = 0.3;
    for (i=0; i<q->num_stages; i++) {
        q->fc[i] = fc;
        while (q->fc[i] >  0.5f) q->fc[i] -= 1.0f;
        while (q->fc[i] < -0.5f) q->fc[i] += 1.0f;
        q->bw[i] = 1-fd;
        q->slsl[i] = q->slsl0;
        q->h_len[i] = i==0 ? 37 : q->h_len[i-1]*0.6;
        fc *= 0.5f;
        fd *= 0.5f;
    }

    // allocate memory for buffering
    q->buffer_len = 1<<(q->num_stages+1);
    printf("buffer length : %u\n", q->buffer_len);
    q->buffer = (T*) malloc((q->buffer_len)*sizeof(T));

    q->b = (T**) malloc((q->num_stages+1)*sizeof(T*));
    unsigned int k, n=0;
    //if (q->is_interp) {
        k = 1;
        for (i=0; i<q->num_stages+1; i++) {
            printf("n : %u\n", n);
            q->b[i] = q->buffer + n;
            n += k;
            k <<= 1;
        }
#if 0
    } else {
        k = 1<<q->num_stages;
        for (i=0; i<q->num_stages+1; i++) {
            printf("n : %u\n", n);
            q->b[i] = q->buffer + n;
            n += k;
            k >>= 1;
        }
    }
#endif

    // TODO : compute resampler parameters
    // TODO : generate resamplers

    // allocate memory for resampler pointers
    q->halfband_resamp = (RESAMP2()*) malloc((q->num_stages)*sizeof(RESAMP()*));
    for (i=0; i<q->num_stages; i++) {
        q->halfband_resamp[i] = RESAMP2(_create)(q->h_len[i],
                                                 q->fc[i],
                                                 q->slsl[i]);
    }

    // create NCO
    q->ncox = nco_create(LIQUID_VCO);
    nco_set_frequency(q->ncox, 2*M_PI*(1<<q->num_stages)*q->fc0);

    return q;
}

void DDS(_destroy)(DDS() _q)
{
    free(_q->h_len);
    free(_q->fc);
    free(_q->bw);

    // destroy buffer, buffer pointers
    free(_q->buffer);
    free(_q->b);

    // destroy halfband resampler objects
    unsigned int i;
    for (i=0; i<_q->num_stages; i++)
        RESAMP2(_destroy)(_q->halfband_resamp[i]);
    free(_q->halfband_resamp);

    // destroy NCO object
    nco_destroy(_q->ncox);

    // destroy DDS object
    free(_q);
}

void DDS(_print)(DDS() _q)
{
    printf("direct digital synthesizer (dds), rate : %u\n", _q->rate);
    printf("      fc : %8.5f\n", _q->fc0);
    printf("    halfband stages : %3u %s, rate : %12.4e\n",
                    _q->num_stages,
                    //_q->is_interp ? "interp" : "decim ",
                    "interp",
                    0.0f);
    unsigned int i;
    for (i=0; i<_q->num_stages; i++) {
        printf("      [%3u] : fc = %8.5f, bw = %8.5f, %3u taps\n",
                    i,
                    _q->fc[i],
                    _q->bw[i],
                    _q->h_len[i]);
        //RESAMP2(_print)(_q->halfband_resamp[i]);
    }
}

void DDS(_reset)(DDS() _q)
{
    // reset internal filter state variables
    unsigned int i;
    for (i=0; i<_q->num_stages; i++) {
        RESAMP2(_clear)(_q->halfband_resamp[i]);
    }

    nco_reset(_q->ncox);
}

// execute decimator
void DDS(_decim_execute)(DDS() _q,
                         T * _x,
                         T * _y)
{
    // TODO : decimator needs to wait until enough samples are written into the buffer
}

// execute interpolator
void DDS(_interp_execute)(DDS() _q,
                          T _x,
                          T * _y)
{
    // TODO : execute arbitrary resampler

    // TODO : increment NCO
    nco_mix_up(_q->ncox, _x, &_x);

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

    for (i=0; i<(1<<_q->num_stages); i++)
        _y[i] = x1[i];
}

