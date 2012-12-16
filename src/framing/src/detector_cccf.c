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
    //windowcf w;         // input buffer

    // internal correlators
    //dotprod_cccf * dp;

    // state
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

    detector_cccf q = (detector_cccf) malloc(sizeof(struct detector_cccf_s));

    // set internal properties
    q->n = _n;

    // allocate memory for sequence and copy
    q->s = (unsigned char*) malloc((q->n)*sizeof(unsigned char));
    memmove(q->s, _s, q->n*sizeof(unsigned char));

    // reset state
    detector_cccf_reset(q);

    // return object
    return q;
}

void detector_cccf_destroy(detector_cccf _q)
{
    // free internal buffers/arrays
    free(_q->s);

    // free main object memory
    free(_q);
}

void detector_cccf_print(detector_cccf _q)
{
    printf("detector_cccf:\n");
    printf("    sequence length     :   %-u\n", _q->n);
}

void detector_cccf_reset(detector_cccf _q)
{
    // reset internal state
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
    return 0;
}



