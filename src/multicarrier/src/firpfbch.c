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
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

//#define DEBUG

#define FIR_FILTER(name)    LIQUID_CONCAT(fir_filter_crcf,name)

struct firpfbch_s {
    unsigned int num_channels;
    unsigned int m;
    float beta;
    float dt;
    float complex * x;  // time-domain buffer
    float complex * X;  // freq-domain buffer
    
    FIR_FILTER() * bank;
#if HAVE_FFTW3_H
    fftwf_plan fft;
#else
    fftplan fft;
#endif
    int nyquist;    // nyquist/root-nyquist
    int type;       // synthesis/analysis
};

firpfbch firpfbch_create(unsigned int _num_channels,
                         unsigned int _m,
                         float _beta,
                         float _dt,
                         int _nyquist,
                         int _type)
{
    firpfbch c = (firpfbch) malloc(sizeof(struct firpfbch_s));
    c->num_channels = _num_channels;
    c->m            = _m;
    c->beta         = _beta;
    c->dt           = _dt;
    c->nyquist      = _nyquist;
    c->type         = _type;

    // create bank of filters
    c->bank = (FIR_FILTER()*) malloc((c->num_channels)*sizeof(FIR_FILTER()));

    // design filter
    unsigned int h_len;

    // design filter using kaiser window and be done with it
    // TODO: use filter prototype object
    if (_m < 1) {
        printf("error: firpfbch_create(), invalid filter delay (must be greater than 0)\n");
        exit(1);
    }
    h_len = 2*(c->m)*(c->num_channels);
    float h[h_len+1];
    if (c->nyquist == FIRPFBCH_NYQUIST) {
        float fc = 1/(float)(c->num_channels);  // cutoff frequency
        fir_kaiser_window(h_len+1, fc, c->beta, 0.0f, h);
    } else if (c->nyquist == FIRPFBCH_ROOTNYQUIST) {
        design_rrc_filter(c->num_channels, c->m, c->beta, c->dt, h);
    } else {
        printf("error: firpfbch_create(), unsupported nyquist flag: %d\n", _nyquist);
        exit(1);
    }

    // generate bank of sub-samped filters
    unsigned int i, n;
    unsigned int h_sub_len = 2*(c->m);  // length of each sub-sampled filter
    float h_sub[h_sub_len];
    for (i=0; i<c->num_channels; i++) {
        for (n=0; n<h_sub_len; n++) {
            h_sub[n] = h[i + n*(c->num_channels)];
        }   
        c->bank[i] = FIR_FILTER(_create)(h_sub, h_sub_len);

#ifdef DEBUG
        printf("h_sub[%u] :\n", i);
        for (n=0; n<h_sub_len; n++)
            printf("  h[%3u] = %8.4f\n", n, h_sub[n]);
#endif
    }

#ifdef DEBUG
    for (i=0; i<h_len+1; i++)
        printf("h(%4u) = %12.4e;\n", i+1, h[i]);
#endif

    // allocate memory for buffers
    c->x = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    c->X = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    firpfbch_clear(c);

    // create fft plan
#if HAVE_FFTW3_H
    c->fft = fftwf_plan_dft_1d(c->num_channels, c->X, c->x, FFTW_BACKWARD, FFTW_ESTIMATE);
#else
    c->fft = fft_create_plan(c->num_channels, c->X, c->x, FFT_REVERSE);
#endif

    return c;
}

void firpfbch_destroy(firpfbch _c)
{
    unsigned int i;
    for (i=0; i<_c->num_channels; i++)
        fir_filter_crcf_destroy(_c->bank[i]);
    free(_c->bank);

#if HAVE_FFTW3_H
    fftwf_destroy_plan(_c->fft);
#else
    fft_destroy_plan(_c->fft);
#endif
    free(_c->x);
    free(_c->X);
    free(_c);
}

void firpfbch_clear(firpfbch _c)
{
    unsigned int i;
    for (i=0; i<_c->num_channels; i++) {
        fir_filter_crcf_clear(_c->bank[i]);
        _c->x[i] = 0;
        _c->X[i] = 0;
    }
}

void firpfbch_print(firpfbch _c)
{
    printf("firpfbch: [%u taps]\n", 0);
}


void firpfbch_synthesizer_execute(firpfbch _c, float complex * _x, float complex * _y)
{
    unsigned int i;

    // copy samples into ifft input buffer _c->X
    memmove(_c->X, _x, (_c->num_channels)*sizeof(float complex));

    // execute inverse fft, store in buffer _c->x
#if HAVE_FFTW3_H
    fftwf_execute(_c->fft);
#else
    fft_execute(_c->fft);
#endif

    // push samples into filter bank and execute, putting
    // samples into output buffer _y
    for (i=0; i<_c->num_channels; i++) {
        fir_filter_crcf_push(_c->bank[i], _c->x[i]);
        fir_filter_crcf_execute(_c->bank[i], &(_y[i]));

        // invoke scaling factor
        _y[i] /= (float)(_c->num_channels);
    }
}

void firpfbch_analyzer_execute(firpfbch _c, float complex * _x, float complex * _y)
{
    // NOTE: The analyzer is different from the synthesizer in
    //       that the invocation of the commutator results in a
    //       delay from the first input sample to the resulting
    //       partitions.  As a result, the inverse DFT is
    //       invoked after the first filter is run, after which
    //       the remaining filters are executed.

    unsigned int i, b;

    // push first value and compute output
    fir_filter_crcf_push(_c->bank[0], _x[0]);
    fir_filter_crcf_execute(_c->bank[0], &(_c->X[0]));

    // execute inverse fft, store in buffer _c->x
#if HAVE_FFTW3_H
    fftwf_execute(_c->fft);
#else
    fft_execute(_c->fft);
#endif

    // copy results to output buffer
    memmove(_y, _c->x, (_c->num_channels)*sizeof(float complex));

    // push remaining samples into filter bank and execute in
    // *reverse* order, putting result into the inverse DFT
    // input buffer _c->X
    for (i=1; i<_c->num_channels; i++) {
        b = _c->num_channels-i;
        fir_filter_crcf_push(_c->bank[b], _x[i]);
        fir_filter_crcf_execute(_c->bank[b], &(_c->X[b]));
    }
}


void firpfbch_execute(firpfbch _c, float complex * _x, float complex * _y)
{
    if (_c->type == FIRPFBCH_ANALYZER)
        firpfbch_analyzer_execute(_c,_x,_y);
    else
        firpfbch_synthesizer_execute(_c,_x,_y);
}

