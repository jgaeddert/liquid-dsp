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
// qdetector_cccf.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_DETECTOR              0
#define DEBUG_DETECTOR_PRINT        0
#define DEBUG_DETECTOR_FILENAME     "qdetector_cccf_debug.m"

//
void qdetector_cccf_init_template(qdetector_cccf _q);

// main object definition
struct qdetector_cccf_s {
    float complex * sequence;       // original sequence of symbols
    unsigned int    sequence_len;   // sequence length
    unsigned int    k;              // samples/symbol
    unsigned int    m;              // filter delay [symbols]
    float           beta;           // excess bandwidth factor
    int             ftype;          // filter type, e.g. LIQUID_FIRDES_RRC

    float           threshold;      // detection threshold
    
    firfilt_crcf    filter;         // matched filter

    windowcf        buffer;         // input buffer

    unsigned int    s_len;          // template (time) length: k * (sequence_len + 2*m)
    float complex * s;              // template (time), [size: s_len x 1]
    float complex * S;              // template (freq), [size: nfft x 1]

    float complex * buf_time;       // time-domain buffer
    float complex * buf_freq;       // frequence-domain buffer
    unsigned int    nfft;           // fft size
    fftplan         fft;            // FFT
    fftplan         ifft;           // IFFT

    float           x2_sum;         // sum{ |x|^2 }

    float           gamma_hat;      // signal level estimate
    float           tau_hat;        // timing offset estimate
    float           phi_hat;        // carrier phase offset estimate
    float           dphi_hat;       // carrier frequency offset estimate

#if 0
    nco_crcf        mixer;          // frequency correction
    eqlms_cccf      equalizer;      // equalizing filter
#endif

#if DEBUG_DETECTOR
    //windowcf        debug_x;
#endif
};

// create detector
//  _sequence       :   symbol sequence
//  _sequence_len   :   length of symbol sequence
//  _k              :   samples/symbol
//  _m              :   filter delay
//  _beta           :   excess bandwidth factor
//  _type           :   filter prototype (e.g. LIQUID_FIRFILT_RRC)
qdetector_cccf qdetector_cccf_create(float complex * _sequence,
                                     unsigned int    _sequence_len,
                                     int             _ftype,
                                     unsigned int    _k,
                                     unsigned int    _m,
                                     float           _beta)
{
    // validate input
    if (_sequence_len == 0) {
        fprintf(stderr,"error: qdetector_cccf_create(), sequence length cannot be zero\n");
        exit(1);
    } else if (_k < 2) {
    } else if (_m < 1) {
    } else if (_beta < 0.0f || _beta > 1.0f) {
    }
    
    // allocate memory for main object and set internal properties
    qdetector_cccf q = (qdetector_cccf) malloc(sizeof(struct qdetector_cccf_s));
    q->sequence_len  = _sequence_len;
    q->ftype         = _ftype;
    q->k             = _k;
    q->m             = _m;
    q->beta          = _beta;

    // copy sequence
    q->sequence = (float complex*) malloc(q->sequence_len*sizeof(float complex));

    // create template
    q->s_len = q->k * (q->sequence_len + 2*q->m);
    q->s     = (float complex*) malloc(q->s_len * sizeof(float complex));
    firinterp_crcf interp = firinterp_crcf_create_rnyquist(q->ftype, q->k, q->m, q->beta, 0);
    firinterp_crcf_execute_block(interp, q->sequence, q->sequence_len, q->s);
    firinterp_crcf_destroy(interp);

    // prepare transforms
    q->nfft = 2048;
    q->buf_time = (float complex*) malloc(q->nfft * sizeof(float complex));
    q->buf_freq = (float complex*) malloc(q->nfft * sizeof(float complex));

    // return object
    return q;
}

void qdetector_cccf_destroy(qdetector_cccf _q)
{
    // free allocated arrays
    free(_q->sequence);
    free(_q->s       );
    free(_q->buf_time);
    free(_q->buf_freq);

    // free main object memory
    free(_q);
}

void qdetector_cccf_print(qdetector_cccf _q)
{
    printf("qdetector_cccf:\n");
    printf("    sequence length     :   %-u\n", _q->sequence_len);
}

void qdetector_cccf_reset(qdetector_cccf _q)
{
}

void qdetector_cccf_execute(qdetector_cccf  _q,
                            float complex * _x,
                            unsigned int    _n)
{
    printf("execute...\n");
}


