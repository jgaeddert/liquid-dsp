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

#define DEBUG_OFDMFRAMESYNC             0
#define DEBUG_OFDMFRAMESYNC_PRINT       0
#define DEBUG_OFDMFRAMESYNC_FILENAME    "ofdmframesync_internal_debug.m"
#define DEBUG_OFDMFRAMESYNC_BUFFER_LEN  (2048)

struct ofdmframesync_s {
    unsigned int M;         // number of subcarriers
    unsigned int M2;        // number of subcarriers (divided by 2)
    unsigned int cp_len;    // cyclic prefix length
    unsigned char * p;      // subcarrier allocation (null, pilot, data)

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
    float complex * R;      // 

    // receiver state
    enum {
        OFDMFRAMESYNC_STATE_SEEKPLCP=0,   // seek initial PLSCP
        OFDMFRAMESYNC_STATE_PLCPSHORT0,   // seek first PLCP short sequence
        OFDMFRAMESYNC_STATE_PLCPSHORT1,   // seek second PLCP short sequence
        OFDMFRAMESYNC_STATE_PLCPLONG,     // seek PLCP long sequence
        OFDMFRAMESYNC_STATE_RXSYMBOLS     // receive payload symbols
    } state;

    // synchronizer objects
    nco_crcf nco_rx;        // numerically-controlled oscillator
    msequence ms_pilot;     // pilot sequence generator
    float phi_prime;        // ...

    // coarse signal detection
    float squelch_threshold;
    int squelch_enabled;

    // timing
    unsigned int timer;         // input sample timer
    unsigned int num_symbols;   // symbol counter
    unsigned int backoff;       // sample timing backoff
    float complex s_hat_0;      // first S0 symbol metrics estimate
    float complex s_hat_1;      // second S0 symbol metrics estimate

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
                                   unsigned char * _p,
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

    // derived values
    q->M2 = _M/2;

    // subcarrier allocation
    q->p = (unsigned char*) malloc((q->M)*sizeof(unsigned char));
    if (_p == NULL) {
        ofdmframe_init_default_sctype(q->M, q->p);
    } else {
        memmove(q->p, _p, q->M*sizeof(unsigned char));
    }

    // validate and count subcarrier allocation
    ofdmframe_validate_sctype(q->p, q->M, &q->M_null, &q->M_pilot, &q->M_data);
    if ( (q->M_pilot + q->M_data) == 0) {
        fprintf(stderr,"error: ofdmframesync_create(), must have at least one enabled subcarrier\n");
        exit(1);
    } else if (q->M_pilot < 2) {
        fprintf(stderr,"error: ofdmframesync_create(), must have at least two pilot subcarriers\n");
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
    q->R  = (float complex*) malloc((q->M)*sizeof(float complex));

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

    // set pilot sequence
    q->ms_pilot = msequence_create_default(8);

    // coarse detection
    q->squelch_threshold = -25.0f;
    q->squelch_enabled = 0;

    // reset object
    ofdmframesync_reset(q);

#if DEBUG_OFDMFRAMESYNC
    // agc, rssi, squelch
    q->agc_rx = agc_crcf_create();
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
    ofdmframesync_debug_print(_q, DEBUG_OFDMFRAMESYNC_FILENAME);

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
    free(_q->R);

    // destroy synchronizer objects
    nco_crcf_destroy(_q->nco_rx);           // numerically-controlled oscillator
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
    msequence_reset(_q->ms_pilot);

    // reset timers
    _q->timer = 0;
    _q->num_symbols = 0;
    _q->s_hat_0 = 0.0f;
    _q->s_hat_1 = 0.0f;
    _q->phi_prime = 0.0f;

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
        windowf_push(_q->debug_rssi, agc_crcf_get_rssi(_q->agc_rx));
#endif

        switch (_q->state) {
        case OFDMFRAMESYNC_STATE_SEEKPLCP:
            ofdmframesync_execute_seekplcp(_q);
            break;
        case OFDMFRAMESYNC_STATE_PLCPSHORT0:
            ofdmframesync_execute_plcpshort0(_q);
            break;
        case OFDMFRAMESYNC_STATE_PLCPSHORT1:
            ofdmframesync_execute_plcpshort1(_q);
            break;
        case OFDMFRAMESYNC_STATE_PLCPLONG:
            ofdmframesync_execute_plcplong(_q);
            break;
        case OFDMFRAMESYNC_STATE_RXSYMBOLS:
            ofdmframesync_execute_rxsymbols(_q);
            break;
        default:;
        }

    } // for (i=0; i<_n; i++)
} // ofdmframesync_execute()

// get receiver RSSI
float ofdmframesync_get_rssi(ofdmframesync _q)
{
    return -10.0f*log10(sqrt(_q->g0));
}


//
// internal methods
//

// frame detection
void ofdmframesync_execute_seekplcp(ofdmframesync _q)
{
    _q->timer++;

    if (_q->timer < _q->M)
        return;

    // reset timer
    _q->timer = 0;

    //
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);

    // estimate gain
    unsigned int i;
    // TODO : decimate input ?
    float g = 0.0f;
    for (i=_q->cp_len; i<_q->M + _q->cp_len; i++)
        g += crealf( rc[i]*conjf(rc[i]) );
    g = 1.0f / sqrtf(g / (float)(_q->M) );
    g = g*g;

    // TODO : squelch here
    if ( -10*log10f( sqrtf(g) ) < _q->squelch_threshold &&
         _q->squelch_enabled)
    {
        printf("squelch\n");
        return;
    }

    // estimate S0 gain
    ofdmframesync_estimate_gain_S0(_q, &rc[_q->cp_len], _q->G0);

    float complex s_hat;
    ofdmframesync_S0_metrics(_q, _q->G0, &s_hat);
    //float g = agc_crcf_get_gain(_q->agc_rx);
    s_hat *= g;

    float tau_hat  = cargf(s_hat) * (float)(_q->M2) / (2*M_PI);
#if DEBUG_OFDMFRAMESYNC_PRINT
    printf(" - gain=%12.3f, rssi=%12.8f, s_hat=%12.4f <%12.8f>, tau_hat=%8.3f\n",
            sqrt(g),
            -10*log10(sqrt(g)),
            cabsf(s_hat), cargf(s_hat),
            tau_hat);
#endif

    // TODO : allow variable threshold
    if (cabsf(s_hat) > 0.35f) {

        // save gain
        _q->g0 = g;

        int dt = (int)roundf(tau_hat);
        // set timer appropriately...
        _q->timer = (_q->M + dt) % (_q->M2);
        _q->timer += _q->M; // add delay to help ensure good S0 estimate
        _q->state = OFDMFRAMESYNC_STATE_PLCPSHORT0;

#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("********** frame detected! ************\n");
        printf("    s_hat   :   %12.8f <%12.8f>\n", cabsf(s_hat), cargf(s_hat));
        printf("  tau_hat   :   %12.8f\n", tau_hat);
        printf("    dt      :   %12d\n", dt);
        printf("    timer   :   %12u\n", _q->timer);
#endif
        //printf("exiting prematurely\n");
        //ofdmframesync_destroy(_q);
        //exit(1);
    }

}

// frame detection
void ofdmframesync_execute_plcpshort0(ofdmframesync _q)
{
    //printf("t : %u\n", _q->timer);
    _q->timer++;

    if (_q->timer < _q->M2)
        return;

    // reset timer
    _q->timer = 0;

    //
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);

    // TODO : re-estimate nominal gain

    // estimate S0 gain
    ofdmframesync_estimate_gain_S0(_q, &rc[_q->cp_len], _q->G0);

    float complex s_hat;
    ofdmframesync_S0_metrics(_q, _q->G0, &s_hat);
    //float g = agc_crcf_get_gain(_q->agc_rx);
    s_hat *= _q->g0;

    _q->s_hat_0 = s_hat;

#if DEBUG_OFDMFRAMESYNC_PRINT
    float tau_hat  = cargf(s_hat) * (float)(_q->M2) / (2*M_PI);
    printf("********** S0[0] received ************\n");
    printf("    s_hat   :   %12.8f <%12.8f>\n", cabsf(s_hat), cargf(s_hat));
    printf("  tau_hat   :   %12.8f\n", tau_hat);
#endif

#if 0
    // TODO : also check for phase of s_hat (should be small)
    if (cabsf(s_hat) < 0.3f) {
        // false alarm
#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("false alarm S0[0]\n");
#endif
        ofdmframesync_reset(_q);
        return;
    }
#endif
    _q->state = OFDMFRAMESYNC_STATE_PLCPSHORT1;
}

// frame detection
void ofdmframesync_execute_plcpshort1(ofdmframesync _q)
{
    //printf("t = %u\n", _q->timer);
    _q->timer++;

    if (_q->timer < _q->M2)
        return;

    // reset timer
    _q->timer = _q->M + _q->cp_len - _q->backoff;

    //
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);

    // estimate S0 gain
    ofdmframesync_estimate_gain_S0(_q, &rc[_q->cp_len], _q->G1);

    float complex s_hat;
    ofdmframesync_S0_metrics(_q, _q->G1, &s_hat);
    //float g = agc_crcf_get_gain(_q->agc_rx);
    s_hat *= _q->g0;

    _q->s_hat_1 = s_hat;

#if DEBUG_OFDMFRAMESYNC_PRINT
    float tau_hat  = cargf(s_hat) * (float)(_q->M2) / (2*M_PI);
    printf("********** S0[1] received ************\n");
    printf("    s_hat   :   %12.8f <%12.8f>\n", cabsf(s_hat), cargf(s_hat));
    printf("  tau_hat   :   %12.8f\n", tau_hat);

    // new timing offset estimate
    tau_hat  = cargf(_q->s_hat_0 + _q->s_hat_1) * (float)(_q->M2) / (2*M_PI);
    printf("  tau_hat * :   %12.8f\n", tau_hat);

    printf("**********\n");
#endif

    // re-adjust timer accordingly
    float tau_prime = cargf(_q->s_hat_0 + _q->s_hat_1) * (float)(_q->M2) / (2*M_PI);
    _q->timer -= (int)roundf(tau_prime);

#if 0
    if (cabsf(s_hat) < 0.3f) {
#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("false alarm S0[1]\n");
#endif
        // false alarm
        ofdmframesync_reset(_q);
        return;
    }
#endif

    float complex g_hat = 0.0f;
    unsigned int i;
    for (i=0; i<_q->M; i++)
        g_hat += _q->G1[i] * conjf(_q->G0[i]);

#if 0
    // compute carrier frequency offset estimate using freq. domain method
    float nu_hat = 2.0f * cargf(g_hat) / (float)(_q->M);
#else
    // compute carrier frequency offset estimate using ML method
    float complex t0 = 0.0f;
    for (i=0; i<_q->M2; i++) {
        t0 += conjf(rc[i])       *       _q->s0[i] * 
                    rc[i+_q->M2] * conjf(_q->s0[i+_q->M2]);
    }
    float nu_hat = cargf(t0) / (float)(_q->M2);
#endif

#if DEBUG_OFDMFRAMESYNC_PRINT
    printf("   nu_hat   :   %12.8f\n", nu_hat);
#endif

    // set NCO frequency
    nco_crcf_set_frequency(_q->nco_rx, nu_hat);

    _q->state = OFDMFRAMESYNC_STATE_PLCPLONG;
}

void ofdmframesync_execute_plcplong(ofdmframesync _q)
{
    _q->timer--;

    if (_q->timer > 0)
        return;

    // increment number of symbols observed
    _q->num_symbols++;

    // run fft
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);

    // estimate S1 gain
    // TODO : add backoff in gain estimation
    ofdmframesync_estimate_gain_S1(_q, &rc[_q->cp_len], _q->G);

    // compute detector output
    float complex g_hat = 0.0f;
    unsigned int i;
    for (i=0; i<_q->M; i++) {
        //g_hat += _q->G[(i+1+_q->M)%_q->M]*conjf(_q->G[(i+_q->M)%_q->M]);
        g_hat += _q->G[(i+1)%_q->M]*conjf(_q->G[i]);
    }
    g_hat /= _q->M_S1; // normalize output
    g_hat *= _q->g0;

    // rotate by complex phasor relative to timing backoff
    g_hat *= liquid_cexpjf((float)(_q->backoff)*2.0f*M_PI/(float)(_q->M));

#if DEBUG_OFDMFRAMESYNC_PRINT
    printf("    g_hat   :   %12.4f <%12.8f>\n", cabsf(g_hat), cargf(g_hat));
#endif

    // check conditions for g_hat:
    //  1. magnitude should be large (near unity) when aligned
    //  2. phase should be very near zero (time aligned)
    if (cabsf(g_hat) > 0.3f && fabsf(cargf(g_hat)) < 0.1f*M_PI ) {
        //printf("    acquisition\n");
        _q->state = OFDMFRAMESYNC_STATE_RXSYMBOLS;
        // reset timer
        _q->timer = _q->M + _q->cp_len + _q->backoff;
        _q->num_symbols = 0;

        // normalize gain by...
        float phi = (float)(_q->backoff)*2.0f*M_PI/(float)(_q->M);
        for (i=0; i<_q->M; i++) {
            _q->G[i] /= sqrtf(_q->M_pilot + _q->M_data) / (float)(_q->M);
            _q->G[i] *= liquid_cexpjf(i*phi);
        }

#if 0
        // TODO : choose number of taps more appropriately
        //unsigned int ntaps = _q->M / 4;
        unsigned int ntaps = (_q->M < 8) ? 2 : 8;
        // FIXME : this is by far the most computationally complex part of synchronization
        ofdmframesync_estimate_eqgain(_q, ntaps);
#else
        unsigned int poly_order = 4;
        if (poly_order >= _q->M_pilot + _q->M_data)
            poly_order = _q->M_pilot + _q->M_data - 1;
        ofdmframesync_estimate_eqgain_poly(_q, poly_order);
#endif

#if 1
        // compute composite gain
        unsigned int i;
        for (i=0; i<_q->M; i++)
            _q->R[i] = _q->B[i] / _q->G[i];
#endif

        return;
#if 0
        printf("exiting prematurely\n");
        ofdmframesync_destroy(_q);
        exit(1);
#endif
    }

    // check if we are stuck searching for the S1 symbol
    if (_q->num_symbols == 16) {
#if DEBUG_OFDMFRAMESYNC_PRINT
        printf("could not find S1 symbol. bailing...\n");
#endif
        ofdmframesync_reset(_q);
    }

    // 'reset' timer (wait another half symbol)
    _q->timer = _q->M2;
}

void ofdmframesync_execute_rxsymbols(ofdmframesync _q)
{
    // wait for timeout
    _q->timer--;

    if (_q->timer == 0) {

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
        _q->timer = _q->M + _q->cp_len;
    }

}

// compute S0 metrics
void ofdmframesync_S0_metrics(ofdmframesync _q,
                              float complex * _G,
                              float complex * _s_hat)
{
    // timing, carrier offset correction
    unsigned int i;
    float complex s_hat = 0.0f;

    // compute timing estimate, accumulate phase difference across
    // gains on subsequent pilot subcarriers
    // FIXME : need to assemble appropriate subcarriers
    for (i=0; i<_q->M; i++) {
        s_hat += _G[(i+2)%_q->M]*conjf(_G[i]);
    }
    s_hat /= _q->M_S0; // normalize output

    // set output values
    *_s_hat = s_hat;
}

// estimate short sequence gain
//  _q      :   ofdmframesync object
//  _x      :   input array (time), [size: M x 1]
//  _G      :   output gain (freq)
void ofdmframesync_estimate_gain_S0(ofdmframesync _q,
                                    float complex * _x,
                                    float complex * _G)
{
    // move input array into fft input buffer
    memmove(_q->x, _x, (_q->M)*sizeof(float complex));

    // compute fft, storing result into _q->X
    FFT_EXECUTE(_q->fft);
    
    // compute gain, ignoring NULL subcarriers
    unsigned int i;
    float gain = sqrtf(_q->M_S0) / (float)(_q->M);

    for (i=0; i<_q->M; i++) {
        if (_q->p[i] != OFDMOQAMFRAME_SCTYPE_NULL && (i%2)==0) {
            // NOTE : if cabsf(_q->S0[i]) == 0 then we can multiply by conjugate
            //        rather than compute division
            //_G[i] = _q->X[i] / _q->S0[i];
            _G[i] = _q->X[i] * conjf(_q->S0[i]);
        } else {
            _G[i] = 0.0f;
        }

        // normalize gain
        _G[i] *= gain;
    }
}

// estimate long sequence gain
//  _q      :   ofdmframesync object
//  _x      :   input array (time), [size: M x 1]
//  _G      :   output gain (freq)
void ofdmframesync_estimate_gain_S1(ofdmframesync _q,
                                    float complex * _x,
                                    float complex * _G)
{
    // move input array into fft input buffer
    memmove(_q->x, _x, (_q->M)*sizeof(float complex));

    // compute fft, storing result into _q->X
    FFT_EXECUTE(_q->fft);
    
    // compute gain, ignoring NULL subcarriers
    unsigned int i;
    float gain = sqrtf(_q->M_S1) / (float)(_q->M);
    for (i=0; i<_q->M; i++) {
        if (_q->p[i] != OFDMOQAMFRAME_SCTYPE_NULL) {
            // NOTE : if cabsf(_q->S1[i]) == 0 then we can multiply by conjugate
            //        rather than compute division
            //_G[i] = _q->X[i] / _q->S1[i];
            _G[i] = _q->X[i] * conjf(_q->S1[i]);
        } else {
            _G[i] = 0.0f;
        }

        // normalize gain
        _G[i] *= gain;
    }   
}

// estimate complex equalizer gain from G0 and G1
//  _q      :   ofdmframesync object
//  _ntaps  :   number of time-domain taps for smoothing
void ofdmframesync_estimate_eqgain(ofdmframesync _q,
                                   unsigned int _ntaps)
{
    // validate input
    if (_ntaps == 0 || _ntaps > _q->M) {
        fprintf(stderr, "error: ofdmframesync_estimate_eqgain(), ntaps must be in [1,M]\n");
        exit(1);
    }

    unsigned int i;

    // generate smoothing window (fft of temporal window)
    for (i=0; i<_q->M; i++)
        _q->x[i] = (i < _ntaps) ? 1.0f : 0.0f;
    FFT_EXECUTE(_q->fft);

    memmove(_q->G0, _q->G, _q->M*sizeof(float complex));

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
            //G_hat += w * 0.5f * (_q->G0[j] + _q->G1[j]);
            G_hat += w * _q->G0[j];
            w0 += w;
        }

        // eliminate divide-by-zero issues
        if (cabsf(w0) < 1e-4f) {
            fprintf(stderr,"error: ofdmframesync_estimate_eqgain(), weighting factor is zero\n");
            w0 = 1.0f;
        }
        _q->G[i] = G_hat / w0;
    }
}

// estimate complex equalizer gain from G0 and G1 using polynomial fit
//  _q      :   ofdmframesync object
//  _order  :   polynomial order
void ofdmframesync_estimate_eqgain_poly(ofdmframesync _q,
                                        unsigned int _order)
{
    // polynomial interpolation
    unsigned int i;
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
        k = (i + _q->M2) % _q->M;

        if (_q->p[k] != OFDMFRAME_SCTYPE_NULL) {
            if (n == N) {
                fprintf(stderr, "error: ofdmframesync_estimate_eqgain_poly(), pilot subcarrier mismatch\n");
                exit(1);
            }
            // store resulting...
            x_freq[n] = (k > _q->M2) ? (float)k - (float)(_q->M) : (float)k;
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
        float freq = (i > _q->M2) ? (float)i - (float)(_q->M) : (float)i;
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
    for (i=0; i<_q->M; i++)
        _q->X[i] *= _q->R[i];

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
        k = (i + _q->M2) % _q->M;

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
            x_phase[n] = (k > _q->M2) ? (float)k - (float)(_q->M) : (float)k;
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
    // TODO : find more computationally efficient way to do this
    for (i=0; i<_q->M; i++) {
        // only apply to data/pilot subcarriers
        if (_q->p[i] == OFDMFRAME_SCTYPE_NULL) {
            _q->X[i] = 0.0f;
        } else {
            float theta = polyf_val(p_phase, 2, (float)(i)-0.5f*(float)(_q->M));
            _q->X[i] *= liquid_cexpjf(-theta);
        }
    }

    // adjust NCO frequency based on differential phase
    if (_q->num_symbols > 0) {
        // compute phase error (unwrapped)
        float dphi_prime = p_phase[0] - _q->phi_prime;
        while (dphi_prime >  M_PI) dphi_prime -= M_2_PI;
        while (dphi_prime < -M_PI) dphi_prime += M_2_PI;

        // adjust NCO proportionally to phase error
        nco_crcf_adjust_frequency(_q->nco_rx, 1e-3f*dphi_prime);
    }
    // set internal phase state
    _q->phi_prime = p_phase[0];
    //printf("%3u : theta : %12.8f, nco freq: %12.8f\n", _q->num_symbols, p_phase[0], nco_crcf_get_frequency(_q->nco_rx));
    
    // increment symbol counter
    _q->num_symbols++;

#if 0
    for (i=0; i<_q->M_pilot; i++)
        printf("x_phase(%3u) = %12.8f; y_phase(%3u) = %12.8f;\n", i+1, x_phase[i], i+1, y_phase[i]);
    printf("poly : p0=%12.8f, p1=%12.8f\n", p_phase[0], p_phase[1]);
#endif
}


void ofdmframesync_debug_print(ofdmframesync _q,
                               const char * _filename)
{
    FILE * fid = fopen(_filename,"w");
    if (!fid) {
        fprintf(stderr,"error: ofdmframe_debug_print(), could not open '%s' for writing\n", _filename);
        return;
    }
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMFRAMESYNC_FILENAME);
#if DEBUG_OFDMFRAMESYNC
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
    fprintf(fid,"plot(agc_rssi)\n");
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
#else
    fprintf(fid,"disp('no debugging info available');\n");
#endif

    fclose(fid);
    printf("ofdmframesync/debug: results written to '%s'\n", _filename);
}


