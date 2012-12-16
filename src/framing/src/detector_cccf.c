/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
// detector_cccf.c
//
// Pre-demodulation detector
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

struct detector_cccf_s {
    float complex * s;      // sequence
    unsigned int n;         // sequence length

    //
    windowcf buffer;        // input buffer
    wdelayf x2;             // buffer of |x|^2 values
    float x2_xxx;           // ...
    float x2_hat;           // estimate of E{|x|^2}

    // internal correlators
    //dotprod_cccf * dp;
    dotprod_cccf dp;

    // state
    unsigned int timer;     // sample timer
};

// create detector_cccf object
//  _s          :   sequence
//  _n          :   sequence length
//  _threshold  :   detection threshold (default: 0.7)
//  _dphi_max   :   maximum carrier offset
detector_cccf detector_cccf_create(float complex * _s,
                                   unsigned int    _n,
                                   float           _threshold,
                                   float           _dphi_max)
{
    // validate input
    if (_n == 0) {
        fprintf(stderr,"error: detector_cccf_create(), sequence length cannot be zero\n");
        exit(1);
    }
    
    unsigned int i;

    detector_cccf q = (detector_cccf) malloc(sizeof(struct detector_cccf_s));

    // set internal properties
    q->n = _n;

    // allocate memory for sequence and copy
    q->s = (float complex*) malloc((q->n)*sizeof(float complex));
    memmove(q->s, _s, q->n*sizeof(float complex));

    // create internal buffer
    q->buffer = windowcf_create(q->n);
    q->x2     = wdelayf_create(q->n);

    // create internal dot product object
    float complex sconj[q->n];
    for (i=0; i<q->n; i++)
        sconj[i] = conjf(q->s[i]);
    q->dp = dotprod_cccf_create(sconj, q->n);

    // reset state
    detector_cccf_reset(q);

    // return object
    return q;
}

void detector_cccf_destroy(detector_cccf _q)
{
    // destroy input buffer
    windowcf_destroy(_q->buffer);

    // destroy dot product
    dotprod_cccf_destroy(_q->dp);

    // destroy |x|^2 buffer
    wdelayf_destroy(_q->x2);

    // free internal buffers/arrays
    free(_q->s);

    // free main object memory
    free(_q);
}

void detector_cccf_print(detector_cccf _q)
{
    printf("detector_cccf:\n");
    printf("    sequence length     :   %-u\n", _q->n);
    printf("    rssi                :   ? dB\n");
}

void detector_cccf_reset(detector_cccf _q)
{
    // reset internal state
    windowcf_clear(_q->buffer);
    wdelayf_clear(_q->x2);

    // reset internal state
    _q->timer = _q->n;
}

// Run sample through pre-demod detector's correlator.
// Returns '1' if signal was detected, '0' otherwise
//  _q          :   pre-demod detector
//  _x          :   input sample
//  _tau_hat    :   fractional sample offset estimate (set when detected)
//  _dphi_hat   :   carrier frequency offset estimate (set when detected)
//  _gamma_hat  :   channel gain estimate (set when detected)
int detector_cccf_correlate(detector_cccf _q,
                            float complex _x,
                            float *       _tau_hat,
                            float *       _dphi_hat,
                            float *       _gamma_hat)
{
    // push sample into buffer
    windowcf_push(_q->buffer, _x);

    // update estimate of signal magnitude
    float x2_n = crealf(_x * conjf(_x));    // |x[n-1]|^2 (input sample)
    float x2_0;                             // |x[0]  |^2 (oldest sample)
    wdelayf_read(_q->x2, &x2_0);            // read oldest sample
    wdelayf_push(_q->x2, x2_n);             // push newest sample
    _q->x2_xxx = _q->x2_xxx + x2_n - x2_0;  // update...
    _q->x2_hat = 0.9f*_q->x2_hat + 0.1f*(_q->x2_xxx / (float)(_q->n));

    // check state
    if (_q->timer) {
        // hasn't timed out yet
        printf("timer = %u\n", _q->timer);
        _q->timer--;
        return 0;
    }

    // compute vector dot product
    float complex * r;
    float complex rxy;
    windowcf_read(_q->buffer, &r);
    dotprod_cccf_execute(_q->dp, r, &rxy);

    // scale by input signal magnitude
    // TODO: peridically re-compute scaling factor)
    //float rxy_abs = cabsf(rxy) / sqrtf(_q->x2_hat);
    float rxy_abs = cabsf(rxy);
    
    printf("  rxy=%8.2f, x2-hat=%12.8f\n", rxy_abs, 10*log10f(_q->x2_hat));


    return 0;
}

