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

#define OFDMFRAMESYNC_MIN_NUM_SUBCARRIERS   (8)

#define DEBUG_OFDMFRAMESYNC             1
#define DEBUG_OFDMFRAMESYNC_FILENAME    "ofdmframesync_internal_debug.m"
#define DEBUG_OFDMFRAMESYNC_BUFFER_LEN  (1024)

struct ofdmframesync_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    float complex * x; // time-domain buffer
    float complex * X; // freq-domain buffer

    // delay correlator
    cfwindow wcp;
    cfwindow wdelay;
    float rxy_threshold;
    float rxy_max;
    bool  cp_detected;
    bool  cp_ignore;

    float zeta;         // scaling factor
    float nu_hat;       // carrier frequency offset estimation

#if HAVE_FFTW3_H
    fftwf_plan fft;
#else
    fftplan fft;
#endif

    ofdmframesync_callback callback;
    void * userdata;

#if DEBUG_OFDMFRAMESYNC
    cfwindow debug_rxy;
#endif
};

ofdmframesync ofdmframesync_create(unsigned int _num_subcarriers,
                                   unsigned int _cp_len,
                                   ofdmframesync_callback _callback,
                                   void * _userdata)
{
    ofdmframesync q = (ofdmframesync) malloc(sizeof(struct ofdmframesync_s));

    // error-checking
    if (_num_subcarriers < OFDMFRAMESYNC_MIN_NUM_SUBCARRIERS) {
        printf("error: ofdmframesync_create(), num_subcarriers (%u) below minimum (%u)\n",
                _num_subcarriers, OFDMFRAMESYNC_MIN_NUM_SUBCARRIERS);
        exit(1);
    } else if (_cp_len < 1) {
        printf("error: ofdmframesync_create(), cp_len must be greater than 0\n");
        exit(1);
    } else if (_cp_len > _num_subcarriers) {
        printf("error: ofdmframesync_create(), cp_len (%u) must be less than number of subcarriers(%u)\n",
                _cp_len, _num_subcarriers);
        exit(1);
    }

    q->num_subcarriers = _num_subcarriers;
    q->cp_len = _cp_len;

    // allocate memory for buffers
    q->x = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->X = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

#if HAVE_FFTW3_H
    q->fft = fftwf_plan_dft_1d(q->num_subcarriers, q->x, q->X, FFTW_FORWARD, FFTW_ESTIMATE);
#else
    q->fft = fft_create_plan(q->num_subcarriers, q->x, q->X, FFT_FORWARD);
#endif

    // cyclic prefix correlation windows
    q->wcp    = cfwindow_create(q->cp_len);
    q->wdelay = cfwindow_create(q->cp_len + q->num_subcarriers);
    q->zeta = 1.0f / sqrtf((float)(q->num_subcarriers));
    q->rxy_threshold = 0.5f*(float)(q->cp_len)*(q->zeta);
    
#if DEBUG_OFDMFRAMESYNC
    q->debug_rxy = cfwindow_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
#endif

    q->cp_detected = false;
    q->cp_ignore   = false;

    q->callback = _callback;
    q->userdata = _userdata;

    return q;
}

void ofdmframesync_destroy(ofdmframesync _q)
{
#if DEBUG_OFDMFRAMESYNC
    FILE * fid = fopen(DEBUG_OFDMFRAMESYNC_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMFRAMESYNC_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %u;\n", DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    unsigned int i;

    fprintf(fid,"rxy = zeros(1,n);\n");
    float complex * rc;
    cfwindow_read(_q->debug_rxy, &rc);
    for (i=0; i<DEBUG_OFDMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxy));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    fclose(fid);
    printf("ofdmframesync/debug: results written to %s\n", DEBUG_OFDMFRAMESYNC_FILENAME);

    cfwindow_destroy(_q->debug_rxy);
#endif

    free(_q->x);
    free(_q->X);
#if HAVE_FFTW3_H
    fftwf_destroy_plan(_q->fft);
#else
    fft_destroy_plan(_q->fft);
#endif

    cfwindow_destroy(_q->wcp);
    cfwindow_destroy(_q->wdelay);
    free(_q);
}

void ofdmframesync_print(ofdmframesync _q)
{
    printf("ofdmframesync:\n");
}

void ofdmframesync_reset(ofdmframesync _q)
{
    _q->cp_detected = false;
    _q->rxy_max = 0.0f;
}

void ofdmframesync_execute(ofdmframesync _q,
                           float complex * _x,
                           unsigned int _n)
{

    unsigned int i;
    float complex rxy;
    for (i=0; i<_n; i++) {
        if (_q->cp_ignore)
            return;
        rxy = ofdmframesync_cpcorrelate(_q, _x[i]);
#ifdef DEBUG_OFDMFRAMESYNC
        cfwindow_push(_q->debug_rxy, rxy);
#endif
        if (!_q->cp_detected) {
            // cyclic prefix has not been detected; check threshold
            if (cabsf(rxy) > _q->rxy_threshold) {
                // cyclic prefix detected
                _q->cp_detected = true;
                _q->rxy_max = cabsf(rxy);
            }
        } else {
            // cyclic prefix has been detected; wait for optimal
            // symbol time
            if (cabsf(rxy) > _q->rxy_max) {
                // maximum has not yet been found
                _q->rxy_max = cabsf(rxy);
            } else {
                // maximum correlation found: receive payload
                printf("max |rxy| found: %12.4f at i=%u\n", cabsf(rxy),i);
                _q->nu_hat = -cargf(rxy)/((float)(_q->num_subcarriers));
                printf("  df_hat = %12.8f\n", _q->nu_hat);

                ofdmframesync_rxpayload(_q);
    
                // TODO : ofdmframesync_execute(), wait before resetting
                // reset state
                _q->rxy_max = 0.0f;
                _q->cp_detected = false;
                _q->cp_ignore   = true;
            }
        }
    }
}

void ofdmframesync_rxpayload(ofdmframesync _q)
{
    // read samples from buffer
    float complex *rc;
    cfwindow_read(_q->wdelay,&rc);

    // compensate for sample delay
    rc += _q->cp_len-1;

    // copy to fft buffer
    memmove(_q->x, rc, (_q->num_subcarriers)*sizeof(float complex));

    // compensate for frequency offset
    unsigned int i;
    float phi=0.0f;
    for (i=0; i<_q->num_subcarriers; i++) {
        _q->x[i] *= cexpf(-_Complex_I*phi);
        phi += _q->nu_hat;
    }

    // execute fft
#if HAVE_FFTW3_H
    fftwf_execute(_q->fft);
#else
    fft_execute(_q->fft);
#endif

    // TODO : ofdmframesync: scale?

    if (_q->callback != NULL)
        _q->callback(_q->X, _q->num_subcarriers, _q->userdata);
}

float complex ofdmframesync_cpcorrelate(ofdmframesync _q,
                                        float complex _x)
{
    cfwindow_push(_q->wcp,   conj(_x));
    cfwindow_push(_q->wdelay,     _x*_q->zeta);

    float complex * rcp;    // read pointer: cyclic prefix
    float complex * rdelay; // read pointer: delay line

    cfwindow_read(_q->wcp,    &rcp);
    cfwindow_read(_q->wdelay, &rdelay);

    // TODO : cpcorrelate uses unnecessary cycles: should rather store _correlation_ in buffers
    float complex rxy;
    dotprod_cccf_run(rcp,rdelay,_q->cp_len,&rxy);
    //rxy /= (float)(_q->cp_len);

    return rxy;
}

