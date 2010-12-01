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
#define DEBUG_OFDMFRAMESYNC_PRINT       1
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

    // 
    float complex * S0;     // short sequence
    float complex * S1;     // long sequence

    // gain
    float g;                // coarse gain estimate
    float complex * G0;     // complex subcarrier gain estimate, S2[0]
    float complex * G1;     // complex subcarrier gain estimate, S2[1]
    float complex * G;      // complex subcarrier gain estimate
    float complex * Y;      // output symbols

    // receiver state
    enum {
        OFDMFRAMESYNC_STATE_PLCPSHORT=0,  // seek PLCP short sequence
        OFDMFRAMESYNC_STATE_PLCPLONG0,    // seek first PLCP long sequence
        OFDMFRAMESYNC_STATE_PLCPLONG1,    // seek second PLCP long sequence
        OFDMFRAMESYNC_STATE_RXSYMBOLS     // receive payload symbols
    } state;

    // synchronizer objects
    nco_crcf nco_rx;        // numerically-controlled oscillator
    agc_crcf agc_rx;        // automatic gain control
    autocorr_cccf autocorr; // auto-correlator

    // callback
    ofdmframesync_callback callback;
    void * userdata;

#if DEBUG_OFDMFRAMESYNC
    windowcf debug_x;
    windowcf debug_agc_out;
    windowcf debug_rxx;
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
    q->fft = FFT_CREATE_PLAN(q->M, q->X, q->x, FFT_DIR_FORWARD, FFT_METHOD);
 
    // create input buffer the length of the transform
    q->input_buffer = windowcf_create(q->M);

    // allocate memory for PLCP arrays
    q->S0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->S1 = (float complex*) malloc((q->M)*sizeof(float complex));
    ofdmframe_init_S0(q->p, q->M, q->S0, &q->M_S0);
    ofdmframe_init_S1(q->p, q->M, q->S1, &q->M_S1);

    // compute scaling factor
    q->g_data = sqrtf(q->M) / sqrtf(q->M_pilot + q->M_data);
    q->g_S0   = sqrtf(q->M) / sqrtf(q->M_S0);
    q->g_S1   = sqrtf(q->M) / sqrtf(q->M_S1);

    // gain
    q->g = 1.0f;
    q->G0 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->G1 = (float complex*) malloc((q->M)*sizeof(float complex));
    q->G  = (float complex*) malloc((q->M)*sizeof(float complex));
    q->Y  = (float complex*) malloc((q->M)*sizeof(float complex));

    // set callback data
    q->callback = _callback;
    q->userdata = _userdata;

    // 
    // synchronizer objects
    //

    // numerically-controlled oscillator
    q->nco_rx = nco_crcf_create(LIQUID_NCO);

    // agc, rssi, squelch
    q->agc_rx = agc_crcf_create();
    agc_crcf_set_target(q->agc_rx, 1.0f);
    agc_crcf_set_bandwidth(q->agc_rx,  1e-2f);
    agc_crcf_set_gain_limits(q->agc_rx, 1e-3f, 1e4f);

    agc_crcf_squelch_activate(q->agc_rx);
    agc_crcf_squelch_set_threshold(q->agc_rx, -35.0f);
    agc_crcf_squelch_set_timeout(q->agc_rx, 32);

    agc_crcf_squelch_enable_auto(q->agc_rx);

    // auto-correlator
    q->autocorr = autocorr_cccf_create(q->M, q->M / 2);

    // reset object
    ofdmframesync_reset(q);

#if DEBUG_OFDMFRAMESYNC
    q->debug_x =        windowcf_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    q->debug_agc_out =  windowcf_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
    q->debug_rxx =      windowcf_create(DEBUG_OFDMFRAMESYNC_BUFFER_LEN);
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

    windowcf_destroy(_q->debug_x);
    windowcf_destroy(_q->debug_agc_out);
    windowcf_destroy(_q->debug_rxx);
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
    free(_q->S1);

    // free gain arrays
    free(_q->G0);
    free(_q->G1);
    free(_q->G);
    free(_q->Y);

    // destroy synchronizer objects
    nco_crcf_destroy(_q->nco_rx);           // numerically-controlled oscillator
    agc_crcf_destroy(_q->agc_rx);           // automatic gain control
    autocorr_cccf_destroy(_q->autocorr);    // auto-correlator

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
    // reset gain parameters
    unsigned int i;
    for (i=0; i<_q->M; i++)
        _q->G[i] = 1.0f;

    // reset synchronizer objects
    agc_crcf_unlock(_q->agc_rx);    // automatic gain control (unlock)

    // reset state
    _q->state = OFDMFRAMESYNC_STATE_PLCPSHORT;
}

void ofdmframesync_execute(ofdmframesync _q,
                           float complex * _x,
                           unsigned int _n)
{
    unsigned int i;
    float complex x;
    float complex y;
    int squelch_status;
    for (i=0; i<_n; i++) {
        x = _x[i];

        // save input sample to buffer
        windowcf_push(_q->input_buffer,x);

        // apply agc (estimate initial signal gain)
        agc_crcf_execute(_q->agc_rx, x, &y);

#if DEBUG_OFDMFRAMESYNC
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
        
        switch (_q->state) {
        case OFDMFRAMESYNC_STATE_PLCPSHORT:
            ofdmframesync_execute_plcpshort(_q,x);
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
// internal
//

// frame detection
void ofdmframesync_execute_plcpshort(ofdmframesync _q,
                                     float complex _x)
{
    float complex rxx;
    autocorr_cccf_push(_q->autocorr, _x);
    autocorr_cccf_execute(_q->autocorr, &rxx);
    //printf("  rxx : %12.8f {%12.8f}\n", cabsf(rxx), cargf(rxx));

    float g0 = agc_crcf_get_gain(_q->agc_rx);
    float g1 = 1.0f / sqrtf(_q->M); // NOTE : this gain will change based on auto-correlator length
    rxx *= g0*g1;

#if DEBUG_OFDMFRAMESYNC
    windowcf_push(_q->debug_rxx, rxx);
#endif

    // check to see if signal exceeds threshold
    //if ( cabsf(rxx) > 0.7f ) printf("frame detected\n");
}

void ofdmframesync_execute_plcplong0(ofdmframesync _q,
                                     float complex _x)
{
}

void ofdmframesync_execute_plcplong1(ofdmframesync _q,
                                     float complex _x)
{
}

void ofdmframesync_execute_rxsymbols(ofdmframesync _q,
                                     float complex _x)
{
}

void ofdmframesync_rxpayload(ofdmframesync _q)
{
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
    fprintf(fid,"   ylabel('auto-correlation, arg{rxx}');\n");


    fprintf(fid,"y = zeros(1,n);\n");
    windowcf_read(_q->debug_agc_out, &rc);
    for (i=0; i<DEBUG_OFDMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(y),0:(n-1),imag(y));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");

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
    for (i=0; i<_q->M; i++) {
        fprintf(fid,"G0(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G0[i]), cimagf(_q->G0[i]));
        fprintf(fid,"G1(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G1[i]), cimagf(_q->G1[i]));
        fprintf(fid,"G(%3u)  = %12.8f + j*%12.8f;\n", i+1, crealf(_q->G[i]),  cimagf(_q->G[i]));
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
    for (i=0; i<DEBUG_OFDMFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(framesyms), imag(framesyms), 'x');\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.3);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("ofdmframesync/debug: results written to %s\n", DEBUG_OFDMFRAMESYNC_FILENAME);
}
#endif


