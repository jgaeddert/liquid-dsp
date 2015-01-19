/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
// M-ary frequency-shift keying demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "liquid.internal.h"

#define DEBUG_FSKDEM 1

// 
// internal methods
//

// fskdem
struct fskdem_s {
    // common
    unsigned int m;             // bits per symbol
    unsigned int k;             // samples per symbol
    float        bandwidth;     // filter bandwidth parameter

    // derived values
    unsigned int    M;          // constellation size
    float           M2;         // (M-1)/2
    unsigned int    K;          // FFT size
    float complex * buf_time;   // FFT input buffer
    float complex * buf_freq;   // FFT output buffer
    fftplan         fft;        // FFT object
    unsigned int *  demod_map;  // demodulation map

    // firpfb resampler for timing recovery
    // nco for carrier recovery
};

// create fskdem object (frequency demodulator)
//  _m          :   bits per symbol, _m > 0
//  _k          :   samples/symbol, _k >= 2^_m
//  _bandwidth  :   total signal bandwidth, (0,0.5)
fskdem fskdem_create(unsigned int _m,
                     unsigned int _k,
                     float        _bandwidth)
{
    // validate input
    if (_m == 0) {
        fprintf(stderr,"error: fskdem_create(), bits/symbol must be greater than 0\n");
        exit(1);
    } else if (_k < 2 || _k > 2048) {
        fprintf(stderr,"error: fskdem_create(), samples/symbol must be in [2^_m, 2048]\n");
        exit(1);
    } else if (_bandwidth <= 0.0f || _bandwidth >= 0.5f) {
        fprintf(stderr,"error: fskdem_create(), bandwidth must be in (0,0.5)\n");
        exit(1);
    }

    // create main object memory
    fskdem q = (fskdem) malloc(sizeof(struct fskdem_s));

    // set basic internal properties
    q->m         = _m;              // bits per symbol
    q->k         = _k;              // samples per symbol
    q->bandwidth = _bandwidth;      // signal bandwidth

    // derived values
    q->M  = 1 << q->m;              // constellation size
    q->M2 = 0.5f*(float)(q->M-1);   // (M-1)/2

    // compute demodulation FFT size such that FFT output bin frequencies are
    // as close to modulated frequencies as possible
    float        df = q->bandwidth / q->M2;         // frequency spacing
    float        err_min = 1e9f;                    // minimum error value
    unsigned int K_min = q->k;                      // minimum FFT size
    unsigned int K_max = q->k*4 < 16 ? 16 : q->k*4; // maximum FFT size
    unsigned int K_hat;
    for (K_hat=K_min; K_hat<=K_max; K_hat++) {
        // compute candidate FFT size
        float v     = 0.5f*df*(float)K_hat;         // bin spacing
        float err = fabsf( roundf(v) - v );         // fractional bin spacing

#if DEBUG_FSKDEM
        // print results
        printf("  K_hat = %4u : v = %12.8f, err=%12.8f %s\n", K_hat, v, err, err < err_min ? "*" : "");
#endif

        // save best result
        if (K_hat==K_min || err < err_min) {
            q->K    = K_hat;
            err_min = err;
        }

        // perfect match; no need to continue searching
        if (err < 1e-6f)
            break;
    }
    
    // determine demodulation mapping between tones and frequency bins
    // TODO: use gray coding
    q->demod_map = (unsigned int *) malloc(q->M * sizeof(unsigned int));
    unsigned int i;
    for (i=0; i<q->M; i++) {
        // print frequency bins
        float freq = ((float)i - q->M2) * q->bandwidth / q->M2;
        float idx  = freq * (float)(q->K);
        unsigned int index = (unsigned int) (idx < 0 ? roundf(idx + q->K) : roundf(idx));
        q->demod_map[i] = index;
#if DEBUG_FSKDEM
        printf("  s=%3u, f = %12.8f, index=%3u\n", i, freq, index);
#endif
    }

    // check for uniqueness
    for (i=1; i<q->M; i++) {
        if (q->demod_map[i] == q->demod_map[i-1]) {
            fprintf(stderr,"warning: fskdem_create(), demod map is not unique; consider increasing bandwidth\n");
            break;
        }
    }

    // allocate memory for transform
    q->buf_time = (float complex*) malloc(q->K * sizeof(float complex));
    q->buf_freq = (float complex*) malloc(q->K * sizeof(float complex));
    q->fft = fft_create_plan(q->K, q->buf_time, q->buf_freq, LIQUID_FFT_FORWARD, 0);

    // reset modem object
    fskdem_reset(q);

    // return main object
    return q;
}

// destroy fskdem object
void fskdem_destroy(fskdem _q)
{
    // free allocated arrays
    free(_q->demod_map);
    free(_q->buf_time);
    free(_q->buf_freq);
    fft_destroy_plan(_q->fft);

    // free main object memory
    free(_q);
}

// print fskdem object internals
void fskdem_print(fskdem _q)
{
    printf("fskdem : frequency-shift keying demodulator\n");
    printf("    bits/symbol     :   %u\n", _q->m);
    printf("    samples/symbol  :   %u\n", _q->k);
    printf("    bandwidth       :   %8.5f\n", _q->bandwidth);
}

// reset state
void fskdem_reset(fskdem _q)
{
    // reset time buffer
    unsigned int i;
    for (i=0; i<_q->K; i++)
        _q->buf_time[i] = 0.0f;
}

#if 0
// demodulate array of samples, recovering timing etc.
//  _q      :   continuous-phase frequency demodulator object
//  _y      :   input sample array [size: _n x 1]
//  _n      :   input sample array length
//  _s      :   output symbol array
//  _nw     :   number of output symbols written
void fskdem_demodulate(fskdem          _q,
                       float complex * _y,
                       unsigned int    _n,
                       unsigned int  * _s,
                       unsigned int  * _nw)
{
    *_nw = 0;
}
#else
// demodulate symbol, assuming perfect symbol timing
//  _q      :   continuous-phase frequency demodulator object
//  _y      :   input sample array [size: _k x 1]
unsigned int fskdem_demodulate(fskdem          _q,
                               float complex * _y)
{
    // copy input to internal time buffer
    memmove(_q->buf_time, _y, _q->k*sizeof(float complex));

    // compute transform, storing result in 'buf_freq'
    fft_execute(_q->fft);

    // find maximum by looking at particular bins
    float        vmax  = 0;
    unsigned int s     = 0;
    unsigned int s_opt = 0;

    // run search
    for (s=0; s<_q->M; s++) {
        float v = cabsf( _q->buf_freq[_q->demod_map[s]] );
        if (s==0 || v > vmax) {
            s_opt = s;
            vmax  =v;
        }
    }

    // save best result
    return s_opt;
}
#endif
