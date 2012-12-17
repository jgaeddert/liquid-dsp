/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
// detector_cccf.c
//
// Pre-demodulation detector
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_DETECTOR              1
#define DEBUG_DETECTOR_BUFFER_LEN   (400)
#define DEBUG_DETECTOR_FILENAME     "detector_cccf_debug.m"

// 
// internal method declarations
//

float detector_cccf_estimate_tau(detector_cccf _q);

float detector_cccf_estimate_dphi(detector_cccf _q);

void detector_cccf_debug_print(detector_cccf _q,
                               const char *  _filename);

struct detector_cccf_s {
    float complex * s;      // sequence
    unsigned int n;         // sequence length
    float threshold;        // detection threshold
    
    // derived values
    float n_inv;            // 1/n (pre-computed for speed)

    //
    windowcf buffer;        // input buffer
    wdelayf x2;             // buffer of |x|^2 values
    float x2_sum;           // ...
    float x2_hat;           // estimate of E{|x|^2}

    // internal correlators
    //dotprod_cccf * dp;
    dotprod_cccf dp;

    // counters/states
    enum {
        DETECTOR_STATE_SEEK=0,  // seek sequence
        DETECTOR_STATE_FINDMAX, // find maximum
    } state;
    unsigned int timer;         // sample timer
    float rxy_max;              // maximum cross-correlation
    float rxy0, rxy1, rxy2;     // buffer of cross-correlation outputs

#if DEBUG_DETECTOR
    windowcf debug_x;
    windowf debug_x2;
    windowf debug_rxy;
#endif
};

// create detector_cccf object
//  _s          :   sequence
//  _n          :   sequence length
//  _threshold  :   detection threshold (default: 0.7)
//  _dphi_max   :   maximum carrier offset
detector_cccf detector_cccf_create(float complex * _s,
                                   unsigned int    _n,
                                   float           _threshold,
                                   float           _dphi_max)
{
    // validate input
    if (_n == 0) {
        fprintf(stderr,"error: detector_cccf_create(), sequence length cannot be zero\n");
        exit(1);
    } else if (_threshold <= 0.0f) {
        fprintf(stderr,"error: detector_cccf_create(), threshold must be greater than zero (0.6 recommended)\n");
        exit(1);
    }
    
    // allocate memory for main object
    detector_cccf q = (detector_cccf) malloc(sizeof(struct detector_cccf_s));
    unsigned int i;

    // set internal properties
    q->n         = _n;
    q->threshold = _threshold;

    // derived values
    q->n_inv = 1.0f / (float)(q->n);    // 1/n for faster processing

    // allocate memory for sequence and copy
    q->s = (float complex*) malloc((q->n)*sizeof(float complex));
    memmove(q->s, _s, q->n*sizeof(float complex));

    // create internal buffer
    q->buffer = windowcf_create(q->n);
    q->x2     = wdelayf_create(q->n);

    // create internal dot product object
    float complex sconj[q->n];
    for (i=0; i<q->n; i++)
        sconj[i] = conjf(q->s[i]);
    q->dp = dotprod_cccf_create(sconj, q->n);

    // reset state
    detector_cccf_reset(q);

#if DEBUG_DETECTOR
    q->debug_x   = windowcf_create(DEBUG_DETECTOR_BUFFER_LEN);
    q->debug_x2  = windowf_create(DEBUG_DETECTOR_BUFFER_LEN);
    q->debug_rxy = windowf_create(DEBUG_DETECTOR_BUFFER_LEN);
#endif
    // return object
    return q;
}

void detector_cccf_destroy(detector_cccf _q)
{
#if DEBUG_DETECTOR
    detector_cccf_debug_print(_q, DEBUG_DETECTOR_FILENAME);
    windowcf_destroy(_q->debug_x);
    windowf_destroy(_q->debug_x2);
    windowf_destroy(_q->debug_rxy);
#endif
    // destroy input buffer
    windowcf_destroy(_q->buffer);

    // destroy dot product
    dotprod_cccf_destroy(_q->dp);

    // destroy |x|^2 buffer
    wdelayf_destroy(_q->x2);

    // free internal buffers/arrays
    free(_q->s);

    // free main object memory
    free(_q);
}

void detector_cccf_print(detector_cccf _q)
{
    printf("detector_cccf:\n");
    printf("    sequence length     :   %-u\n", _q->n);
    printf("    threshold           :   %8.4f\n", _q->threshold);
}

void detector_cccf_reset(detector_cccf _q)
{
    // reset internal state
    windowcf_clear(_q->buffer);
    wdelayf_clear(_q->x2);

    // reset internal state
    _q->timer   = _q->n;                // reset timer
    _q->state   = DETECTOR_STATE_SEEK;  // set state to seek threshold
    _q->x2_sum  = 0.0f;
    _q->rxy_max = 0.0f;                 // 
    _q->rxy0    = 0.0f;
    _q->rxy1    = 0.0f;
    _q->rxy2    = 0.0f;
}

// Run sample through pre-demod detector's correlator.
// Returns '1' if signal was detected, '0' otherwise
//  _q          :   pre-demod detector
//  _x          :   input sample
//  _tau_hat    :   fractional sample offset estimate (set when detected)
//  _dphi_hat   :   carrier frequency offset estimate (set when detected)
//  _gamma_hat  :   channel gain estimate (set when detected)
int detector_cccf_correlate(detector_cccf _q,
                            float complex _x,
                            float *       _tau_hat,
                            float *       _dphi_hat,
                            float *       _gamma_hat)
{
    // push sample into buffer
    windowcf_push(_q->buffer, _x);

    // update estimate of signal magnitude
    float x2_n = crealf(_x * conjf(_x));    // |x[n-1]|^2 (input sample)
    float x2_0;                             // |x[0]  |^2 (oldest sample)
    wdelayf_read(_q->x2, &x2_0);            // read oldest sample
    wdelayf_push(_q->x2, x2_n);             // push newest sample
    _q->x2_sum = _q->x2_sum + x2_n - x2_0;  // update sum( |x|^2 ) of last 'n' input samples
#if 0
    // filtered estimate of E{ |x|^2 }
    _q->x2_hat = 0.8f*_q->x2_hat + 0.2f*_q->x2_sum*_q->n_inv;
#else
    // unfiltered estimate of E{ |x|^2 }
    _q->x2_hat = _q->x2_sum * _q->n_inv;
#endif

#if DEBUG_DETECTOR
    windowcf_push(_q->debug_x, _x);
    windowf_push(_q->debug_x2, _q->x2_hat);
#endif
    // check state
    if (_q->timer) {
        // hasn't timed out yet
        //printf("timer = %u\n", _q->timer);
        _q->timer--;
#if DEBUG_DETECTOR
        windowf_push(_q->debug_rxy, 0.0f);
#endif
        return 0;
    }

    // compute vector dot product
    float complex * r;
    float complex rxy;
    windowcf_read(_q->buffer, &r);
    dotprod_cccf_execute(_q->dp, r, &rxy);

    // scale by input signal magnitude
    // TODO: peridically re-compute scaling factor)
    float rxy_abs = cabsf(rxy) * _q->n_inv / sqrtf(_q->x2_hat);
    //float rxy_abs = cabsf(rxy);
#if DEBUG_DETECTOR
    windowf_push(_q->debug_rxy, rxy_abs);
#endif
    
    //printf("  rxy=%8.2f, x2-hat=%12.8f\n", rxy_abs, 10*log10f(_q->x2_hat));

    if (_q->state == DETECTOR_STATE_SEEK) {
        // check to see if value exceeds threshold
        if (rxy_abs > _q->threshold) {
            float dphi_hat = detector_cccf_estimate_dphi(_q);
            printf("threshold exceeded:      rxy = %8.4f, dphi=%8.4f\n", rxy_abs, dphi_hat);
            _q->rxy_max = rxy_abs;
            _q->rxy1    = rxy_abs;
            _q->state = DETECTOR_STATE_FINDMAX;
        } else {
            _q->rxy0 = rxy_abs;
        }
    } else if (_q->state == DETECTOR_STATE_FINDMAX) {
        // see if this new value exceeds maximum
        if (rxy_abs > _q->rxy_max) {
            float dphi_hat = detector_cccf_estimate_dphi(_q);
            printf("maximum not yet reached: rxy = %8.4f, dphi=%8.4f\n", rxy_abs, dphi_hat);
            _q->rxy_max = rxy_abs;

            // shift buffer...
            _q->rxy0 = _q->rxy1;
            _q->rxy1 = rxy_abs;
        } else {
            // peak was found last time; run estimates, reset values,
            // and return
            _q->rxy2 = rxy_abs;
            printf("maximum found:           rxy = %8.4f\n", rxy_abs);
            printf("    [%8.4f %8.4f %8.4f]\n", _q->rxy0, _q->rxy1, _q->rxy2);
            _q->rxy_max = 0.0f;
            _q->state = DETECTOR_STATE_SEEK;
            _q->timer = _q->n/4;            // set timer to allow signal to settle

            // set return values
            *_tau_hat = detector_cccf_estimate_tau(_q);
            return 1;
        }
    } else {
        fprintf(stderr,"error: detector_cccf_correlate(), unknown/unsupported internal state\n");
        exit(1);
    }

    return 0;
}

// 
// internal methods
//

// estimate timing
float detector_cccf_estimate_tau(detector_cccf _q)
{
    return 0.5f*(_q->rxy2 - _q->rxy0) / (_q->rxy2 + _q->rxy0 - 2*_q->rxy1);
}

float detector_cccf_estimate_dphi(detector_cccf _q)
{
    // read buffer...
    float complex * r;
    windowcf_read(_q->buffer, &r);

    //
    float complex r0 = 0.0f;
    float complex r1 = 0.0f;
    float complex metric = 0.0f;
    unsigned int i;
    for (i=0; i<_q->n; i++) {
        r0 = r1;
        r1 = r[i] * conjf(_q->s[i]);

        metric += r1 * conjf(r0);
    }

    return cargf(metric);
}

void detector_cccf_debug_print(detector_cccf _q,
                               const char *  _filename)
{
#if DEBUG_DETECTOR
    FILE * fid = fopen(_filename,"w");
    if (!fid) {
        fprintf(stderr,"error: detector_cccf_debug_print(), could not open '%s' for writing\n", _filename);
        return;
    }
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_DETECTOR_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"N = %u;\n", DEBUG_DETECTOR_BUFFER_LEN);
    unsigned int i;
    float complex * rc;
    float * r;

    fprintf(fid,"x = zeros(1,N);\n");
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_DETECTOR_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));

    fprintf(fid,"rxy = zeros(1,N);\n");
    windowf_read(_q->debug_rxy, &r);
    for (i=0; i<DEBUG_DETECTOR_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e;\n", i+1, r[i]);

    fprintf(fid,"x2 = zeros(1,N);\n");
    windowf_read(_q->debug_x2, &r);
    for (i=0; i<DEBUG_DETECTOR_BUFFER_LEN; i++)
        fprintf(fid,"x2(%4u) = %12.4e;\n", i+1, r[i]);

    fprintf(fid,"figure;\n");
    fprintf(fid,"t = 0:(N-1);\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t,real(x),t,imag(x));\n");
    fprintf(fid,"  ylabel('received signal, x');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t, rxy);\n");
    fprintf(fid,"  ylabel('rxy');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t, x2);\n");
    fprintf(fid,"  ylabel('rssi');\n");
    fprintf(fid,"  grid on;\n");

    fclose(fid);
    printf("detector_ccf/debug: results written to '%s'\n", _filename);
#else
    fprintf(stderr,"detector_cccf_debug_print(): compile-time debugging disabled\n");
#endif
}
