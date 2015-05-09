/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

#define DEBUG_FIRPFBCH_PRINT    0

#define FIRFILT(name)       LIQUID_CONCAT(firfilt_crcf,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_crcf,name)
#define WINDOW(name)        LIQUID_CONCAT(windowcf,name)

struct firpfbch_s {
    unsigned int num_channels;
    unsigned int m;
    float beta;
    float dt;
    float complex * x;  // time-domain buffer
    float complex * X;  // freq-domain buffer

    // run|state buffers
    float complex * X_prime;    // freq-domain buffer (analysis
                                // filter bank)

    // filter
    unsigned int h_len;
    float * h;
    unsigned int filter_index;
    
    // create separate bank of dotprod and window objects
    DOTPROD() * dp;
    WINDOW() * w;

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
    c->dp   = (DOTPROD()*) malloc((c->num_channels)*sizeof(DOTPROD()));
    c->w    = (WINDOW()*) malloc((c->num_channels)*sizeof(WINDOW()));

    // design filter
    // TODO: use filter prototype object
    c->h_len = 2*(c->m)*(c->num_channels);
    c->h = (float*) malloc((c->h_len+1)*sizeof(float));

    if (c->nyquist == FIRPFBCH_NYQUIST) {
        float fc = 0.5f/(float)(c->num_channels);  // cutoff frequency
        liquid_firdes_kaiser(c->h_len+1, fc, c->beta, 0.0f, c->h);
    } else if (c->nyquist == FIRPFBCH_ROOTNYQUIST) {
        design_rkaiser_filter(c->num_channels, c->m, c->beta, c->dt, c->h);
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
        // sub-sample prototype filter, loading coefficients in reverse order
        for (n=0; n<h_sub_len; n++) {
            h_sub[h_sub_len-n-1] = c->h[i + n*(c->num_channels)];
        }
        // create window buffer and dotprod object (coefficients
        // loaded in reverse order)
        c->dp[i] = DOTPROD(_create)(h_sub,h_sub_len);
        c->w[i]  = WINDOW(_create)(h_sub_len);

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
    c->X_prime = (float complex*) malloc((c->num_channels)*sizeof(float complex));
    firpfbch_clear(c);

    // create fft plan
    c->fft = FFT_CREATE_PLAN(c->num_channels, c->X, c->x, FFT_DIR_BACKWARD, FFT_METHOD);

    return c;
}

void firpfbch_destroy(firpfbch _c)
{
    unsigned int i;
    for (i=0; i<_c->num_channels; i++) {
        DOTPROD(_destroy)(_c->dp[i]);
        WINDOW(_destroy)(_c->w[i]);
    }
    free(_c->dp);
    free(_c->w);

    FFT_DESTROY_PLAN(_c->fft);
    free(_c->h);
    free(_c->x);
    free(_c->X);
    free(_c->X_prime);
    free(_c);
}

void firpfbch_clear(firpfbch _c)
{
    unsigned int i;
    for (i=0; i<_c->num_channels; i++) {
        WINDOW(_clear)(_c->w[i]);
        _c->x[i] = 0;
        _c->X[i] = 0;
        _c->X_prime[i] = 0;
    }
    _c->filter_index = 0;
}

void firpfbch_print(firpfbch _c)
{
    printf("firpfbch: [%u channels]\n", _c->num_channels);
    unsigned int i;
    for (i=0; i<_c->num_channels; i++) {
        DOTPROD(_print)(_c->dp[i]);
    }
}

void firpfbch_get_filter_taps(firpfbch _c, 
                              float * _h)
{
    memmove(_h, _c->h, (_c->h_len)*sizeof(float));
}

// 
// SYNTHESIZER
//

void firpfbch_synthesizer_execute(firpfbch _c, float complex * _x, float complex * _y)
{
    unsigned int i;

    // copy samples into ifft input buffer _c->X
    memmove(_c->X, _x, (_c->num_channels)*sizeof(float complex));

    // execute inverse fft, store in buffer _c->x
    FFT_EXECUTE(_c->fft);

    // push samples into filter bank and execute, putting
    // samples into output buffer _y
    float complex * r;
    for (i=0; i<_c->num_channels; i++) {
        WINDOW(_push)(_c->w[i], _c->x[i]);
        WINDOW(_read)(_c->w[i], &r);
        DOTPROD(_execute)(_c->dp[i], r, &(_y[i]));

        // invoke scaling factor
        _y[i] /= (float)(_c->num_channels);
    }
}

// 
// ANALYZER
//

void firpfbch_analyzer_execute(firpfbch _c, float complex * _x, float complex * _y)
{
    unsigned int i;
    for (i=0; i<_c->num_channels; i++)
        firpfbch_analyzer_push(_c,_x[i]);

    // run the analysis filters on the given input
    firpfbch_analyzer_run(_c,_y);

    // save the run state : IDFT input X -> X_prime
    firpfbch_analyzer_saverunstate(_c);
}

void firpfbch_analyzer_push(firpfbch _c, float complex _x)
{
    // push sample into the buffer at filter_index
    WINDOW(_push)(_c->w[_c->filter_index], _x);

    // decrement filter index
    _c->filter_index = (_c->filter_index+_c->num_channels-1) % _c->num_channels;
}

void firpfbch_analyzer_run(firpfbch _c, float complex * _y)
{
    // NOTE: The analyzer is different from the synthesizer in
    //       that the invocation of the commutator results in a
    //       delay from the first input sample to the resulting
    //       partitions.  As a result, the inverse DFT is
    //       invoked after the first filter is run, after which
    //       the remaining filters are executed.

    // restore saved IDFT input state X from X_prime
    memmove(_c->X, _c->X_prime, (_c->num_channels)*sizeof(float complex));

    unsigned int i, b;
    unsigned int k = _c->filter_index;

    // push first value and compute output
    float complex * r;
    WINDOW(_read)(_c->w[k], &r);
    DOTPROD(_execute)(_c->dp[0], r, &(_c->X[0]));

    // execute inverse fft, store in buffer _c->x
    FFT_EXECUTE(_c->fft);

    // copy results to output buffer
    memmove(_y, _c->x, (_c->num_channels)*sizeof(float complex));

    // push remaining samples into filter bank and execute in
    // *reverse* order, putting result into the inverse DFT
    // input buffer _c->X
    //for (i=1; i<_c->num_channels; i++) {
    // NOTE : the filter window buffers have already been loaded
    //        in the proper reverse order, so there is no need
    //        to execute the dot products in any particular order,
    //        so long as they are aligned with the proper input
    //        buffer.
    for (i=1; i<_c->num_channels; i++) {
        b = (k+i) % _c->num_channels;
        WINDOW(_read)(_c->w[b], &r);
        DOTPROD(_execute)(_c->dp[i], r, &(_c->X[i]));
    }
}

// save the run state of the filter bank by dumping the
// IDFT input buffer X into the temporary buffer X_prime
void firpfbch_analyzer_saverunstate(firpfbch _c)
{
    memmove(_c->X_prime, _c->X, (_c->num_channels)*sizeof(float complex));
}

// clear the run state of the filter bank
void firpfbch_analyzer_clearrunstate(firpfbch _c)
{
    unsigned int i;
    for (i=0; i<_c->num_channels; i++)
        _c->X_prime[i] = 0;
}
