/*
 * Copyright (c) 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2010, 2011 Virginia Polytechnic
 *                          Institute & State University
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
// ofdmframesync.c
//
// OFDM frame synchronizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_OFDMFRAMESYNC             1
#define DEBUG_OFDMFRAMESYNC_PRINT       0
#define DEBUG_OFDMFRAMESYNC_FILENAME    "ofdmframesync_internal_debug.m"
#define DEBUG_OFDMFRAMESYNC_BUFFER_LEN  (2048)

#if DEBUG_OFDMFRAMESYNC
void ofdmframesync_debug_print(ofdmframesync _q);
#endif

struct ofdmframesync_s {
    unsigned int M;         // number of subcarriers
    unsigned int cp_len;    // cyclic prefix length
    unsigned int * p;       // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1

    // scaling factors
    float g_data;           // data symbols gain
    float g_S0;             // S0 training symbols gain
    float g_S1;             // S1 training symbols gain

    // transform object
    FFT_PLAN fft;           // ifft object
    float complex * X;      // frequency-domain buffer
    float complex * x;      // time-domain buffer
    windowcf input_buffer;  // input sequence buffer

    // PLCP sequences
    float complex * S0;     // short sequence (freq)
    float complex * s0;     // short sequence (time)
    float complex * S1;     // long sequence (freq)
    float complex * s1;     // long sequence (time)

    // gain
    float g0;               // nominal gain (coarse initial estimate)
    float complex * G0;     // complex subcarrier gain estimate, S1[0]
    float complex * G1;     // complex subcarrier gain estimate, S1[1]
    float complex * G;      // complex subcarrier gain estimate
    float complex * B;      // subcarrier phase rotation due to backoff

    // receiver state
    enum {
        OFDMFRAMESYNC_STATE_SEEKPLCP=0,   // seek initial PLSCP
        OFDMFRAMESYNC_STATE_PLCPSHORT0,   // seek first PLCP short sequence
        OFDMFRAMESYNC_STATE_PLCPSHORT1,   // seek second PLCP short sequence
        OFDMFRAMESYNC_STATE_PLCPLONG0,    // seek first PLCP long sequence
        OFDMFRAMESYNC_STATE_PLCPLONG1,    // seek second PLCP long sequence
        OFDMFRAMESYNC_STATE_RXSYMBOLS     // receive payload symbols
    } state;

    // synchronizer objects
    nco_crcf nco_rx;        // numerically-controlled oscillator
    autocorr_cccf autocorr; // auto-correlator
    dotprod_cccf crosscorr; // long sequence cross-correlator
    msequence ms_pilot;     // pilot sequence generator

    // coarse signal detection
    float squelch_threshold;
    int squelch_enabled;

    //
    float complex rxx_max;  // maximum auto-correlator output

    // timing
    unsigned int timer;         // input sample timer
    unsigned int num_symbols;   // symbol counter
    unsigned int backoff;       // sample timing backoff

    // callback
    ofdmframesync_callback callback;
    void * userdata;

#if DEBUG_OFDMFRAMESYNC
    agc_crcf agc_rx;        // automatic gain control (rssi)
    windowcf debug_x;
    windowcf debug_rxx;
    windowcf debug_rxy;
    windowf  debug_rssi;
    windowcf debug_framesyms;
#endif
};

ofdmframesync ofdmframesync_create(unsigned int _M,
                                   unsigned int _cp_len,
                                   unsigned int * _p,
                                   //unsigned int _taper_len,
                                   ofdmframesync_callback _callback,
                                   void * _userdata)
{
    ofdmframesync q = (ofdmframesync) malloc(sizeof(struct ofdmframesync_s));

    // validate input
    if (_M < 8) {
        fprintf(stderr,"warning: ofdmframesync_create(), less than 8 subcarriers\n");
    } else if (_M % 2) {
        fprintf(stderr,"error: ofdmframesync_create(), number of subcarriers must be even\n");
        exit(1);
    } else if (_cp_len > _M) {
        fprintf(stderr,"error: ofdmframesync_create(), cyclic prefix length cannot exceed number of subcarriers\n");
        exit(1);
    }
    q->M = _M;
    q->cp_len = _cp_len;

    // subcarrier allocation
    q->p = (unsigned int*) malloc((q->M)*sizeof(unsigned int));
    if (_p == NULL) {
        ofdmframe_init_default_sctype(q->M, q->p);
    } else {
        memmove(q->p, _p, q->M*sizeof(unsigned int));
    }

    // validate and count subcarrier allocation
    ofdmframe_validate_sctype(q->p, q->M, &q->M_null, &q->M_pilot, &q->M_data);
    if ( (q->M_pilot + q->M_data) == 0) {
        fprintf(stderr,"error: ofdmframesync_create(), must have at least one enabled subcarrier\n");
        exit(1);
    }

    // create transform object
    q->X = (float complex*) malloc((q->M)*sizeof(float complex));
    q->x = (float complex*) malloc((q->M)*sizeof(float complex));
    q->fft = FFT_CREATE_PLAN(q->M, q->x, q->X, FFT_DIR_FORWARD, FFT_METHOD);
 
    // create input buffer the length of the transform
    q->input_buffer = windowcf_create(q->M + q->cp_len);

    // allocate memory for PLCP arrays
    q->S0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->s0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->S1 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->s1 = (float complex*) malloc((q->M)*sizeof(float complex));
    ofdmframe_init_S0(q->p, q->M, q->S0, q->s0, &q->M_S0);
    ofdmframe_init_S1(q->p, q->M, q->S1, q->s1, &q->M_S1);

    // compute scaling factor
    q->g_data = sqrtf(q->M) / sqrtf(q->M_pilot + q->M_data);
    q->g_S0   = sqrtf(q->M) / sqrtf(q->M_S0);
    q->g_S1   = sqrtf(q->M) / sqrtf(q->M_S1);

    // gain
    q->g0 = 1.0f;
    q->G0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->G1 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->G  = (float complex*) malloc((q->M)*sizeof(float complex));
    q->B  = (float complex*) malloc((q->M)*sizeof(float complex));

#if 1
    memset(q->G0, 0x00, q->M*sizeof(float complex));
    memset(q->G1, 0x00, q->M*sizeof(float complex));
    memset(q->G , 0x00, q->M*sizeof(float complex));
    memset(q->B,  0x00, q->M*sizeof(float complex));
#endif

    // timing backoff
    q->backoff = q->cp_len < 2 ? q->cp_len : 2;
    float phi = (float)(q->backoff)*2.0f*M_PI/(float)(q->M);
    unsigned int i;
    for (i=0; i<q->M; i++)
        q->B[i] = liquid_cexpjf(i*phi);

    // set callback data
    q->callback = _callback;
    q->userdata = _userdata;

    // 
    // synchronizer objects
    //

    // numerically-controlled oscillator
    q->nco_rx = nco_crcf_create(LIQUID_VCO);

    // auto-correlator
    q->autocorr = autocorr_cccf_create(1.3*q->M, q->M / 2);

    // long sequence cross-correlator
    // compute conjugate s1 sequence, put into dotprod object
    for (i=0; i<q->M; i++)
        q->s1[i] = conjf(q->s1[i]);
    q->crosscorr = dotprod_cccf_create(q->s1, q->M);

    // set pilot sequence
    q->ms_pilot = msequence_create_default(8);

    // coarse detection
    q->squelch_threshold = -30.0f;
    q->squelch_enabled = 0;

    // reset object
    ofdmframesync_reset(q);

#if DEBUG_OFDMFRAMESYNC
    // agc, rssi, squelch
    q->agc_rx = agc_crcf_create();
    agc_crcf_set_target(q->agc_rx, 1.0f);
    agc_crcf_set_bandwidth(q->agc_rx,  1e-2f);
    agc_crcf_set_gain_limits(q->agc_rx, 1e-5f, 1e5f);

    q->debug_x =        windowcf_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    q->debug_rxx =      windowcf_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    q->debug_rxy =      windowcf_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    q->debug_rssi =     windowf_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    q->debug_framesyms =windowcf_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
#endif

    // return object
    return q;
}

void ofdmframesync_destroy(ofdmframesync _q)
{
#if DEBUG_OFDMFRAMESYNC
    ofdmframesync_debug_print(_q);

    agc_crcf_destroy(_q->agc_rx);

    windowcf_destroy(_q->debug_x);
    windowcf_destroy(_q->debug_rxx);
    windowcf_destroy(_q->debug_rxy);
    windowf_destroy(_q->debug_rssi);
    windowcf_destroy(_q->debug_framesyms);
#endif

    // free transform object
    windowcf_destroy(_q->input_buffer);
    free(_q->X);
    free(_q->x);
    FFT_DESTROY_PLAN(_q->fft);

    // clean up PLCP arrays
    free(_q->S0);
    free(_q->s0);
    free(_q->S1);
    free(_q->s1);

    // free gain arrays
    free(_q->G0);
    free(_q->G1);
    free(_q->G);
    free(_q->B);

    // destroy synchronizer objects
    nco_crcf_destroy(_q->nco_rx);           // numerically-controlled oscillator
    autocorr_cccf_destroy(_q->autocorr);    // auto-correlator
    dotprod_cccf_destroy(_q->crosscorr);    // cross-correlator
    msequence_destroy(_q->ms_pilot);

    // free main object memory
    free(_q);
}

void ofdmframesync_print(ofdmframesync _q)
{
    printf("ofdmframesync:\n");
    printf("    num subcarriers     :   %-u\n", _q->M);
    printf("    cyclic prefix len   :   %-u\n", _q->cp_len);
}

void ofdmframesync_reset(ofdmframesync _q)
{
#if 0
    // reset gain parameters
    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->G[i] = 1.0f;
#endif

    // reset synchronizer objects
    nco_crcf_reset(_q->nco_rx);
    autocorr_cccf_clear(_q->autocorr);
    msequence_reset(_q->ms_pilot);

    // reset internal state variables
    _q->rxx_max = 0.0f;

    // reset timers
    _q->timer = 0;
    _q->num_symbols = 0;

    // reset state
    _q->state = OFDMFRAMESYNC_STATE_SEEKPLCP;
}

void ofdmframesync_execute(ofdmframesync _q,
                           float complex * _x,
                           unsigned int _n)
{
    unsigned int i;
    float complex x;
    for (i=0; i<_n; i++) {
        x = _x[i];

        // correct for carrier frequency offset
        nco_crcf_mix_down(_q->nco_rx, x, &x);
        nco_crcf_step(_q->nco_rx);

        // save input sample to buffer
        windowcf_push(_q->input_buffer,x);

#if DEBUG_OFDMFRAMESYNC
        // apply agc (estimate initial signal gain)
        float complex y;
        agc_crcf_execute(_q->agc_rx, x, &y);

        windowcf_push(_q->debug_x, x);
        windowf_push(_q->debug_rssi, agc_crcf_get_signal_level(_q->agc_rx));
#endif

        switch (_q->state) {
        case OFDMFRAMESYNC_STATE_SEEKPLCP:
            ofdmframesync_execute_seekplcp(_q,x);
            break;
        case OFDMFRAMESYNC_STATE_PLCPSHORT0:
            ofdmframesync_execute_plcpshort0(_q,x);
            break;
        case OFDMFRAMESYNC_STATE_PLCPSHORT1:
            ofdmframesync_execute_plcpshort1(_q,x);
            break;
        case OFDMFRAMESYNC_STATE_PLCPLONG0:
            ofdmframesync_execute_plcplong0(_q,x);
            break;
        case OFDMFRAMESYNC_STATE_PLCPLONG1:
            ofdmframesync_execute_plcplong1(_q,x);
            break;
        case OFDMFRAMESYNC_STATE_RXSYMBOLS:
            ofdmframesync_execute_rxsymbols(_q,x);
            break;
        default:;
        }

    } // for (i=0; i<_n; i++)
} // ofdmframesync_execute()



//
// internal methods
//

// frame detection
void ofdmframesync_execute_seekplcp(ofdmframesync _q,
                                    float complex _x)
{
    _q->timer++;

    if (_q->timer < _q->M)
        return;

    // reset timer
    printf("timeout\n");
    _q->timer = 0;

    //
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);

    // estimate gain
    unsigned int i;
    float g = 0.0f;
    for (i=_q->cp_len; i<_q->M + _q->cp_len; i++)
        g += crealf( rc[i]*conjf(rc[i]) );
    g = 1.0f / sqrtf(g / (float)(_q->M) );
    g = g*g;

    // TODO : squelch here

    // run fft
    memmove(_q->x, &rc[_q->cp_len], (_q->M)*sizeof(float complex));
    FFT_EXECUTE(_q->fft);

    float complex g_hat;
    float complex s_hat;
    ofdmframesync_S0_metrics(_q, &g_hat, &s_hat);
    //float g = agc_crcf_get_gain(_q->agc_rx);
    s_hat *= g;
    g_hat *= g;

    float tau_hat  = cargf(s_hat) * (float)(_q->M) / (2*2*M_PI);
    printf("     gain   : %12.8f\n", sqrt(g));
    printf("     rssi   : %12.8f\n", -10*log10(sqrt(g)));
    printf("    s_hat   :   %12.8f <%12.8f>\n", cabsf(s_hat), cargf(s_hat));
    printf("    g_hat   :   %12.8f <%12.8f>\n", cabsf(g_hat), cargf(g_hat));
    printf("  tau_hat   :   %12.8f\n", tau_hat);

    // TODO : allow variable threshold
    if (cabsf(s_hat) > 0.65f) {
        printf("********** frame detected! ************\n");

        // save gain
        _q->g0 = g;

        int dt = (int)roundf(tau_hat);
        printf("dt = %5d\n", dt);
        if (dt == 0) {
            // continue
            _q->timer = _q->M/2;
            _q->state = OFDMFRAMESYNC_STATE_PLCPSHORT0;
        } else {
            // set timer appropriately...
            _q->timer = (_q->M + dt) % _q->M;
            _q->state = OFDMFRAMESYNC_STATE_PLCPSHORT0;
        }
        //printf("exiting prematurely\n");
        //ofdmframesync_destroy(_q);
        //exit(1);
    }

}

// frame detection
void ofdmframesync_execute_plcpshort0(ofdmframesync _q,
                                     float complex _x)
{
    _q->timer++;

    if (_q->timer < _q->M/2)
        return;

    // reset timer
    printf("********** S0[0] received ************\n");
    _q->timer = 0;

    //
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);

    // run fft
    memmove(_q->x, &rc[_q->cp_len], (_q->M)*sizeof(float complex));
    FFT_EXECUTE(_q->fft);

    float complex g_hat;
    float complex s_hat;
    ofdmframesync_S0_metrics(_q, &g_hat, &s_hat);
    //float g = agc_crcf_get_gain(_q->agc_rx);
    s_hat *= _q->g0;
    g_hat *= _q->g0;

    float tau_hat  = cargf(s_hat) * (float)(_q->M) / (2*2*M_PI);
    printf("    s_hat   :   %12.8f <%12.8f>\n", cabsf(s_hat), cargf(s_hat));
    printf("    g_hat   :   %12.8f <%12.8f>\n", cabsf(g_hat), cargf(g_hat));
    printf("  tau_hat   :   %12.8f\n", tau_hat);

    memmove(_q->G0, _q->G, _q->M*sizeof(float complex));
    _q->state = OFDMFRAMESYNC_STATE_PLCPSHORT1;
}

// frame detection
void ofdmframesync_execute_plcpshort1(ofdmframesync _q,
                                     float complex _x)
{
    _q->timer++;

    if (_q->timer < _q->M/2)
        return;

    printf("********** S0[1] received ************\n");
    // reset timer
    _q->timer = 0;

    //
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);

    // run fft
    memmove(_q->x, &rc[_q->cp_len], (_q->M)*sizeof(float complex));
    FFT_EXECUTE(_q->fft);

    float complex g_hat;
    float complex s_hat;
    ofdmframesync_S0_metrics(_q, &g_hat, &s_hat);
    //float g = agc_crcf_get_gain(_q->agc_rx);
    s_hat *= _q->g0;
    g_hat *= _q->g0;

    float tau_hat  = cargf(s_hat) * (float)(_q->M) / (2*2*M_PI);
    printf("    s_hat   :   %12.8f <%12.8f>\n", cabsf(s_hat), cargf(s_hat));
    printf("    g_hat   :   %12.8f <%12.8f>\n", cabsf(g_hat), cargf(g_hat));
    printf("  tau_hat   :   %12.8f\n", tau_hat);

    printf("**********\n");

    memmove(_q->G1, _q->G, _q->M*sizeof(float complex));

    g_hat = 0.0f;
    unsigned int i;
    for (i=0; i<_q->M; i++)
        g_hat += _q->G1[i] * conjf(_q->G0[i]);

    // carrier frequency offset estimate
    float nu_hat = 2.0f * cargf(g_hat) / (float)(_q->M);
    printf("   nu_hat   :   %12.8f\n", nu_hat);

    _q->state = OFDMFRAMESYNC_STATE_PLCPLONG0;
}

void ofdmframesync_execute_plcplong0(ofdmframesync _q,
                                     float complex _x)
{
    _q->timer++;

    if (_q->timer < _q->M)
        return;

    // reset timer
    _q->timer = 0;

    // run fft
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);
    memmove(_q->x, &rc[_q->cp_len], (_q->M)*sizeof(float complex));
    FFT_EXECUTE(_q->fft);

    // compute complex gains
    unsigned int i;
    float gain = sqrtf(_q->M_S1) / (float)(_q->M);
    for (i=0; i<_q->M; i++) {
        if (_q->p[i] != OFDMOQAMFRAME_SCTYPE_NULL) {
            // NOTE : if cabsf(_q->S0[i]) == 0 then we can multiply by conjugate
            //        rather than compute division
            //_q->G[i] = _q->X[i] / _q->S0[i];
            _q->G[i] = _q->X[i] * conjf(_q->S1[i]);
        } else {
            _q->G[i] = 0.0f;
        }

        // normalize gain
        _q->G[i] *= gain;
    }

    // compute detector output
    float complex g_hat = 0.0f;
    for (i=0; i<_q->M; i++) {
        //g_hat += _q->G[(i+1+_q->M)%_q->M]*conjf(_q->G[(i+_q->M)%_q->M]);
        g_hat += _q->G[(i+1)%_q->M]*conjf(_q->G[i]);
    }
    //g_hat /= _q->M_S1; // normalize output

    printf("    g_hat   :   %12.8f <%12.8f>\n", cabsf(g_hat), cargf(g_hat));

    if (cabsf(g_hat) > 4000.0f) {
        printf("exiting prematurely\n");
        ofdmframesync_destroy(_q);
        exit(1);
    }
#if 0
    if (cabsf(rxy) > 0.7f) {
#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("  rxy[0] = |%12.8f| {%12.8f}\n", cabsf(rxy), cargf(rxy));
#endif

        // reset timer
        _q->timer = 0;

        // compute complex gain on sequence
        ofdmframesync_estimate_gain_S1(_q, rc, _q->G0);

        // 
        _q->state = OFDMFRAMESYNC_STATE_PLCPLONG1;
    }

    // reset (false alarm) if timer is too large
    if (_q->timer > 4*_q->M) {
#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("ofdmframesync_execute_plcplong0(), could not find S1 symbol, resetting\n");
#endif
        ofdmframesync_reset(_q);
    }
#endif
}

void ofdmframesync_execute_plcplong1(ofdmframesync _q,
                                     float complex _x)
{
    _q->timer++;
    if (_q->timer != _q->M)
        return;

    // run cross-correlator
    float complex rxy, *rc;
    windowcf_read(_q->input_buffer, &rc);
    dotprod_cccf_execute(_q->crosscorr, &rc[_q->cp_len], &rxy);

    // scale
    rxy *= _q->g0 / sqrtf(_q->M);

#if DEBUG_OFDMFRAMESYNC
    windowcf_push(_q->debug_rxy,rxy);
    //printf("  rxy = |%12.8f| {%12.8f}\n", cabsf(rxy), cargf(rxy));
#endif

    if (cabsf(rxy) > 0.6f) {
#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("  rxy[1] = |%12.8f| {%12.8f}\n", cabsf(rxy), cargf(rxy));
#endif

        // reset timer
        _q->timer = 0;

        // compute complex gain on sequence
        ofdmframesync_estimate_gain_S1(_q, rc, _q->G1);

        // estimate residual carrier frequency offset and adjust nco
        float nu_hat;
#if 0
        unsigned int ntaps = _q->M / 2;
        ofdmframesync_estimate_eqgain(_q, ntaps, &nu_hat);
#else
        unsigned int poly_order = 8;
        if (poly_order >= _q->M_pilot + _q->M_data)
            poly_order = _q->M_pilot + _q->M_data - 1;
        ofdmframesync_estimate_eqgain_poly(_q, poly_order, &nu_hat);
#endif

#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("  nu_hat : %12.8f\n", nu_hat);
#endif
        nco_crcf_adjust_frequency(_q->nco_rx, nu_hat);

        // 
        _q->state = OFDMFRAMESYNC_STATE_RXSYMBOLS;
    } else {
        // reset (false alarm)
#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("ofdmframesync_execute_plcplong1(), could not find S1 symbol, resetting\n");
#endif
        ofdmframesync_reset(_q);
    }
}

void ofdmframesync_execute_rxsymbols(ofdmframesync _q,
                                     float complex _x)
{
    // wait for timeout
    _q->timer++;

    if (_q->timer == _q->M + _q->cp_len) {
        // run fft
        float complex * rc;
        windowcf_read(_q->input_buffer, &rc);
        memmove(_q->x, &rc[_q->cp_len-_q->backoff], (_q->M)*sizeof(float complex));
        FFT_EXECUTE(_q->fft);

        // recover symbol in internal _q->X buffer
        ofdmframesync_rxsymbol(_q);

#if DEBUG_OFDMFRAMESYNC
        unsigned int i;
        for (i=0; i<_q->M; i++) {
            if (_q->p[i] == OFDMFRAME_SCTYPE_DATA)
                windowcf_push(_q->debug_framesyms, _q->X[i]);
        }
#endif
        // invoke callback
        if (_q->callback != NULL) {
            int retval = _q->callback(_q->X, _q->p, _q->M, _q->userdata);

            if (retval != 0)
                ofdmframesync_reset(_q);
        }

        // reset timer
        _q->timer = 0;

    }

}

// compute S0 metrics
void ofdmframesync_S0_metrics(ofdmframesync _q,
                              float complex * _g_hat,
                              float complex * _s_hat)
{
    // timing, carrier offset correction
    unsigned int i;
    float complex g_hat = 0.0f;
    float complex s_hat = 0.0f;

    // compute complex gains
    float gain = sqrtf(_q->M_S0) / (float)(_q->M);
    for (i=0; i<_q->M; i++) {
        if (_q->p[i] != OFDMOQAMFRAME_SCTYPE_NULL && (i%2)==0) {
            // NOTE : if cabsf(_q->S0[i]) == 0 then we can multiply by conjugate
            //        rather than compute division
            //_q->G[i] = _q->X[i] / _q->S0[i];
            _q->G[i] = _q->X[i] * conjf(_q->S0[i]);
        } else {
            _q->G[i] = 0.0f;
        }

        // normalize gain
        _q->G[i] *= gain;
    }   

#if 0
    // compute carrier frequency offset metric
    for (i=0; i<_q->M; i++)
        g_hat += _q->G0[i] * conjf(_q->G1[i]);
    g_hat /= _q->M_S0; // normalize output
#endif

    // compute timing estimate, accumulate phase difference across
    // gains on subsequent pilot subcarriers
    // FIXME : need to assemble appropriate subcarriers
    for (i=0; i<_q->M; i++) {
        s_hat += _q->G[(i+2)%_q->M]*conjf(_q->G[i]);
    }
    s_hat /= _q->M_S0; // normalize output

    // set output values
    *_g_hat = g_hat;
    *_s_hat = s_hat;
}

// estimate long sequence gain
//  _q      :   ofdmframesync object
//  _x      :   input array (time), [size: M+cp_len x 1]
//  _G      :   output gain (freq)
void ofdmframesync_estimate_gain_S1(ofdmframesync _q,
                                    float complex * _x,
                                    float complex * _G)
{
    // move _x into fft input buffer
    memmove(_q->x, &_x[_q->cp_len-_q->backoff], (_q->M)*sizeof(float complex));

    // compute fft, storing result into _q->X
    FFT_EXECUTE(_q->fft);
    
    // compute gain, ignoring NULL subcarriers
    unsigned int i;
    for (i=0; i<_q->M; i++) {
        if (_q->p[i] == OFDMFRAME_SCTYPE_NULL)
            _G[i] = 0.0f;
        else
            _G[i] = _q->X[i] / _q->S1[i];

        // compensate for backoff
        _G[i] *= _q->B[i];
    }
}

// estimate residual carrier frequency offset from gain estimates
float ofdmframesync_estimate_nu_S1(ofdmframesync _q)
{
    float complex s = 0;

    // accumulate phase difference over subcarriers
    unsigned int i;
    for (i=0; i<_q->M; i++) {
        if (_q->p[i] != OFDMFRAME_SCTYPE_NULL)
            s += _q->G1[i] * conjf(_q->G0[i]);
    }

    // carrier frequency offset is argument of phase accumulation
    // over enabled subcarriers, relative to M samples of delay
    // between the gain estimates
    float nu_hat = cargf(s) / (float)(_q->M);

    return nu_hat;
}

// estimate complex equalizer gain from G0 and G1
//  _q      :   ofdmframesync object
//  _ntaps  :   number of time-domain taps for smoothing
//  _nu_hat :   residual phase difference between G0 and G1
void ofdmframesync_estimate_eqgain(ofdmframesync _q,
                                   unsigned int _ntaps,
                                   float * _nu_hat)
{
    // validate input
    if (_ntaps == 0 || _ntaps > _q->M) {
        fprintf(stderr, "error: ofdmframesync_estimate_eqgain(), ntaps must be in [1,M]\n");
        exit(1);
    }
    // estimate residual carrier frequency offset between
    // gain estimates G0 and G1
    float nu_hat = ofdmframesync_estimate_nu_S1(_q);
    *_nu_hat = nu_hat;

    // correct for phase difference in G1
    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->G1[i] *= cexpf(-_Complex_I*nu_hat*_q->M);

    // average equalizer gain
    for (i=0; i<_q->M; i++)
        _q->G[i] = 0.5f * (_q->G0[i] + _q->G1[i]);

#if 0
    // generate smoothing window (fft of temporal window)
    for (i=0; i<_q->M; i++)
        _q->x[i] = (i < _ntaps) ? 1.0f : 0.0f;
    FFT_EXECUTE(_q->fft);

    // smooth complex equalizer gains
    for (i=0; i<_q->M; i++) {
        // set gain to zero for null subcarriers
        if (_q->p[i] == OFDMFRAME_SCTYPE_NULL) {
            _q->G[i] = 0.0f;
            continue;
        }

        float complex w;
        float complex w0 = 0.0f;
        float complex G_hat = 0.0f;

        unsigned int j;
        for (j=0; j<_q->M; j++) {
            if (_q->p[j] == OFDMFRAME_SCTYPE_NULL) continue;

            // select window sample from array
            w = _q->X[(i + _q->M - j) % _q->M];

            // accumulate gain
            G_hat += w * 0.5f * (_q->G0[j] + _q->G1[j]);
            w0 += w;
        }

        // eliminate divide-by-zero issues
        if (cabsf(w0) < 1e-4f) {
            fprintf(stderr,"error: ofdmframesync_estimate_eqgain(), weighting factor is zero\n");
            w0 = 1.0f;
        }
        _q->G[i] = G_hat / w0;
    }
#endif
}

// estimate complex equalizer gain from G0 and G1 using polynomial fit
//  _q      :   ofdmframesync object
//  _order  :   polynomial order
//  _nu_hat :   residual phase difference between G0 and G1
void ofdmframesync_estimate_eqgain_poly(ofdmframesync _q,
                                        unsigned int _order,
                                        float * _nu_hat)
{
    // estimate residual carrier frequency offset between
    // gain estimates G0 and G1
    float nu_hat = ofdmframesync_estimate_nu_S1(_q);
    *_nu_hat = nu_hat;

    // correct for phase difference in G1
    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->G1[i] *= cexpf(-_Complex_I*nu_hat*_q->M);

    // average equalizer gain
    for (i=0; i<_q->M; i++)
        _q->G[i] = 0.5f * (_q->G0[i] + _q->G1[i]);

    // polynomial interpolation
    unsigned int N = _q->M_pilot + _q->M_data;
    if (_order > N-1) _order = N-1;
    if (_order > 10)  _order = 10;
    float x_freq[N];
    float y_abs[N];
    float y_arg[N];
    float p_abs[_order+1];
    float p_arg[_order+1];

    unsigned int n=0;
    unsigned int k;
    for (i=0; i<_q->M; i++) {

        // start at mid-point (effective fftshift)
        k = (i + _q->M/2) % _q->M;

        if (_q->p[k] != OFDMFRAME_SCTYPE_NULL) {
            if (n == N) {
                fprintf(stderr, "error: ofdmframesync_estimate_eqgain_poly(), pilot subcarrier mismatch\n");
                exit(1);
            }
            // store resulting...
            x_freq[n] = (k > _q->M/2) ? (float)k - (float)(_q->M) : (float)k;
            x_freq[n] = x_freq[n] / (float)(_q->M);
            y_abs[n] = cabsf(_q->G[k]);
            y_arg[n] = cargf(_q->G[k]);

            // update counter
            n++;
        }
    }

    if (n != N) {
        fprintf(stderr, "error: ofdmframesync_estimate_eqgain_poly(), pilot subcarrier mismatch\n");
        exit(1);
    }

    // try to unwrap phase
    for (i=1; i<N; i++) {
        while ((y_arg[i] - y_arg[i-1]) >  M_PI)
            y_arg[i] -= 2*M_PI;
        while ((y_arg[i] - y_arg[i-1]) < -M_PI)
            y_arg[i] += 2*M_PI;
    }

    // fit to polynomial
    polyf_fit(x_freq, y_abs, N, p_abs, _order+1);
    polyf_fit(x_freq, y_arg, N, p_arg, _order+1);

    // compute subcarrier gain
    for (i=0; i<_q->M; i++) {
        float freq = (i > _q->M/2) ? (float)i - (float)(_q->M) : (float)i;
        freq = freq / (float)(_q->M);
        float A     = polyf_val(p_abs, _order+1, freq);
        float theta = polyf_val(p_arg, _order+1, freq);
        _q->G[i] = (_q->p[i] == OFDMFRAME_SCTYPE_NULL) ? 0.0f : A * liquid_cexpjf(theta);
    }

#if 0
    for (i=0; i<N; i++)
        printf("x(%3u) = %12.8f; y_abs(%3u) = %12.8f; y_arg(%3u) = %12.8f;\n",
                i+1, x_freq[i],
                i+1, y_abs[i],
                i+1, y_arg[i]);

    for (i=0; i<=_order; i++)
        printf("p_abs(%3u) = %12.8f;\n", i+1, p_abs[i]);
    for (i=0; i<=_order; i++)
        printf("p_arg(%3u) = %12.8f;\n", i+1, p_arg[i]);
#endif
}

// recover symbol, correcting for gain, pilot phase, etc.
void ofdmframesync_rxsymbol(ofdmframesync _q)
{
    // apply gain
    unsigned int i;
    for (i=0; i<_q->M; i++) {
        //_q->X[i] *= _q->G[i] * _q->B[i];
        _q->X[i] *= _q->B[i] / _q->G[i];
    }

    // polynomial curve-fit
    float x_phase[_q->M_pilot];
    float y_phase[_q->M_pilot];
    float p_phase[2];

    // TODO : rotate pilot phase for every pilot, not just every symbol
    unsigned int pilot_phase = msequence_advance(_q->ms_pilot);
    unsigned int n=0;
    unsigned int k;
    float complex pilot = 1.0f;
    for (i=0; i<_q->M; i++) {

        // start at mid-point (effective fftshift)
        k = (i + _q->M/2) % _q->M;

        if (_q->p[k]==OFDMFRAME_SCTYPE_PILOT) {
            if (n == _q->M_pilot) {
                fprintf(stderr,"warning: ofdmframesync_rxsymbol(), pilot subcarrier mismatch\n");
                return;
            }
            pilot = (pilot_phase ? 1.0f : -1.0f);
#if 0
            printf("pilot[%3u] = %12.4e + j*%12.4e (expected %12.4e + j*%12.4e)\n",
                    i,
                    crealf(_q->X[i]), cimagf(_q->X[i]),
                    pilot_phase ? 1.0f : -1.0f, 0.0f);
#endif
            // store resulting...
            x_phase[n] = (k > _q->M/2) ? (float)k - (float)(_q->M) : (float)k;
            y_phase[n] = cargf(_q->X[k]*conjf(pilot));

            // update counter
            n++;

        }
    }

    if (n != _q->M_pilot) {
        fprintf(stderr,"warning: ofdmframesync_rxsymbol(), pilot subcarrier mismatch\n");
        return;
    }

    // try to unwrap phase
    for (i=1; i<_q->M_pilot; i++) {
        while ((y_phase[i] - y_phase[i-1]) >  M_PI)
            y_phase[i] -= 2*M_PI;
        while ((y_phase[i] - y_phase[i-1]) < -M_PI)
            y_phase[i] += 2*M_PI;
    }

    // fit phase to 1st-order polynomial (2 coefficients)
    polyf_fit(x_phase, y_phase, _q->M_pilot, p_phase, 2);

    // compensate for phase offset
    for (i=0; i<_q->M; i++) {
        float theta = polyf_val(p_phase, 2, (float)(i)-0.5f*(float)(_q->M));
        _q->X[i] *= liquid_cexpjf(-theta);
    }

#if DEBUG_OFDMFRAMESYNC_PRINT
    for (i=0; i<_q->M_pilot; i++)
        printf("x_phase(%3u) = %12.8f; y_phase(%3u) = %12.8f;\n", i+1, x_phase[i], i+1, y_phase[i]);
    printf("poly : p0=%12.8f, p1=%12.8f\n", p_phase[0], p_phase[1]);
#endif
}


#if DEBUG_OFDMFRAMESYNC
void ofdmframesync_debug_print(ofdmframesync _q)
{
    FILE * fid = fopen(DEBUG_OFDMFRAMESYNC_FILENAME,"w");
    if (!fid) {
        fprintf(stderr,"error: ofdmframe_debug_print(), could not open file for writing\n");
        return;
    }
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMFRAMESYNC_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %u;\n", DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    fprintf(fid,"M = %u;\n", _q->M);
    fprintf(fid,"M_null  = %u;\n", _q->M_null);
    fprintf(fid,"M_pilot = %u;\n", _q->M_pilot);
    fprintf(fid,"M_data  = %u;\n", _q->M_data);
    unsigned int i;
    float complex * rc;
    float * r;

    // save subcarrier allocation
    fprintf(fid,"p = zeros(1,M);\n");
    for (i=0; i<_q->M; i++)
        fprintf(fid,"p(%4u) = %d;\n", i+1, _q->p[i]);
    fprintf(fid,"i_null  = find(p==%d);\n", OFDMFRAME_SCTYPE_NULL);
    fprintf(fid,"i_pilot = find(p==%d);\n", OFDMFRAME_SCTYPE_PILOT);
    fprintf(fid,"i_data  = find(p==%d);\n", OFDMFRAME_SCTYPE_DATA);

    // short, long, training sequences
    for (i=0; i<_q->M; i++) {
        fprintf(fid,"S0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S0[i]), cimagf(_q->S0[i]));
        fprintf(fid,"S1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S1[i]), cimagf(_q->S1[i]));
    }

    fprintf(fid,"x = zeros(1,n);\n");
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_OFDMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(x),0:(n-1),imag(x));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");


    fprintf(fid,"rxx = zeros(1,n);\n");
    windowcf_read(_q->debug_rxx, &rc);
    for (i=0; i<DEBUG_OFDMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"   plot(0:(n-1),abs(rxx));\n");
    fprintf(fid,"   xlabel('sample index');\n");
    fprintf(fid,"   ylabel('auto-correlation, |rxx|');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"   plot(0:(n-1),arg(rxx));\n");
    fprintf(fid,"   xlabel('sample index');\n");
    fprintf(fid,"   ylabel('auto-correlation, arg[rxx]');\n");

    fprintf(fid,"rxy = zeros(1,n);\n");
    windowcf_read(_q->debug_rxy, &rc);
    for (i=0; i<DEBUG_OFDMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"   plot(0:(n-1),abs(rxy));\n");
    fprintf(fid,"   xlabel('sample index');\n");
    fprintf(fid,"   ylabel('cross-correlation, |rxy|');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"   plot(0:(n-1),arg(rxy));\n");
    fprintf(fid,"   xlabel('sample index');\n");
    fprintf(fid,"   ylabel('cross-correlation, arg[rxy]');\n");

    fprintf(fid,"s1 = [];\n");
    for (i=0; i<_q->M; i++)
        fprintf(fid,"s1(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->s1[i]), cimagf(_q->s1[i]));


    // write agc_rssi
    fprintf(fid,"\n\n");
    fprintf(fid,"agc_rssi = zeros(1,%u);\n", DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    windowf_read(_q->debug_rssi, &r);
    for (i=0; i<DEBUG_OFDMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(10*log10(agc_rssi))\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");

    // write short, long symbols
    fprintf(fid,"\n\n");
    fprintf(fid,"S0 = zeros(1,M);\n");
    fprintf(fid,"S1 = zeros(1,M);\n");
    for (i=0; i<_q->M; i++) {
        fprintf(fid,"S0(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->S0[i]), cimagf(_q->S0[i]));
        fprintf(fid,"S1(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->S1[i]), cimagf(_q->S1[i]));
    }


    // write gain arrays
    fprintf(fid,"\n\n");
    fprintf(fid,"G0 = zeros(1,M);\n");
    fprintf(fid,"G1 = zeros(1,M);\n");
    fprintf(fid,"G  = zeros(1,M);\n");
    for (i=0; i<_q->M; i++) {
        fprintf(fid,"G0(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G0[i]), cimagf(_q->G0[i]));
        fprintf(fid,"G1(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G1[i]), cimagf(_q->G1[i]));
        fprintf(fid,"G(%3u)  = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G[i]),  cimagf(_q->G[i]));
    }
    fprintf(fid,"f = [0:(M-1)] - (M/2);\n");
    //fprintf(fid,"G([i_data i_pilot]) = 1./G([i_data i_pilot]); %% invert gain\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(f, fftshift(abs(G0)),'-','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"       f, fftshift(abs(G1)),'-','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"       f, fftshift(abs(G)),'-','Color',[1 1 1]*0,'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('subcarrier index');\n");
    fprintf(fid,"  ylabel('gain estimate (mag)');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(f, fftshift(arg(G0)),'-','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"       f, fftshift(arg(G1)),'-','Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"       f, fftshift(arg(G)),'-','Color',[1 1 1]*0,'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('subcarrier index');\n");
    fprintf(fid,"  ylabel('gain estimate (phase)');\n");

    // write frame symbols
    fprintf(fid,"framesyms = zeros(1,n);\n");
    windowcf_read(_q->debug_framesyms, &rc);
    for (i=0; i<DEBUG_OFDMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(framesyms), imag(framesyms), 'x');\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.6);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("ofdmframesync/debug: results written to %s\n", DEBUG_OFDMFRAMESYNC_FILENAME);
}
#endif


