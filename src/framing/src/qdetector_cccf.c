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

#define DEBUG_QDETECTOR              1
#define DEBUG_QDETECTOR_PRINT        0
#define DEBUG_QDETECTOR_FILENAME     "qdetector_cccf_debug.m"

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

    unsigned int    s_len;          // template (time) length: k * (sequence_len + 2*m)
    float complex * s;              // template (time), [size: s_len x 1]
    float complex * S;              // template (freq), [size: nfft x 1]
    float           s2_sum;         // sum{ s^2 }

    float complex * buf_time_0;     // time-domain buffer (FFT)
    float complex * buf_freq_0;     // frequence-domain buffer (FFT)
    float complex * buf_freq_1;     // frequence-domain buffer (IFFT)
    float complex * buf_time_1;     // time-domain buffer (IFFT)
    unsigned int    nfft;           // fft size
    fftplan         fft;            // FFT
    fftplan         ifft;           // IFFT

    unsigned int    counter;        //
    unsigned int    num_transforms; //

    float           x2_sum_0;       // sum{ |x|^2 } of first half of buffer
    float           x2_sum_1;       // sum{ |x|^2 } of second half of buffer

    float           gamma_hat;      // signal level estimate
    float           tau_hat;        // timing offset estimate
    float           phi_hat;        // carrier phase offset estimate
    float           dphi_hat;       // carrier frequency offset estimate

#if 0
    nco_crcf        mixer;          // frequency correction
    eqlms_cccf      equalizer;      // equalizing filter
#endif

#if DEBUG_QDETECTOR
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
    } else if (_k < 2 || _k > 80) {
        fprintf(stderr,"error: qdetector_cccf_create(), samples per symbol must be in [2,80]\n");
        exit(1);
    } else if (_m < 1 || _m > 100) {
        fprintf(stderr,"error: qdetector_cccf_create(), filter delay must be in [1,100]\n");
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: qdetector_cccf_create(), excess bandwidth factor must be in [0,1]\n");
        exit(1);
    }
    unsigned int i;
    
    // allocate memory for main object and set internal properties
    qdetector_cccf q = (qdetector_cccf) malloc(sizeof(struct qdetector_cccf_s));
    q->sequence_len  = _sequence_len;
    q->ftype         = _ftype;
    q->k             = _k;
    q->m             = _m;
    q->beta          = _beta;

    // copy sequence
    q->sequence = (float complex*) malloc(q->sequence_len*sizeof(float complex));
    memmove(q->sequence, _sequence, q->sequence_len*sizeof(float complex));

    // create time-domain template
    q->s_len = q->k * (q->sequence_len + 2*q->m);
    q->s     = (float complex*) malloc(q->s_len * sizeof(float complex));
    firinterp_crcf interp = firinterp_crcf_create_rnyquist(q->ftype, q->k, q->m, q->beta, 0);
    for (i=0; i<q->sequence_len + 2*q->m; i++)
        firinterp_crcf_execute(interp, i < q->sequence_len ? q->sequence[i] : 0, &q->s[q->k*i]);
    firinterp_crcf_destroy(interp);
    q->s2_sum = liquid_sumsqcf(q->s, q->s_len); // compute sum{ s^2 }

    // prepare transforms
    q->nfft     = 1 << liquid_nextpow2( (unsigned int)( 1.5f * q->s_len ) ); // NOTE: must be even
    q->buf_time_0 = (float complex*) malloc(q->nfft * sizeof(float complex));
    q->buf_freq_0 = (float complex*) malloc(q->nfft * sizeof(float complex));
    q->buf_freq_1 = (float complex*) malloc(q->nfft * sizeof(float complex));
    q->buf_time_1 = (float complex*) malloc(q->nfft * sizeof(float complex));

    q->fft  = fft_create_plan(q->nfft, q->buf_time_0, q->buf_freq_0, LIQUID_FFT_FORWARD,  0);
    q->ifft = fft_create_plan(q->nfft, q->buf_freq_1, q->buf_time_1, LIQUID_FFT_BACKWARD, 0);

    // create frequency-domain template by taking nfft-point transform on 's', storing in 'S'
    q->S = (float complex*) malloc(q->nfft * sizeof(float complex));
    memset(q->buf_time_0, 0x00, q->nfft*sizeof(float complex));
    memmove(q->buf_time_0, q->s, q->s_len*sizeof(float complex));
    fft_execute(q->fft);
    memmove(q->S, q->buf_freq_0, q->nfft*sizeof(float complex));

    // reset state variables
    q->counter        = q->nfft/2;
    q->num_transforms = 0;
    q->x2_sum_0       = 0.0f;
    q->x2_sum_1       = 0.0f;
    memset(q->buf_time_0, 0x00, q->nfft*sizeof(float complex));

    // return object
    return q;
}

void qdetector_cccf_destroy(qdetector_cccf _q)
{
    // free allocated arrays
    free(_q->sequence);
    free(_q->s         );
    free(_q->S         );
    free(_q->buf_time_0);
    free(_q->buf_freq_0);
    free(_q->buf_freq_1);
    free(_q->buf_time_1);

    // destroy objects
    fft_destroy_plan(_q->fft);
    fft_destroy_plan(_q->ifft);

    // free main object memory
    free(_q);
}

void qdetector_cccf_print(qdetector_cccf _q)
{
    printf("qdetector_cccf:\n");
    printf("  sequence length       :   %-u\n",  _q->sequence_len);
    printf("  filter type           :   %-u\n",  _q->ftype);
    printf("  k    (samples/symbol) :   %-u\n",  _q->k);
    printf("  m    (filter delay)   :   %-u\n",  _q->m);
    printf("  beta (excess b/w)     :   %.3f\n", _q->beta);
    printf("  template length (time):   %-u\n",  _q->s_len);
    printf("  FFT size              :   %-u\n",  _q->nfft);
}

void qdetector_cccf_reset(qdetector_cccf _q)
{
}

void qdetector_cccf_execute(qdetector_cccf _q,
                            float complex  _x)
{
    // write sample to buffer and increment counter
    _q->buf_time_0[_q->counter++] = _x;

    // accumulate signal magnitude
    _q->x2_sum_1 += crealf(_x)*crealf(_x) + cimagf(_x)*cimagf(_x);

    if (_q->counter < _q->nfft)
        return;
    
    unsigned int i;

    // reset counter (last half of time buffer)
    _q->counter = _q->nfft/2;

    // run transform
    fft_execute(_q->fft);
    
    // cross-multiply
    for (i=0; i<_q->nfft; i++)
        _q->buf_freq_1[i] = _q->buf_freq_0[i] * conjf(_q->S[i]);

    // run inverse transform
    fft_execute(_q->ifft);
    
    // scale by signal level (TODO: use median rather than mean signal level)
    float g0 = sqrtf(_q->x2_sum_0 + _q->x2_sum_1) * sqrtf((float)(_q->s_len) / (float)(_q->nfft));
    float g = 1.0f / ( (float)(_q->nfft) * g0 * sqrtf(_q->s2_sum) );
    for (i=0; i<_q->nfft; i++)
        _q->buf_time_1[i] *= g;

#if DEBUG_QDETECTOR
    char filename[64];
    sprintf(filename,"qdetector_out_%u.m", _q->num_transforms);
    FILE * fid = fopen(filename, "w");
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"nfft = %u;\n", _q->nfft);
    for (i=0; i<_q->nfft; i++)
        fprintf(fid,"rxy(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(_q->buf_time_1[i]), cimagf(_q->buf_time_1[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=[0:(nfft-1)];\n");
    fprintf(fid,"plot(t,abs(rxy));\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("debug: %s\n", filename);
#endif
    _q->num_transforms++;

    // search for peak
    float        rxy_peak  = 0.0f;
    unsigned int rxy_index = 0;
    for (i=0; i<_q->nfft; i++) {
        float rxy_abs = cabsf(_q->buf_time_1[i]);
        if (rxy_abs > rxy_peak || i==0) {
            rxy_peak  = rxy_abs;
            rxy_index = i;
        }
    }
    // check detection threshold
    float rxy_threshold = 0.7f;
    if (rxy_peak > rxy_threshold) {
        printf("*** frame detected! rxy = %12.8f, idx=%u\n", rxy_peak, rxy_index);
    }

    // copy last half of fft input buffer to front
    memmove(_q->buf_time_0, _q->buf_time_0 + _q->nfft/2, (_q->nfft/2)*sizeof(float complex));

    // swap accumulated signal levels
    _q->x2_sum_0 = _q->x2_sum_1;
    _q->x2_sum_1 = 0.0f;
}


void qdetector_cccf_execute_block(qdetector_cccf  _q,
                                  float complex * _x,
                                  unsigned int    _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        qdetector_cccf_execute(_q, _x[i]);
}


