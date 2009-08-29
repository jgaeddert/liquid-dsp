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
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

#define OFDMFRAME64SYNC_MIN_NUM_SUBCARRIERS   (8)

#define DEBUG_OFDMFRAME64SYNC             1
#define DEBUG_OFDMFRAME64SYNC_PRINT       1
#define DEBUG_OFDMFRAME64SYNC_FILENAME    "ofdmframe64sync_internal_debug.m"
#define DEBUG_OFDMFRAME64SYNC_BUFFER_LEN  (1024)

struct ofdmframe64sync_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    float complex * x; // time-domain buffer
    float complex * X; // freq-domain buffer

    // delay correlator
    autocorr_cccf delay_correlator;
    float complex rxy;
    float complex * rxy_buffer;

    float zeta;         // scaling factor
    float nu_hat;       // carrier frequency offset estimation
    float dt_hat;       // symbol timing offset estimation

    msequence ms_pilot;

#if HAVE_FFTW3_H
    fftwf_plan fft;
#else
    fftplan fft;
#endif

    ofdmframe64sync_callback callback;
    void * userdata;

#if DEBUG_OFDMFRAME64SYNC
    cfwindow debug_rxy;
#endif
};

ofdmframe64sync ofdmframe64sync_create(ofdmframe64sync_callback _callback,
                                       void * _userdata)
{
    ofdmframe64sync q = (ofdmframe64sync) malloc(sizeof(struct ofdmframe64sync_s));
    q->num_subcarriers = 64;
    q->cp_len = 16;

    // allocate memory for buffers
    q->x = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->X = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

#if HAVE_FFTW3_H
    q->fft = fftwf_plan_dft_1d(q->num_subcarriers, q->x, q->X, FFTW_FORWARD, FFTW_ESTIMATE);
#else
    q->fft = fft_create_plan(q->num_subcarriers, q->x, q->X, FFT_FORWARD);
#endif

    // cyclic prefix correlation windows
    q->delay_correlator = autocorr_cccf_create(64,64);
    q->zeta = 1.0f / sqrtf((float)(q->num_subcarriers));
    q->rxy_buffer = (float complex*) malloc((q->cp_len)*sizeof(float complex));
    
#if DEBUG_OFDMFRAME64SYNC
    q->debug_rxy = cfwindow_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
#endif

    // pilot sequence generator
    q->ms_pilot = msequence_create(8);

    q->callback = _callback;
    q->userdata = _userdata;

    return q;
}

void ofdmframe64sync_destroy(ofdmframe64sync _q)
{
#if DEBUG_OFDMFRAME64SYNC
    FILE * fid = fopen(DEBUG_OFDMFRAME64SYNC_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMFRAME64SYNC_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %u;\n", DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    unsigned int i;

    fprintf(fid,"rxy = zeros(1,n);\n");
    float complex * rc;
    cfwindow_read(_q->debug_rxy, &rc);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxy));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    fclose(fid);
    printf("ofdmframe64sync/debug: results written to %s\n", DEBUG_OFDMFRAME64SYNC_FILENAME);

    cfwindow_destroy(_q->debug_rxy);
#endif

    free(_q->x);
    free(_q->X);
    free(_q->rxy_buffer);
#if HAVE_FFTW3_H
    fftwf_destroy_plan(_q->fft);
#else
    fft_destroy_plan(_q->fft);
#endif

    msequence_destroy(_q->ms_pilot);
    autocorr_cccf_destroy(_q->delay_correlator);
    free(_q);
}

void ofdmframe64sync_print(ofdmframe64sync _q)
{
    printf("ofdmframe64sync:\n");
}

void ofdmframe64sync_reset(ofdmframe64sync _q)
{
    // reset pilot sequence generator
    msequence_reset(_q->ms_pilot);
}

void ofdmframe64sync_execute(ofdmframe64sync _q,
                           float complex * _x,
                           unsigned int _n)
{
}

void ofdmframe64sync_rxpayload(ofdmframe64sync _q)
{
    unsigned int i;
    //float dt = (float)(d) / (float)(_q->num_subcarriers);
    float dt = 0.0f;
    for (i=0; i<_q->num_subcarriers; i++)
        _q->X[i] *= cexpf(_Complex_I*2.0f*M_PI*dt*i);

    if (_q->callback != NULL)
        _q->callback(_q->X, _q->num_subcarriers, _q->userdata);
}

void ofdmframe64sync_cpcorrelate(ofdmframe64sync _q)
{
}

void ofdmframe64sync_findrxypeak(ofdmframe64sync _q)
{
}

