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
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

#define DEBUG_OFDMFRAME64GEN                1

const float complex plcp_short[64] = {
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
     -1.4720+ -1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
     -1.4720+ -1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      1.4720+  1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      1.4720+  1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      1.4720+  1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      1.4720+  1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      1.4720+  1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
     -1.4720+ -1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      1.4720+  1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
     -1.4720+ -1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
     -1.4720+ -1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      1.4720+  1.4720*_Complex_I,   0.0000+  0.0000*_Complex_I, 
      0.0000+  0.0000*_Complex_I,   0.0000+  0.0000*_Complex_I
};

const float complex plcp_long[64] = {
     0.0,  1.0, -1.0, -1.0,  1.0,  1.0, -1.0,  1.0, 
    -1.0,  1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  1.0, 
     1.0, -1.0, -1.0,  1.0, -1.0,  1.0, -1.0,  1.0, 
     1.0,  1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, 
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  1.0,  1.0, 
    -1.0, -1.0,  1.0,  1.0, -1.0,  1.0, -1.0,  1.0, 
     1.0,  1.0,  1.0,  1.0,  1.0, -1.0, -1.0,  1.0, 
     1.0, -1.0,  1.0, -1.0,  1.0,  1.0,  1.0,  1.0
};

struct ofdmframe64gen_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    float complex * x;      // time-domain buffer
    float complex * X;      // freq-domain buffer

    float complex * xcp;    // cyclic prefix pointer (not allocated)

#if HAVE_FFTW3_H
    fftwf_plan fft;
#else
    fftplan fft;
#endif
    float zeta;             // fft scaling factor
};

ofdmframe64gen ofdmframe64gen_create()
{
    ofdmframe64gen q = (ofdmframe64gen) malloc(sizeof(struct ofdmframe64gen_s));
    q->num_subcarriers = 64;
    q->cp_len = 16;

    // allocate memory for buffers
    q->x = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->X = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

#if HAVE_FFTW3_H
    q->fft = fftwf_plan_dft_1d(q->num_subcarriers, q->X, q->x, FFTW_BACKWARD, FFTW_ESTIMATE);
#else
    q->fft = fft_create_plan(q->num_subcarriers, q->X, q->x, FFT_REVERSE);
#endif
    q->zeta = 1.0f / sqrtf((float)(q->num_subcarriers));

    // set cyclic prefix array pointer
    q->xcp = &(q->x[q->num_subcarriers - q->cp_len]);

    return q;
}

void ofdmframe64gen_destroy(ofdmframe64gen _q)
{
    free(_q->x);
    free(_q->X);

#if HAVE_FFTW3_H
    fftwf_destroy_plan(_q->fft);
#else
    fft_destroy_plan(_q->fft);
#endif
    free(_q);
}

void ofdmframe64gen_print(ofdmframe64gen _q)
{
    printf("ofdmframe64gen:\n");
    printf("    num subcarriers     :   %u\n", _q->num_subcarriers);
    printf("    cyclic prefix len   :   %u (%6.2f%%)\n",
                    _q->cp_len,
                    100.0f*(float)(_q->cp_len)/(float)(_q->num_subcarriers));
}

void ofdmframe64gen_clear(ofdmframe64gen _q)
{
}

void ofdmframe64gen_writeshortsequence(ofdmframe64gen _q,
                                       float complex * _y)
{
    //
    memmove(_q->X, plcp_short, 64*sizeof(float complex));

#if HAVE_FFTW3_H
    fftwf_execute(_q->fft);
#else
    fft_execute(_q->fft);
#endif

    // move cyclic prefix (double)
    memmove(_y, _q->x+64-2*16, 2*16*sizeof(float complex));

    // move remainder of signal (twice)
    memmove(_y+2*16,    _q->x, 64*sizeof(float complex));
    memmove(_y+2*16+64, _q->x, 64*sizeof(float complex));
}


void ofdmframe64gen_writelongsequence(ofdmframe64gen _q,
                                      float complex * _y)
{
    //
    memmove(_q->X, plcp_long, 64*sizeof(float complex));

#if HAVE_FFTW3_H
    fftwf_execute(_q->fft);
#else
    fft_execute(_q->fft);
#endif

    // move cyclic prefix (double)
    memmove(_y, _q->x+64-2*16, 2*16*sizeof(float complex));

    // move remainder of signal (twice)
    memmove(_y+2*16,    _q->x, 64*sizeof(float complex));
    memmove(_y+2*16+64, _q->x, 64*sizeof(float complex));
}

void ofdmframe64gen_writeheader(ofdmframe64gen _q,
                                float complex * _y)
{
}

void ofdmframe64gen_writesymbol(ofdmframe64gen _q,
                                float complex * _x,
                                float complex * _y)
{
    // move frequency data to internal buffer
    unsigned int i, j=0;
    for (i=0; i<64; i++) {
        if (i==0 || (i>26 && i<38)) {
            // disabled subcarrier
            _q->X[i] = 0.0f;
        } else if (i==9 || i==25 || i==39 || i==55) {
            // pilot subcarrier
            // TODO : use p/n sequence for pilot
            _q->X[i] = 1.0f;// * _q->zeta;
        } else {
            // data subcarrier
            _q->X[i] = _x[j++];// * _q->zeta;
        }

        //printf("X[%3u] = %12.8f + j*%12.8f;\n",i+1,crealf(_q->X[i]),cimagf(_q->X[i]));
    }
    assert(j==48);

    // execute inverse fft, store in buffer _q->x
#if HAVE_FFTW3_H
    fftwf_execute(_q->fft);
#else
    fft_execute(_q->fft);
#endif

    // copy cyclic prefix
    memmove(_y, _q->xcp, (_q->cp_len)*sizeof(float complex));

    // copy remainder of signal
    memmove(&_y[_q->cp_len], _q->x, (_q->num_subcarriers)*sizeof(float complex));
}

