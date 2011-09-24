/*
 * Copyright (c) 2010 Joseph Gaeddert
 * Copyright (c) 2010 Virginia Polytechnic Institute & State University
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
// ofdmoqamframesync.c
//
// OFDM/OQAM frame synchronizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_OFDMOQAMFRAMESYNC             1
#define DEBUG_OFDMOQAMFRAMESYNC_PRINT       1
#define DEBUG_OFDMOQAMFRAMESYNC_FILENAME    "ofdmoqamframesync_internal_debug.m"
#define DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN  (2048)

#if DEBUG_OFDMOQAMFRAMESYNC
void ofdmoqamframesync_debug_print(ofdmoqamframesync _q);
#endif

struct ofdmoqamframesync_s {
    unsigned int M;         // number of subcarriers
    unsigned int m;         // filter delay (symbols)
    float beta;             // filter excess bandwidth factor
    int ftype;              // filter prototype (e.g. LIQUID_RNYQUIST_ARKAISER)
    unsigned char * p;      // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M2;        // M/2 (time delay)
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1

    // scaling factors
    float g_data;           // data symbols gain
    float g_S0;             // S0 training symbols gain
    float g_S1;             // S1 training symbols gain

    // filterbank objects
    firpfbch_crcf ca0;      // upper analysis filterbank
    firpfbch_crcf ca1;      // lower analysis filterbank

    // generic transform buffers
    float complex * X0;     // upper analysis filterbank output
    float complex * X1;     // lower analysis filterbank output

    // 
    float complex * S0;     // short sequence
    float complex * S1;     // long sequence

    // gain
    float g;                // coarse gain estimate
    float * wg;             // gain estimation window
    float complex * G0;     // complex subcarrier gain estimate, S2[0]
    float complex * G1;     // complex subcarrier gain estimate, S2[1]
    float complex * G;      // complex subcarrier gain estimate
    float complex * Y;      // output symbols

    // receiver state
    enum {
        OFDMOQAMFRAMESYNC_STATE_PLCPSHORT=0,  // seek PLCP short sequence
        OFDMOQAMFRAMESYNC_STATE_PLCPLONG0,    // seek first PLCP long sequence
        OFDMOQAMFRAMESYNC_STATE_PLCPLONG1,    // seek second PLCP long sequence
        OFDMOQAMFRAMESYNC_STATE_RXSYMBOLS     // receive payload symbols
    } state;

    // synchronizer objects
    agc_crcf agc_rx;        // automatic gain control
    float dphi_prime;       // initial carrier frequency offset
    float dphi_hat;         // carrier frequency offset estimate
    float phi;              // carrier phase

    // counters, timers, etc.
    windowcf input_buffer;  // input sequence buffer
    windowcf delay_buffer;  // input delay buffer
    unsigned int timer;     // input sample timer
    unsigned int k;         // analyzer alignment (timing)
    unsigned int num_S0;    // number of full S0 symbols detected

    // callback
    ofdmoqamframesync_callback callback;
    void * userdata;

#if DEBUG_OFDMOQAMFRAMESYNC
    windowcf debug_x;
    windowcf debug_agc_out;
    windowf  debug_rssi;
    windowcf debug_framesyms;
#endif
};

ofdmoqamframesync ofdmoqamframesync_create(unsigned int _M,
                                           unsigned int _m,
                                           float _beta,
                                           unsigned char * _p,
                                           ofdmoqamframesync_callback _callback,
                                           void * _userdata)
{
    ofdmoqamframesync q = (ofdmoqamframesync) malloc(sizeof(struct ofdmoqamframesync_s));

    // validate input
    if (_M % 2) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), number of subcarriers must be even\n");
        exit(1);
    } else if (_M < 8) {
        fprintf(stderr,"warning: ofdmoqamframesync_create(), less than 8 subcarriers\n");
        exit(1);
    } else if (_m < 1) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), filter delay must be > 0\n");
        exit(1);
    } else if (_beta < 0.0f) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), filter excess bandwidth must be > 0\n");
        exit(1);
    }
    q->M = _M;
    q->m = _m;
    q->beta = _beta;

    // subcarrier allocation
    q->p = (unsigned char*) malloc((q->M)*sizeof(unsigned char));
    if (_p == NULL) {
        ofdmoqamframe_init_default_sctype(q->M, q->p);
    } else {
        memmove(q->p, _p, q->M*sizeof(unsigned char));
    }

    // validate and count subcarrier allocation
    ofdmoqamframe_validate_sctype(q->p, q->M, &q->M_null, &q->M_pilot, &q->M_data);
    if ( (q->M_pilot + q->M_data) == 0) {
        fprintf(stderr,"error: ofdmoqamframesync_create(), must have at least one enabled subcarrier\n");
        exit(1);
    }

    // default/derived values
    q->ftype = LIQUID_RNYQUIST_ARKAISER;
    q->M2 = q->M/2;
    
    // create analysis filter banks
    q->ca0 = firpfbch_crcf_create_rnyquist(LIQUID_ANALYZER, q->M, q->m, q->beta, q->ftype);
    q->ca1 = firpfbch_crcf_create_rnyquist(LIQUID_ANALYZER, q->M, q->m, q->beta, q->ftype);
    q->X0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->X1 = (float complex*) malloc((q->M)*sizeof(float complex));
 
    // allocate memory for PLCP arrays
    q->S0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->S1 = (float complex*) malloc((q->M)*sizeof(float complex));
    ofdmoqamframe_init_S0(q->p, q->M, q->S0, &q->M_S0);
    ofdmoqamframe_init_S1(q->p, q->M, q->S1, &q->M_S1);

    // compute scaling factor
    //q->g_data = q->M / sqrtf(q->M_pilot + q->M_data);
    q->g_data = sqrtf(q->M) / sqrtf(q->M_pilot + q->M_data);
    q->g_S0   = sqrtf(q->M) / sqrtf(q->M_S0);
    q->g_S1   = sqrtf(q->M) / sqrtf(q->M_S1);
#if 0
    printf("M(S0) = %u\n", q->M_S0);
    printf("g(S0) = %12.8f\n", q->g_S0);
    printf("M(S1) = %u\n", q->M_S1);
    printf("g(S1) = %12.8f\n", q->g_S1);
#endif

    // create delay buffer
    q->delay_buffer = windowcf_create((q->M));

    // create input buffer the length of the filter prototype
    q->input_buffer = windowcf_create(2*(q->M)*(q->m));

    // gain
    q->g = 1.0f;
    q->G0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->G1 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->G  = (float complex*) malloc((q->M)*sizeof(float complex));
    q->Y  = (float complex*) malloc((q->M)*sizeof(float complex));

    // create/initialize gain estimation window
    q->wg = (float*) malloc((q->M)*sizeof(float));
    ofdmoqamframesync_init_gain_window(q, 0.02f);

    // set callback data
    q->callback = _callback;
    q->userdata = _userdata;

    // 
    // synchronizer objects
    //

    // agc, rssi, squelch
    q->agc_rx = agc_crcf_create();
    agc_crcf_set_bandwidth(q->agc_rx,  1e-3f);
    agc_crcf_set_gain_limits(q->agc_rx, 1e-3f, 1e4f);

    agc_crcf_squelch_activate(q->agc_rx);
    agc_crcf_squelch_set_threshold(q->agc_rx, -35.0f);
    agc_crcf_squelch_set_timeout(q->agc_rx, 32);

    agc_crcf_squelch_enable_auto(q->agc_rx);

    // reset object
    ofdmoqamframesync_reset(q);

#if DEBUG_OFDMOQAMFRAMESYNC
    q->debug_x =        windowcf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_agc_out =  windowcf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_rssi =     windowf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    q->debug_framesyms =windowcf_create(DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
#endif

    // return object
    return q;
}

void ofdmoqamframesync_destroy(ofdmoqamframesync _q)
{
#if DEBUG_OFDMOQAMFRAMESYNC
    ofdmoqamframesync_debug_print(_q);
    windowcf_destroy(_q->debug_x);
    windowcf_destroy(_q->debug_agc_out);
    windowf_destroy(_q->debug_rssi);
    windowcf_destroy(_q->debug_framesyms);
#endif

    // free analysis filterbank objects
    firpfbch_crcf_destroy(_q->ca0);
    firpfbch_crcf_destroy(_q->ca1);
    free(_q->X0);
    free(_q->X1);

    // clean up PLCP arrays
    free(_q->S0);
    free(_q->S1);

    // free gain arrays
    free(_q->wg);
    free(_q->G0);
    free(_q->G1);
    free(_q->G);
    free(_q->Y);

    // free input/delay buffers
    windowcf_destroy(_q->input_buffer);
    windowcf_destroy(_q->delay_buffer);

    // destroy synchronizer objects
    agc_crcf_destroy(_q->agc_rx);      // automatic gain control

    // free main object memory
    free(_q);
}

void ofdmoqamframesync_print(ofdmoqamframesync _q)
{
    printf("ofdmoqamframesync:\n");
    printf("    num subcarriers     :   %-u\n", _q->M);
    printf("    m (filter delay)    :   %-u\n", _q->m);
    printf("    beta (excess b/w)   :   %8.6f\n", _q->beta);
}

void ofdmoqamframesync_reset(ofdmoqamframesync _q)
{
    // clear input/delay buffer
    windowcf_clear(_q->delay_buffer);
    windowcf_clear(_q->input_buffer);

    _q->timer = 0;
    _q->k = 0;
    _q->num_S0 = 0;

    // clear analysis filter bank objects
    firpfbch_crcf_clear(_q->ca0);
    firpfbch_crcf_clear(_q->ca1);

    // reset gain parameters
    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->G[i] = 1.0f;

    // reset synchronizer objects
    agc_crcf_unlock(_q->agc_rx);    // automatic gain control (unlock)
    _q->dphi_prime = 0.0f;
    _q->dphi_hat   = 0.0f;
    _q->phi = 0.0f;

    // reset state
    _q->state = OFDMOQAMFRAMESYNC_STATE_PLCPSHORT;
}

void ofdmoqamframesync_execute(ofdmoqamframesync _q,
                                 float complex * _x,
                                 unsigned int _n)
{
    unsigned int i;
    float complex x;
    float complex y;
    int squelch_status;
    for (i=0; i<_n; i++) {
        x = _x[i];

        // correct for initial carrier frequency offset
        x *= liquid_cexpjf(_q->phi);
        _q->phi += _q->dphi_prime;

        // save input sample (only during initial state)
        if (_q->state == OFDMOQAMFRAMESYNC_STATE_PLCPSHORT)
            windowcf_push(_q->input_buffer,x);

        // apply agc (estimate initial signal gain)
        agc_crcf_execute(_q->agc_rx, x, &y);

#if DEBUG_OFDMOQAMFRAMESYNC
        windowcf_push(_q->debug_x, x);
        windowcf_push(_q->debug_agc_out, y);
        windowf_push(_q->debug_rssi, agc_crcf_get_signal_level(_q->agc_rx));
#endif

        // squelch: block agc output from synchronizer only if
        // 1. received signal strength indicator has not exceeded squelch
        //    threshold at any time within the past <squelch_timeout> samples
        // 2. mode is to seek preamble
        squelch_status = agc_crcf_squelch_get_status(_q->agc_rx);
        if (squelch_status == LIQUID_AGC_SQUELCH_ENABLED)
            continue;
        
        // push sample into analysis filter banks
        float complex x_delay0;
        float complex x_delay1;
        windowcf_index(_q->delay_buffer, 0,      &x_delay0); // full symbol delay
        windowcf_index(_q->delay_buffer, _q->M2, &x_delay1); // half symbol delay

        windowcf_push(_q->delay_buffer,x);

        firpfbch_crcf_analyzer_push(_q->ca0, x_delay0);  // push input sample
        firpfbch_crcf_analyzer_push(_q->ca1, x_delay1);  // push delayed sample

        switch (_q->state) {
        case OFDMOQAMFRAMESYNC_STATE_PLCPSHORT:
            ofdmoqamframesync_execute_plcpshort(_q,x);
            break;
        case OFDMOQAMFRAMESYNC_STATE_PLCPLONG0:
            ofdmoqamframesync_execute_plcplong0(_q,x);
            break;
        case OFDMOQAMFRAMESYNC_STATE_PLCPLONG1:
            ofdmoqamframesync_execute_plcplong1(_q,x);
            break;
        case OFDMOQAMFRAMESYNC_STATE_RXSYMBOLS:
            ofdmoqamframesync_execute_rxsymbols(_q,x);
            break;
        default:;
        }

    } // for (i=0; i<_n; i++)
} // ofdmoqamframesync_execute()

//
// internal
//

void ofdmoqamframesync_execute_plcpshort(ofdmoqamframesync _q,
                                         float complex _x)
{
    // wait for timeout
    _q->timer++;
    if ( ((_q->timer + _q->k) % _q->M ) == 0) {
        //printf("timeout\n");

        // run analysis filters
        firpfbch_crcf_analyzer_run(_q->ca0, _q->k, _q->X0);
        firpfbch_crcf_analyzer_run(_q->ca1, _q->k, _q->X1);

        // compute metrics
        float complex g0_hat;
        float complex s0_hat;
        ofdmoqamframesync_S0_metrics(_q, &g0_hat, &s0_hat);

#if 0
        // fixed gain
        _q->g = 1.0f;
#else
        // gain from automatic gain control
        _q->g = agc_crcf_get_gain(_q->agc_rx);
#endif
        g0_hat *= _q->g * _q->g;
        s0_hat *= _q->g * _q->g;

        // compute carrier frequency offset estimate
        float dphi_hat = cargf(g0_hat) / (float)(_q->M2);
        float tau_hat  = cargf(s0_hat) * (float)(_q->M) / (2*2*M_PI);

        int dt = 0;
        // adjust timing
        if (cabsf(g0_hat) > 0.7f) {
            // lock AGC
            agc_crcf_lock(_q->agc_rx);

            // estimate sample timing offset
            dt = (int) roundf(tau_hat);
            _q->k = (_q->k + _q->M + dt) % _q->M;
            //printf(" k : %3u (dt = %3d)\n", k, dt);

            if (dt >= 0) {
                // increment S0 counter
                _q->num_S0++;
            }

            if (_q->num_S0 == _q->m+1) {

                // estimate carrier frequency offset
                dphi_hat = cargf(g0_hat) / (float)(_q->M2);
                _q->dphi_prime = dphi_hat;
#if DEBUG_OFDMOQAMFRAMESYNC_PRINT
                printf("dphi-prime : %16.12f\n", _q->dphi_prime);
#endif

                // correct filter buffer using initial carrier
                // frequency offset estimate (dphi_prime)
                ofdmoqamframesync_correct_buffer(_q);

                // re-run analysis filters
                firpfbch_crcf_analyzer_run(_q->ca0, _q->k, _q->X0);
                firpfbch_crcf_analyzer_run(_q->ca1, _q->k, _q->X1);

                // re-compute metrics
                ofdmoqamframesync_S0_metrics(_q, &g0_hat, &s0_hat);
                g0_hat *= _q->g * _q->g;
                s0_hat *= _q->g * _q->g;

                // re-estimate carrier frequency offset
                dphi_hat = cargf(g0_hat) / (float)(_q->M2);
                _q->dphi_hat = dphi_hat;
#if DEBUG_OFDMOQAMFRAMESYNC_PRINT
                printf("dphi-hat   : %16.12f\n", _q->dphi_hat);
#endif

                // estimate gain
                ofdmoqamframesync_estimate_gain(_q, _q->G0, _q->G);
                
                _q->state = OFDMOQAMFRAMESYNC_STATE_PLCPLONG0;
            }
        } else {
            // false alarm: reset counter, unlock agc
            _q->num_S0 = 0;
            agc_crcf_unlock(_q->agc_rx);
        }

#if DEBUG_OFDMOQAMFRAMESYNC_PRINT
        float complex t0_hat = 0.0f;
        float complex t1_hat = 0.0f;

        //printf("%4u|%4u: %c g |%7.4f|, dphi: %7.4f, tau: %7.3f, k=%2u, dt=%3d, %c t0[%7.4f], %c t1[%7.4f]\n",
        printf("%6u : %c g |%7.4f|, dphi: %7.4f, tau: %7.3f, %c t0[%7.4f], %c t1[%7.4f]\n",
                _q->timer,
                cabsf(g0_hat) > 0.7f ? '*' : ' ',
                cabsf(g0_hat),
                dphi_hat,
                tau_hat,
                //k, dt, 
                cabsf(t0_hat) > 0.7f ? '*' : ' ',
                cabsf(t0_hat),
                cabsf(t1_hat) > 0.7f ? '*' : ' ',
                cabsf(t1_hat));
#endif

    }
}

void ofdmoqamframesync_execute_plcplong0(ofdmoqamframesync _q,
                                         float complex _x)
{
    // wait for timeout
    _q->timer++;
    if ( ((_q->timer + _q->k) % _q->M ) == 0) {
        //printf("timeout\n");

        // run analysis filters
        firpfbch_crcf_analyzer_run(_q->ca0, _q->k, _q->X0);
        firpfbch_crcf_analyzer_run(_q->ca1, _q->k, _q->X1);

        // compute S1 metrics
        float complex t0_hat;
        float complex t1_hat;
        ofdmoqamframesync_S1_metrics(_q, &t0_hat, &t1_hat);

        if (cabsf(t0_hat) > 0.7f) {
            // symbol properly aligned with analysis filter bank
#if DEBUG_OFDMOQAMFRAMESYNC_PRINT
            printf("long sequence detected [t0] |%12.8f| {%12.8f}\n", cabsf(t0_hat), cargf(t0_hat));
#endif

            // set internal phase
            //_q->phi = -cargf(t0_hat);

            _q->state = OFDMOQAMFRAMESYNC_STATE_RXSYMBOLS;
        } else if (cabsf(t1_hat) > 0.7f) {
            // sample timing off by half a symbol
#if DEBUG_OFDMOQAMFRAMESYNC_PRINT
            printf("long sequence detected [t1] |%12.8f| {%12.8f}\n", cabsf(t1_hat), cargf(t1_hat));
#endif
            _q->k = (_q->k + _q->M2) % _q->M;

            // re-compute gain on G1
            ofdmoqamframesync_estimate_gain(_q, _q->G1, _q->G);
            //_q->state = OFDMOQAMFRAMESYNC_STATE_RXSYMBOLS;
        }

    }

}

void ofdmoqamframesync_execute_plcplong1(ofdmoqamframesync _q,
                                         float complex _x)
{
}

void ofdmoqamframesync_execute_rxsymbols(ofdmoqamframesync _q,
                                         float complex _x)
{
    // wait for timeout
    _q->timer++;
    if ( ((_q->timer + _q->k) % _q->M ) == 0) {

        // run analysis filters
        firpfbch_crcf_analyzer_run(_q->ca0, _q->k, _q->X0);
        firpfbch_crcf_analyzer_run(_q->ca1, _q->k, _q->X1);

        // apply channel gain
        unsigned int i;
        for (i=0; i<_q->M; i++) {
            _q->X0[i] *= _q->G[i];
            _q->X1[i] *= _q->G[i];
        }

#if 0
        // remove carrier frequency/phase offset, ensuring appropriate
        // phase rotation, compensating for time delay between upper
        // and lower analysis banks
        _q->phi += _q->dphi_hat * _q->M;
        float complex g0 = liquid_cexpjf(_q->phi);
        float complex g1 = liquid_cexpjf(_q->phi + _q->dphi_hat*_q->M2);
        for (i=0; i<_q->M; i++) {
            _q->X0[i] *= g0;
            _q->X1[i] *= g1;
        }
#endif

        // TODO : extract pilots, track carrier phase offset

        // recover time-aligned symbols
        float gain = sqrtf(_q->M_data) / (float)(_q->M);
        for (i=0; i<_q->M; i++) {
            if ( (i%2) == 0) {
                // even subcarrier
                _q->Y[i] = crealf(_q->X0[i]) + _Complex_I*cimagf(_q->X1[i]);
            } else {
                // odd subcarrier
                _q->Y[i] = crealf(_q->X1[i]) + _Complex_I*cimagf(_q->X0[i]);
            }

            // apply gain
            _q->Y[i] *= gain;

#if DEBUG_OFDMOQAMFRAMESYNC
            if (_q->p[i] == OFDMOQAMFRAME_SCTYPE_DATA)
                windowcf_push(_q->debug_framesyms, _q->Y[i]);
#endif
        }

        // invoke callback
        int rc = _q->callback(_q->Y, _q->userdata);
        if (rc == 0) {
            // continue receiving frames...
        } else {
            printf("ofdmoqamframesync: end of frame signal received\n");
            ofdmoqamframesync_reset(_q);
        }
    }

}

void ofdmoqamframesync_rxpayload(ofdmoqamframesync _q,
                                 float complex * _Y0,
                                 float complex * _Y1)
{
}

// initialize gain normalization window
//  _q      :   synchronizer object
//  _sigma  :   window standard deviation (width)
void ofdmoqamframesync_init_gain_window(ofdmoqamframesync _q,
                                        float _sigma)
{
    // validate input
    if (_sigma <= 0.0f) {
        fprintf(stderr,"ofdmoqamframesync_init_gain_window(), sigma must be greater than zero\n");
        exit(1);
    }

    unsigned int i;     // distance (index)
    float d;            // distance

    // pre-compute window
    for (i=0; i<_q->M; i++) {
        // compute distance
        d = (float)(i) / (float)(_q->M);

        // compute window element
        _q->wg[i] = expf(-(float)(d*d)/(2.0f*_sigma*_sigma));
    }
}

// estimate/interpolate gain from available samples
//  _q      :   synchronizer object
//  _G_hat  :   initial gain estimate
//  _G      :   resulting (interpolated) gain estimate
void ofdmoqamframesync_estimate_gain(ofdmoqamframesync _q,
                                     float complex * _G_hat,
                                     float complex * _G)
{
    //
    unsigned int i;
    unsigned int j;

    // estimate gain...
    float complex H_hat;
    float w;
    float w0;
    unsigned int d;
    for (i=0; i<_q->M; i++) {

        _G[i] = 0.0f;
        if (_q->p[i] == OFDMOQAMFRAME_SCTYPE_NULL)
            continue;

        // reset 
        H_hat = 0.0f;
        w0 = 0.0f;

        for (j=0; j<_q->M; j++) {
            // skip non-pilot subcarriers
            if ( _q->p[j] == OFDMOQAMFRAME_SCTYPE_NULL || (j % 2) != 0)
                continue;

            // compute distance/window
            d = abs( (int)(i) - (int)(j) );
            w = _q->wg[d];

            // accumulate gain estimate
            H_hat += w*_G_hat[j];

            // accumulate window
            w0 += w;
        }

        // eliminate divide-by-zero issues
        if (fabsf(w0) < 1e-4f) {
            fprintf(stderr,"warning: ofdmoqamframesync_estimate_gain(), weighting factor is zero; try increasing smoothing factor\n");
            _G[i] = 0.0f;
        } else if (cabsf(H_hat) < 1e-4f) {
            fprintf(stderr,"warning: ofdmoqamframesync_estimate_gain(), channel response is zero\n");
            _G[i] = 1.0f;
        } else {
            _G[i] = w0 / H_hat;
        }   
    }
}

float ofdmoqamframesync_estimate_pilot_phase(float complex _y0,
                                             float complex _y1,
                                             float complex _p)
{
    return 0.0f;
}

void ofdmoqamframesync_S0_metrics(ofdmoqamframesync _q,
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
            _q->G0[i] = _q->X0[i] / _q->S0[i];
            _q->G1[i] = _q->X1[i] / _q->S0[i];
        } else {
            _q->G0[i] = 0.0f;
            _q->G1[i] = 0.0f;
        }

        // normalize gain
        _q->G0[i] *= gain;
        _q->G1[i] *= gain;
    }   

    // compute carrier frequency offset metric
    for (i=0; i<_q->M; i++)
        g_hat += _q->G0[i] * conjf(_q->G1[i]);
    g_hat /= _q->M_S0; // normalize output

    // compute timing estimate, accumulate phase difference across
    // gains on subsequent pilot subcarriers
    // FIXME : need to assemble appropriate subcarriers
    for (i=2; i<_q->M; i+=2) {
        s_hat += _q->G0[i]*conjf(_q->G0[i-2]);
        s_hat += _q->G1[i]*conjf(_q->G1[i-2]);
    }

    // set output values
    *_g_hat = g_hat;
    *_s_hat = s_hat;
}


void ofdmoqamframesync_S1_metrics(ofdmoqamframesync _q,
                                  float complex * _t0_hat,
                                  float complex * _t1_hat)
{
    // timing, carrier offset correction
    unsigned int i;
    float complex t0_hat = 0.0f;
    float complex t1_hat = 0.0f;

    for (i=0; i<_q->M; i++) {
        t0_hat += _q->X0[i] * conjf(_q->S1[i]) * _q->G[i];
        t1_hat += _q->X1[i] * conjf(_q->S1[i]) * _q->G[i];
    }
    t0_hat /= (float)(_q->M * sqrtf(_q->M_S1));
    t1_hat /= (float)(_q->M * sqrtf(_q->M_S1));

    // set output values
    *_t0_hat = t0_hat;
    *_t1_hat = t1_hat;
}

void ofdmoqamframesync_correct_buffer(ofdmoqamframesync _q)
{
    // read buffer, re-fill filterbank
    float complex * rc;
    windowcf_read(_q->input_buffer, &rc);

    // initialize internal phase to be zero
    _q->phi = 0;

    unsigned int i;
    float complex x;
    for (i=0; i<2*(_q->M)*(_q->m); i++) {
        //printf("  %4u : phi = %12.8f\n", i, _q->phi);
        x = rc[i] * liquid_cexpjf(_q->phi);
        _q->phi += _q->dphi_prime;

        // push sample into analysis filter banks
        float complex x_delay0;
        float complex x_delay1;
        windowcf_index(_q->delay_buffer, 0,      &x_delay0); // full symbol delay
        windowcf_index(_q->delay_buffer, _q->M2, &x_delay1); // half symbol delay

        windowcf_push(_q->delay_buffer,x);
        
        firpfbch_crcf_analyzer_push(_q->ca0, x_delay0);  // push input sample
        firpfbch_crcf_analyzer_push(_q->ca1, x_delay1);  // push delayed sample
    }
}

#if DEBUG_OFDMOQAMFRAMESYNC
void ofdmoqamframesync_debug_print(ofdmoqamframesync _q)
{
    FILE * fid = fopen(DEBUG_OFDMOQAMFRAMESYNC_FILENAME,"w");
    if (!fid) {
        printf("error: ofdmoqamframe_debug_print(), could not open file for writing\n");
        return;
    }
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMOQAMFRAMESYNC_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %u;\n", DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    unsigned int i;
    float complex * rc;
    float * r;


    // short, long, training sequences
    for (i=0; i<_q->M; i++) {
        fprintf(fid,"S0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S0[i]), cimagf(_q->S0[i]));
        fprintf(fid,"S1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S1[i]), cimagf(_q->S1[i]));
    }

    fprintf(fid,"x = zeros(1,n);\n");
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(x),0:(n-1),imag(x));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    fprintf(fid,"y = zeros(1,n);\n");
    windowcf_read(_q->debug_agc_out, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(y),0:(n-1),imag(y));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    // write agc_rssi
    fprintf(fid,"\n\n");
    fprintf(fid,"agc_rssi = zeros(1,%u);\n", DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN);
    windowf_read(_q->debug_rssi, &r);
    for (i=0; i<DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(10*log10(agc_rssi))\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");

    // write short, long symbols
    fprintf(fid,"\n\n");
    fprintf(fid,"S0 = zeros(1,%u);\n", _q->M);
    fprintf(fid,"S1 = zeros(1,%u);\n", _q->M);
    for (i=0; i<_q->M; i++) {
        fprintf(fid,"S0(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->S0[i]), cimagf(_q->S0[i]));
        fprintf(fid,"S1(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->S1[i]), cimagf(_q->S1[i]));
    }


    // write gain arrays
    fprintf(fid,"\n\n");
    fprintf(fid,"G0 = zeros(1,%u);\n", _q->M);
    fprintf(fid,"G1 = zeros(1,%u);\n", _q->M);
    fprintf(fid,"G  = zeros(1,%u);\n", _q->M);
    fprintf(fid,"X0 = zeros(1,%u);\n", _q->M);
    fprintf(fid,"X1 = zeros(1,%u);\n", _q->M);
    for (i=0; i<_q->M; i++) {
        fprintf(fid,"G0(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G0[i]), cimagf(_q->G0[i]));
        fprintf(fid,"G1(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G1[i]), cimagf(_q->G1[i]));
        fprintf(fid,"G(%3u)  = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G[i]),  cimagf(_q->G[i]));
        fprintf(fid,"X0(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->X0[i]), cimagf(_q->X0[i]));
        fprintf(fid,"X1(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->X1[i]), cimagf(_q->X1[i]));
    }
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(0:(length(G)-1), fftshift(abs(G)));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('subcarrier index');\n");
    fprintf(fid,"  ylabel('gain estimate (mag)');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(0:(length(G)-1), fftshift(arg(G)));\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('subcarrier index');\n");
    fprintf(fid,"  ylabel('gain estimate (phase)');\n");

    // write frame symbols
    fprintf(fid,"framesyms = zeros(1,n);\n");
    windowcf_read(_q->debug_framesyms, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(framesyms), imag(framesyms), 'x');\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.3);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");


    fclose(fid);
    printf("ofdmoqamframesync/debug: results written to %s\n", DEBUG_OFDMOQAMFRAMESYNC_FILENAME);
}
#endif


