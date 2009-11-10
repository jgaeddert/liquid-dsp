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

// Use fftw library if installed, otherwise use internal (less
// efficient) fft library.
#if HAVE_FFTW3_H
#   include <fftw3.h>
#   define FFT_PLAN             fftwf_plan
#   define FFT_CREATE_PLAN      fftwf_plan_dft_1d
#   define FFT_DESTROY_PLAN     fftwf_destroy_plan
#   define FFT_EXECUTE          fftwf_execute
#   define FFT_DIR_FORWARD      FFTW_FORWARD
#   define FFT_DIR_BACKWARD     FFTW_BACKWARD
#   define FFT_METHOD           FFTW_ESTIMATE
#else
#   define FFT_PLAN             fftplan
#   define FFT_CREATE_PLAN      fft_create_plan
#   define FFT_DESTROY_PLAN     fft_destroy_plan
#   define FFT_EXECUTE          fft_execute
#   define FFT_DIR_FORWARD      FFT_FORWARD
#   define FFT_DIR_BACKWARD     FFT_REVERSE
#   define FFT_METHOD           FFTW_ESTIMATE
#endif

#define DEBUG_FIRPFBCH_PRINT    0

#define FIR_FILTER(name)    LIQUID_CONCAT(fir_filter_crcf,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_crcf,name)
#define WINDOW(name)        LIQUID_CONCAT(cfwindow,name)

#define FIRPFBCH_USE_DOTPROD    1

struct firpfbch_s {
    unsigned int num_channels;
    unsigned int m;
    float beta;
    float dt;
    float complex * x;  // time-domain buffer
    float complex * X;  // freq-domain buffer

    // filter
    unsigned int h_len;
    float * h;
    
#if FIRPFBCH_USE_DOTPROD
    // create separate bank of dotprod and window objects
    DOTPROD() * dp;
    WINDOW() * w;
#else
    // create bank of FIR filters
    FIR_FILTER() * bank;
#endif

    // fft plan
    FFT_PLAN fft;

    int nyquist;    // nyquist/root-nyquist
    //int type;       // synthesis/analysis
};

firpfbch firpfbch_create(unsigned int _num_channels,
                         unsigned int _m,
                         float _beta,
                         float _dt,
                         int _nyquist,
                         int _gradient)
{
    firpfbch c = (firpfbch) malloc(sizeof(struct firpfbch_s));
    c->num_channels = _num_channels;
    c->m            = _m;
    c->beta         = _beta;
    c->dt           = _dt;
    c->nyquist      = _nyquist;

    // validate inputs
    if (_m < 1) {
        printf("error: firpfbch_create(), invalid filter delay (must be greater than 0)\n");
        exit(1);
    }

    // create bank of filters
#if FIRPFBCH_USE_DOTPROD
    c->dp   = (DOTPROD()*) malloc((c->num_channels)*sizeof(DOTPROD()));
    c->w    = (WINDOW()*) malloc((c->num_channels)*sizeof(WINDOW()));
#else
    c->bank = (FIR_FILTER()*) malloc((c->num_channels)*sizeof(FIR_FILTER()));
#endif

    // design filter
    // TODO: use filter prototype object
    c->h_len = 2*(c->m)*(c->num_channels);
    c->h = (float*) malloc((c->h_len+1)*sizeof(float));

    if (c->nyquist == FIRPFBCH_NYQUIST) {
        float fc = 1/(float)(c->num_channels);  // cutoff frequency
        fir_kaiser_window(c->h_len+1, fc, c->beta, 0.0f, c->h);
    } else if (c->nyquist == FIRPFBCH_ROOTNYQUIST) {
        design_rrc_filter(c->num_channels, c->m, c->beta, c->dt, c->h);
    } else {
        printf("error: firpfbch_create(), unsupported nyquist flag: %d\n", _nyquist);
        exit(1);
    }
    
    unsigned int i;
    if (_gradient) {
        float dh[c->h_len];
        for (i=0; i<c->h_len; i++) {
            if (i==0) {
                dh[i] = c->h[i+1] - c->h[c->h_len-1];
            } else if (i==c->h_len-1) {
                dh[i] = c->h[0]   - c->h[i-1];
            } else {
                dh[i] = c->h[i+1] - c->h[i-1];
            }
        }
        memmove(c->h, dh, (c->h_len)*sizeof(float));
    }

    // generate bank of sub-samped filters
    unsigned int n;
    unsigned int h_sub_len = 2*(c->m);  // length of each sub-sampled filter
    float h_sub[h_sub_len];
    for (i=0; i<c->num_channels; i++) {
        for (n=0; n<h_sub_len; n++) {
            h_sub[n] = c->h[i + n*(c->num_channels)];
        }
#if FIRPFBCH_USE_DOTPROD
        // create window buffer and dotprod object (coefficients
        // loaded in reverse order)
        c->dp[i] = DOTPROD(_create_rev)(h_sub,h_sub_len);
        c->w[i]  = WINDOW(_create)(h_sub_len);
#else
        c->bank[i] = FIR_FILTER(_create)(h_sub, h_sub_len);
#endif

#if DEBUG_FIRPFBCH_PRINT
        printf("h_sub[%u] :\n", i);
        for (n=0; n<h_sub_len; n++)
            printf("  h[%3u] = %8.4f\n", n, h_sub[n]);
#endif
    }

#if DEBUG_FIRPFBCH_PRINT
    for (i=0; i<c->h_len+1; i++)
        printf("h(%4u) = %12.4e;\n", i+1, c->h[i]);
#endif

    // allocate memory for buffers
    // TODO : use fftw_malloc if HAVE_FFTW3_H
    c->x = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    c->X = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    firpfbch_clear(c);

    // create fft plan
    c->fft = FFT_CREATE_PLAN(c->num_channels, c->X, c->x, FFT_DIR_BACKWARD, FFT_METHOD);

    return c;
}

void firpfbch_destroy(firpfbch _c)
{
    unsigned int i;
#if FIRPFBCH_USE_DOTPROD
    for (i=0; i<_c->num_channels; i++) {
        DOTPROD(_destroy)(_c->dp[i]);
        WINDOW(_destroy)(_c->w[i]);
    }
    free(_c->dp);
    free(_c->w);
#else
    for (i=0; i<_c->num_channels; i++)
        FIR_FILTER(_destroy)(_c->bank[i]);
    free(_c->bank);
#endif

    FFT_DESTROY_PLAN(_c->fft);
    free(_c->h);
    free(_c->x);
    free(_c->X);
    free(_c);
}

void firpfbch_clear(firpfbch _c)
{
    unsigned int i;
    for (i=0; i<_c->num_channels; i++) {
#if FIRPFBCH_USE_DOTPROD
        WINDOW(_clear)(_c->w[i]);
#else
        FIR_FILTER(_clear)(_c->bank[i]);
#endif
        _c->x[i] = 0;
        _c->X[i] = 0;
    }
}

void firpfbch_print(firpfbch _c)
{
    printf("firpfbch: [%u channels]\n", _c->num_channels);
    unsigned int i;
    for (i=0; i<_c->num_channels; i++) {
#if FIRPFBCH_USE_DOTPROD
        DOTPROD(_print)(_c->dp[i]);
#else
#endif
    }
}

void firpfbch_get_filter_taps(firpfbch _c, 
                              float * _h)
{
    memmove(_h, _c->h, (_c->h_len)*sizeof(float));
}

void firpfbch_synthesizer_execute(firpfbch _c, float complex * _x, float complex * _y)
{
    unsigned int i;

    // copy samples into ifft input buffer _c->X
    memmove(_c->X, _x, (_c->num_channels)*sizeof(float complex));

    // execute inverse fft, store in buffer _c->x
    FFT_EXECUTE(_c->fft);

    // push samples into filter bank and execute, putting
    // samples into output buffer _y
#if FIRPFBCH_USE_DOTPROD
    float complex * r;
#endif
    for (i=0; i<_c->num_channels; i++) {
#if FIRPFBCH_USE_DOTPROD
        WINDOW(_push)(_c->w[i], _c->x[i]);
        WINDOW(_read)(_c->w[i], &r);
        DOTPROD(_execute)(_c->dp[i], r, &(_y[i]));
#else
        FIR_FILTER(_push)(_c->bank[i], _c->x[i]);
        FIR_FILTER(_execute)(_c->bank[i], &(_y[i]));
#endif

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
#if FIRPFBCH_USE_DOTPROD
    float complex * r;
    WINDOW(_push)(_c->w[0], _x[0]);
    WINDOW(_read)(_c->w[0], &r);
    DOTPROD(_execute)(_c->dp[0], r, &(_c->X[0]));
#else
    FIR_FILTER(_push)(_c->bank[0], _x[0]);
    FIR_FILTER(_execute)(_c->bank[0], &(_c->X[0]));
#endif

    // execute inverse fft, store in buffer _c->x
    FFT_EXECUTE(_c->fft);

    // copy results to output buffer
    memmove(_y, _c->x, (_c->num_channels)*sizeof(float complex));

    // push remaining samples into filter bank and execute in
    // *reverse* order, putting result into the inverse DFT
    // input buffer _c->X
    for (i=1; i<_c->num_channels; i++) {
        b = _c->num_channels-i;
#if FIRPFBCH_USE_DOTPROD
        WINDOW(_push)(_c->w[b], _x[i]);
        WINDOW(_read)(_c->w[b], &r);
        DOTPROD(_execute)(_c->dp[b], r, &(_c->X[b]));
#else
        FIR_FILTER(_push)(_c->bank[b], _x[i]);
        FIR_FILTER(_execute)(_c->bank[b], &(_c->X[b]));
#endif
    }
}

#if 0
void firpfbch_execute(firpfbch _c, float complex * _x, float complex * _y)
{
    if (_c->type == FIRPFBCH_ANALYZER)
        firpfbch_analyzer_execute(_c,_x,_y);
    else
        firpfbch_synthesizer_execute(_c,_x,_y);
}
#endif

