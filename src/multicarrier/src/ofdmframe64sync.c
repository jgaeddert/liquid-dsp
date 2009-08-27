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
    cfwindow wcp;
    cfwindow wdelay;
    float complex rxy;
    float complex * rxy_buffer;
    float rxy_magnitude;
    float rxy_threshold;
    bool  cp_detected;
    unsigned int cp_excess_delay;
    unsigned int cp_timer;
    unsigned int rxy_buffer_index;

    float zeta;         // scaling factor
    float nu_hat;       // carrier frequency offset estimation
    float dt_hat;       // symbol timing offset estimation

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
    q->wcp    = cfwindow_create(q->cp_len);
    q->wdelay = cfwindow_create(q->cp_len + q->num_subcarriers);
    q->zeta = 1.0f / sqrtf((float)(q->num_subcarriers));
    q->rxy_threshold = 0.75f*(float)(q->cp_len)*(q->zeta);
    q->rxy_buffer = (float complex*) malloc((q->cp_len)*sizeof(float complex));
    
#if DEBUG_OFDMFRAME64SYNC_PRINT
    printf("rxy threshold : %12.8f\n", q->rxy_threshold);
#endif
#if DEBUG_OFDMFRAME64SYNC
    q->debug_rxy = cfwindow_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
#endif

    q->cp_detected = false;

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

    cfwindow_destroy(_q->wcp);
    cfwindow_destroy(_q->wdelay);
    free(_q);
}

void ofdmframe64sync_print(ofdmframe64sync _q)
{
    printf("ofdmframe64sync:\n");
}

void ofdmframe64sync_reset(ofdmframe64sync _q)
{
    _q->cp_detected     = false;
    _q->cp_excess_delay = 0;
    _q->cp_timer        = _q->num_subcarriers - _q->cp_len;
}

void ofdmframe64sync_execute(ofdmframe64sync _q,
                           float complex * _x,
                           unsigned int _n)
{

    unsigned int i;
    for (i=0; i<_n; i++) {
        cfwindow_push(_q->wcp,   conj(_x[i]));
        cfwindow_push(_q->wdelay,     _x[i]*_q->zeta);

        ofdmframe64sync_cpcorrelate(_q);
        _q->rxy_magnitude = cabsf(_q->rxy);
#if DEBUG_OFDMFRAME64SYNC
        cfwindow_push(_q->debug_rxy, _q->rxy);
#endif
        if (_q->cp_timer > 0) {
            // not enough samples gathered to merit running correlator
            _q->cp_timer--;
            continue;
        }

        if (!_q->cp_detected) {
            // cyclic prefix has not been detected; check threshold
            if (_q->rxy_magnitude > _q->rxy_threshold) {
                // cyclic prefix detected
                _q->cp_detected = true;
                _q->rxy_buffer_index = 0;
            }
        } else {
            // cyclic prefix has been detected; store correlation in buffer
            if (_q->rxy_buffer_index != _q->cp_len) {
                // buffer is not yet full
                _q->rxy_buffer[_q->rxy_buffer_index] = _q->rxy;
                _q->rxy_buffer_index++;
            } else {
                // buffer is full: look for maximum
                ofdmframe64sync_findrxypeak(_q);
                _q->nu_hat = -cargf(_q->rxy)/((float)(_q->num_subcarriers));
#if DEBUG_OFDMFRAME64SYNC_PRINT
                printf("max |rxy| found: %12.4f at i=%u\n",
                        cabsf(_q->rxy),
                        i);
                        //i-_q->cp_len+_q->cp_excess_delay);
                printf("  excess delay : %u\n", _q->cp_excess_delay);
                printf("  nu_hat = %12.8f\n", _q->nu_hat);
#endif

                ofdmframe64sync_rxpayload(_q);
    
                // TODO : ofdmframe64sync_execute(), wait before resetting
                // reset state
                _q->cp_detected = false;
                _q->cp_timer    = _q->num_subcarriers - _q->cp_len;
            }
        }
    }
}

void ofdmframe64sync_rxpayload(ofdmframe64sync _q)
{
    // read samples from buffer
    float complex *rc;
    cfwindow_read(_q->wdelay,&rc);

    // compensate for sample delay
    rc += _q->cp_excess_delay;

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

    // TODO : compensate for fractional time delay
    //float dt = (float)(d) / (float)(_q->num_subcarriers);
    float dt = 0.0f;
    for (i=0; i<_q->num_subcarriers; i++)
        _q->X[i] *= cexpf(_Complex_I*2.0f*M_PI*dt*i);

    if (_q->callback != NULL)
        _q->callback(_q->X, _q->num_subcarriers, _q->userdata);
}

void ofdmframe64sync_cpcorrelate(ofdmframe64sync _q)
{
    float complex * rcp;    // read pointer: cyclic prefix
    float complex * rdelay; // read pointer: delay line

    cfwindow_read(_q->wcp,    &rcp);
    cfwindow_read(_q->wdelay, &rdelay);

    // TODO : cpcorrelate uses unnecessary cycles: should rather store _correlation_ in buffers
    dotprod_cccf_run(rcp,rdelay,_q->cp_len,&_q->rxy);
}

void ofdmframe64sync_findrxypeak(ofdmframe64sync _q)
{
    unsigned int i;
    float rxy_peak = 0.0f;
    float rxy_magnitude;
    for (i=0; i<_q->cp_len; i++) {
        rxy_magnitude = cabsf(_q->rxy_buffer[i]);
        if (rxy_magnitude > rxy_peak) {
            rxy_peak = rxy_magnitude;
            _q->cp_excess_delay = i;
        }
    }
    _q->rxy = _q->rxy_buffer[_q->cp_excess_delay];
}

