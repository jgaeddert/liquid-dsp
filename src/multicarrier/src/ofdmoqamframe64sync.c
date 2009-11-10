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
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_OFDMOQAMFRAME64SYNC             1
#define DEBUG_OFDMOQAMFRAME64SYNC_PRINT       1
#define DEBUG_OFDMOQAMFRAME64SYNC_FILENAME    "ofdmoqamframe64sync_internal_debug.m"
#define DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN  (2048)

// auto-correlation integration length
#define OFDMOQAMFRAME64SYNC_AUTOCORR_LEN      (64)

#if DEBUG_OFDMOQAMFRAME64SYNC
void ofdmoqamframe64sync_debug_print(ofdmoqamframe64sync _q);
#endif

struct ofdmoqamframe64sync_s {
    unsigned int num_subcarriers;   // 64
    unsigned int m;                 // filter delay
    float beta;                     // filter excess bandwidth factor

    // synchronizer parameters
    float rxx_thresh;   // auto-correlation threshold (0,1)
    float rxy_thresh;   // cross-correlation threshold (0,1)

    // filterbank objects
    firpfbch ca0;
    firpfbch ca1;
    float complex * X0, * X1;   // @ 64

    // constants
    float zeta;         // scaling factor

    // PLCP
    float complex * S0; // short sequence
    float complex * S1; // long sequence
    float complex * S1a;    // decoded long sequence (first)
    float complex * S1b;    // decoded long sequence (second)

    // pilot sequence
    msequence ms_pilot;

    // signal detection | automatic gain control
    agc sigdet;

    // auto-correlators
    autocorr_cccf autocorr0;        // auto-correlation object [0]
    autocorr_cccf autocorr1;        // auto-correlation object [1]
    unsigned int autocorr_length;   // auto-correlation length
    unsigned int autocorr_delay0;   // delay [0]
    unsigned int autocorr_delay1;   // delay [1]
    float complex rxx0;
    float complex rxx1;
    float complex rxx_max0;
    float complex rxx_max1;
    float rxx_mag_max;

    // cross-correlator
    float complex * rxy0;
    fir_filter_cccf crosscorr;

    // carrier frequency offset (CFO) estimation, compensation
    float nu_hat;
    nco nco_rx;     //numerically-controlled oscillator

    // gain
    float g;    // coarse gain estimate
    float complex * G;  // complex subcarrier gain estimate

    // receiver state
    enum {
        OFDMOQAMFRAME64SYNC_STATE_PLCPSHORT=0,  // seek PLCP short sequence
        OFDMOQAMFRAME64SYNC_STATE_PLCPLONG0,    // seek first PLCP long sequence
        OFDMOQAMFRAME64SYNC_STATE_PLCPLONG1,    // seek second PLCP long sequence
        OFDMOQAMFRAME64SYNC_STATE_RXPAYLOAD     // receive payload symbols
    } state;

    // input sample buffer
    // NOTE: this is necessary to align the channelizer
    //       buffers, although it is hardly an ideal
    //       solution
    cfwindow input_buffer;
    unsigned int timer;

    // output data buffer, ready for demodulation
    float complex * data;

#if DEBUG_OFDMOQAMFRAME64SYNC
    cfwindow debug_x;
    cfwindow debug_rxx0;
    cfwindow debug_rxx1;
    cfwindow debug_rxy;
    cfwindow debug_framesyms;
#endif
};

ofdmoqamframe64sync ofdmoqamframe64sync_create(unsigned int _m,
                                               float _beta,
                                               ofdmoqamframe64sync_callback _callback,
                                               void * _userdata)
{
    ofdmoqamframe64sync q = (ofdmoqamframe64sync) malloc(sizeof(struct ofdmoqamframe64sync_s));
    q->num_subcarriers = 64;

    // validate input
    if (_m < 2) {
        fprintf(stderr,"error: ofdmoqamframe64sync_create(), filter delay must be > 1\n");
        exit(1);
    } else if (_beta < 0.0f) {
        fprintf(stderr,"error: ofdmoqamframe64sync_create(), filter excess bandwidth must be > 0\n");
        exit(1);
    }
    q->m = _m;
    q->beta = _beta;

    // synchronizer parameters
    q->rxx_thresh = 0.75f;  // auto-correlation threshold
    q->rxy_thresh = 0.75f;  // cross-correlation threshold

    q->zeta = 64.0f/sqrtf(52.0f);   // scaling factor
    
    // create analysis filter banks
    q->ca0 = firpfbch_create(q->num_subcarriers, q->m, q->beta, 0.0f /*dt*/,FIRPFBCH_ROOTNYQUIST,0/*gradient*/);
    q->ca1 = firpfbch_create(q->num_subcarriers, q->m, q->beta, 0.0f /*dt*/,FIRPFBCH_ROOTNYQUIST,0/*gradient*/);
    q->X0 = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->X1 = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
 
    // allocate memory for PLCP arrays
    q->S0 = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->S1 = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    ofdmoqamframe64_init_S0(q->S0);
    ofdmoqamframe64_init_S1(q->S1);
    unsigned int i;
    for (i=0; i<q->num_subcarriers; i++) {
        q->S0[i] *= q->zeta;
        q->S1[i] *= q->zeta;
    }
    q->S1a = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->S1b = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

    // set pilot sequence
    q->ms_pilot = msequence_create(8);

    // create agc | signal detection object
    q->sigdet = agc_create(1.0f, 0.01f);

    // create NCO for CFO compensation
    q->nco_rx = nco_create(LIQUID_VCO);

    // create auto-correlator objects
    q->autocorr_length = OFDMOQAMFRAME64SYNC_AUTOCORR_LEN;
    q->autocorr_delay0 = q->num_subcarriers;
    q->autocorr_delay1 = q->num_subcarriers / 2;
    q->autocorr0 = autocorr_cccf_create(q->autocorr_length, q->autocorr_delay0);
    q->autocorr1 = autocorr_cccf_create(q->autocorr_length, q->autocorr_delay1);

    // create cross-correlator object
    q->rxy0 = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    ofdmoqam cs = ofdmoqam_create(q->num_subcarriers,q->m,q->beta,
                                  0.0f,   // dt
                                  OFDMOQAM_SYNTHESIZER,
                                  0);     // gradient
    for (i=0; i<2*(q->m); i++)
        ofdmoqam_execute(cs,q->S1,q->rxy0);
    q->crosscorr = fir_filter_cccf_create(q->rxy0, q->num_subcarriers);
    ofdmoqam_destroy(cs);

    // input buffer: 3 S0 symbols + 2 S1 symbols
    q->input_buffer = cfwindow_create(5*(q->num_subcarriers));

    // gain
    q->g = 1.0f;
    q->G = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

    q->data = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

    // reset object
    ofdmoqamframe64sync_reset(q);

#if DEBUG_OFDMOQAMFRAME64SYNC
    q->debug_x =        cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxx0=      cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxx1=      cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxy=       cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    q->debug_framesyms= cfwindow_create(DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
#endif

    return q;
}

void ofdmoqamframe64sync_destroy(ofdmoqamframe64sync _q)
{
#if DEBUG_OFDMOQAMFRAME64SYNC
    ofdmoqamframe64sync_debug_print(_q);
    cfwindow_destroy(_q->debug_x);
    cfwindow_destroy(_q->debug_rxx0);
    cfwindow_destroy(_q->debug_rxx1);
    cfwindow_destroy(_q->debug_rxy);
    cfwindow_destroy(_q->debug_framesyms);
#endif

    // free analysis filter bank memory objects
    firpfbch_destroy(_q->ca0);
    firpfbch_destroy(_q->ca1);
    free(_q->X0);
    free(_q->X1);

    // clean up PLCP arrays
    free(_q->S0);
    free(_q->S1);
    free(_q->S1a);
    free(_q->S1b);

    // free pilot msequence object memory
    msequence_destroy(_q->ms_pilot);

    // free agc | signal detection object memory
    agc_destroy(_q->sigdet);

    // free NCO object memory
    nco_destroy(_q->nco_rx);

    // free auto-correlator memory objects
    autocorr_cccf_destroy(_q->autocorr0);
    autocorr_cccf_destroy(_q->autocorr1);

    // free cross-correlator memory objects
    fir_filter_cccf_destroy(_q->crosscorr);
    free(_q->rxy0);

    // free gain arrays
    free(_q->G);

    // free data buffer
    free(_q->data);

    // free circular buffer array
    cfwindow_destroy(_q->input_buffer);

    // free main object memory
    free(_q);
}

void ofdmoqamframe64sync_print(ofdmoqamframe64sync _q)
{
    printf("ofdmoqamframe64sync:\n");
    printf("    num subcarriers     :   %-u\n", _q->num_subcarriers);
    printf("    m (filter delay)    :   %-u\n", _q->m);
    printf("    beta (excess b/w)   :   %8.6f\n", _q->beta);
}

void ofdmoqamframe64sync_reset(ofdmoqamframe64sync _q)
{
    // reset pilot sequence generator
    msequence_reset(_q->ms_pilot);

    // reset auto-correlators
    autocorr_cccf_clear(_q->autocorr0);
    autocorr_cccf_clear(_q->autocorr1);
    _q->rxx_max0 = 0.0f;
    _q->rxx_max1 = 0.0f;
    _q->rxx_mag_max = 0.0f;

    // reset frequency offset estimation, correction
    _q->nu_hat = 0.0f;
    nco_reset(_q->nco_rx);

    // clear input buffer
    cfwindow_clear(_q->input_buffer);

    // clear analysis filter bank objects
    firpfbch_clear(_q->ca0);
    firpfbch_clear(_q->ca1);

    // reset gain parameters
    _q->g = 1.0f;   // coarse gain estimate
    unsigned int i;
    for (i=0; i<_q->num_subcarriers; i++)
        _q->G[i] = 1.0f;

    // reset state
    _q->state = OFDMOQAMFRAME64SYNC_STATE_PLCPSHORT;
    _q->timer = 0;
}

void ofdmoqamframe64sync_execute(ofdmoqamframe64sync _q,
                                 float complex * _x,
                                 unsigned int _n)
{
    unsigned int i;
    float complex x;
    for (i=0; i<_n; i++) {
        x = _x[i];
#if DEBUG_OFDMOQAMFRAME64SYNC
        cfwindow_push(_q->debug_x,x);
#endif

        // coarse gain correction
        //x *= _q->g;
        
        // compensate for CFO
        //nco_mix_up(_q->nco_rx, x, &x);

        // push sample into analysis filter banks
        firpfbch_analyzer_push(_q->ca0, x);
        firpfbch_analyzer_push(_q->ca1, x); // TODO : push delayed sample

        switch (_q->state) {
        case OFDMOQAMFRAME64SYNC_STATE_PLCPSHORT:
            ofdmoqamframe64sync_execute_plcpshort(_q,x);
            break;
        case OFDMOQAMFRAME64SYNC_STATE_PLCPLONG0:
            ofdmoqamframe64sync_execute_plcplong0(_q,x);
            break;
        case OFDMOQAMFRAME64SYNC_STATE_PLCPLONG1:
            ofdmoqamframe64sync_execute_plcplong1(_q,x);
            break;
        case OFDMOQAMFRAME64SYNC_STATE_RXPAYLOAD:
            ofdmoqamframe64sync_execute_rxpayload(_q,x);
            break;
        default:;
        }
    }
}

//
// internal
//

#if DEBUG_OFDMOQAMFRAME64SYNC
void ofdmoqamframe64sync_debug_print(ofdmoqamframe64sync _q)
{
    FILE * fid = fopen(DEBUG_OFDMOQAMFRAME64SYNC_FILENAME,"w");
    if (!fid) {
        printf("error: ofdmoqamframe64_debug_print(), could not open file for writing\n");
        return;
    }
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMOQAMFRAME64SYNC_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %u;\n", DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN);
    unsigned int i;
    float complex * rc;

    // gain vectors
    fprintf(fid,"g = %12.4e;\n", _q->g);
    for (i=0; i<_q->num_subcarriers; i++) {
        fprintf(fid,"G(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G[i]), cimagf(_q->G[i]));
        //fprintf(fid,"G0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G0[i]), cimagf(_q->G0[i]));
        //fprintf(fid,"G1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G1[i]), cimagf(_q->G1[i]));
    }
 
    // CFO estimate
    fprintf(fid,"nu_hat = %12.4e;\n", _q->nu_hat);

    // short, long sequences
    for (i=0; i<_q->num_subcarriers; i++) {
        fprintf(fid,"S0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S0[i]), cimagf(_q->S0[i]));
        fprintf(fid,"S1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S1[i]), cimagf(_q->S1[i]));
    }

    fprintf(fid,"x = zeros(1,n);\n");
    cfwindow_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(x),0:(n-1),imag(x));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    fprintf(fid,"rxx0 = zeros(1,n);\n");
    cfwindow_read(_q->debug_rxx0, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxx0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"rxx1 = zeros(1,n);\n");
    cfwindow_read(_q->debug_rxx1, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxx1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxx0),0:(n-1),abs(rxx1),0:(n-1),[abs(rxx0)+abs(rxx1)]/2,'-k','LineWidth',2);\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xx}|');\n");

    fprintf(fid,"rxy = zeros(1,n);\n");
    cfwindow_read(_q->debug_rxy, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxy));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    // decoded long sequences
    fprintf(fid,"S1  = zeros(1,64);\n");
    fprintf(fid,"S1a = zeros(1,64);\n");
    fprintf(fid,"S1b = zeros(1,64);\n");
    for (i=0; i<64; i++) {
        fprintf(fid,"S1(%4u)  = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S1[i]),  cimagf(_q->S1[i]));
        fprintf(fid,"S1a(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S1a[i]), cimagf(_q->S1a[i]));
        fprintf(fid,"S1b(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->S1b[i]), cimagf(_q->S1b[i]));
    }
    fprintf(fid,"S1 = S1/(64/sqrt(52));\n");
    fprintf(fid,"figure;\n");
    //fprintf(fid,"f = [(0:63)]/64 - 0.5;\n");
    //fprintf(fid,"plot(S1,'x',S1a,'x',S1b,'x');\n");
    fprintf(fid,"plot(S1,'x',S1a,'x');\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.3);\n");
    fprintf(fid,"title('PLCP long sequences');\n");

    fprintf(fid,"t = 5*64;\n");
    fprintf(fid,"input_buffer = zeros(1,t);\n");
    cfwindow_read(_q->input_buffer, &rc);
    for (i=0; i<5*64; i++)
        fprintf(fid,"input_buffer(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(t-1),real(input_buffer),0:(t-1),imag(input_buffer));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('input buffer');\n");


    /*
    // plot gain vectors
    fprintf(fid,"f = [-32:31];\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(f,fftshift(abs(G0)),f,fftshift(abs(G1)),f,fftshift(abs(G)));\n");
    fprintf(fid,"    ylabel('gain');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(f,unwrap(fftshift(arg(G0))),...\n");
    fprintf(fid,"         f,unwrap(fftshift(arg(G1))),...\n");
    fprintf(fid,"         f,unwrap(fftshift(arg(G))));\n");
    fprintf(fid,"    ylabel('phase');\n");
    */

    // frame symbols
    fprintf(fid,"framesyms = zeros(1,n);\n");
    cfwindow_read(_q->debug_framesyms, &rc);
    for (i=0; i<DEBUG_OFDMOQAMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(framesyms),imag(framesyms),'x','MarkerSize',1);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title('Frame Symbols');\n");

    fclose(fid);
    printf("ofdmoqamframe64sync/debug: results written to %s\n", DEBUG_OFDMOQAMFRAME64SYNC_FILENAME);
}
#endif

void ofdmoqamframe64sync_execute_plcpshort(ofdmoqamframe64sync _q, float complex _x)
{
    cfwindow_push(_q->input_buffer,_x);

    // run AGC, clip output
    float complex y;
    agc_execute(_q->sigdet, _x, &y);
    //if (agc_get_signal_level(_q->sigdet) < -15.0f)
    //    return;


    // auto-correlators
    autocorr_cccf_push(_q->autocorr0, _x);
    autocorr_cccf_execute(_q->autocorr0, &_q->rxx0);

    autocorr_cccf_push(_q->autocorr1, _x);
    autocorr_cccf_execute(_q->autocorr1, &_q->rxx1);

#if DEBUG_OFDMOQAMFRAME64SYNC
    cfwindow_push(_q->debug_rxx0, _q->rxx0);
    cfwindow_push(_q->debug_rxx1, _q->rxx1);
#endif
    float rxx_mag = cabsf(_q->rxx0) + cabsf(_q->rxx1);
    rxx_mag *= 0.5f;
    rxx_mag *= agc_get_signal_level(_q->sigdet);
    if (rxx_mag > _q->rxx_mag_max) {
        _q->rxx_max0 = _q->rxx0;
        _q->rxx_max1 = _q->rxx1;
        _q->rxx_mag_max = rxx_mag;
    }

    if (rxx_mag > (_q->rxx_thresh)*(_q->autocorr_length)) {
        // TODO : wait for auto-correlation to peak before changing state

        // estimate CFO
        _q->nu_hat = cargf((_q->rxx_max0)*conjf(_q->rxx_max1));
        if (_q->nu_hat >  M_PI/2.0f) _q->nu_hat -= M_PI;
        if (_q->nu_hat < -M_PI/2.0f) _q->nu_hat += M_PI;
        _q->nu_hat *= 2.0f / (float)(_q->num_subcarriers);

#if DEBUG_OFDMOQAMFRAME64SYNC_PRINT
        printf("rxx[0] = |%12.8f| arg{%12.8f}\n", cabsf(_q->rxx_max0),cargf(_q->rxx_max0));
        printf("rxx[1] = |%12.8f| arg{%12.8f}\n", cabsf(_q->rxx_max1),cargf(_q->rxx_max1));
        printf("nu_hat =  %12.8f\n", _q->nu_hat);
#endif

        //nco_set_frequency(_q->nco_rx, _q->nu_hat);
        _q->state = OFDMOQAMFRAME64SYNC_STATE_PLCPLONG0;

        _q->g = agc_get_gain(_q->sigdet);
    }
}

void ofdmoqamframe64sync_execute_plcplong0(ofdmoqamframe64sync _q, float complex _x)
{
    cfwindow_push(_q->input_buffer,_x);

    // cross-correlator
    float complex rxy;
    fir_filter_cccf_push(_q->crosscorr, _x);
    fir_filter_cccf_execute(_q->crosscorr, &rxy);

#if DEBUG_OFDMOQAMFRAME64SYNC
    cfwindow_push(_q->debug_rxy, rxy);
#endif

    if (cabsf(rxy) > (_q->rxy_thresh)*(_q->num_subcarriers)) {
        printf("rxy[0] : %12.8f\n", cabsf(rxy));

        // run analyzers
        firpfbch_analyzer_run(_q->ca0, _q->X0);
        firpfbch_analyzer_run(_q->ca1, _q->X1);
        unsigned int i;
        for (i=0; i<_q->num_subcarriers; i++) {
            if ((i%2)==0) {
                _q->S1a[i] = crealf(_q->X0[i]) + cimagf(_q->X1[i])*_Complex_I;
            } else {
                _q->S1a[i] = crealf(_q->X1[i]) + cimagf(_q->X0[i])*_Complex_I;
            }
            _q->S1a[i] /= _q->zeta;
        }
        
        //_q->state = OFDMOQAMFRAME64SYNC_STATE_PLCPLONG1;
    }
}

void ofdmoqamframe64sync_execute_plcplong1(ofdmoqamframe64sync _q, float complex _x)
{
    cfwindow_push(_q->input_buffer,_x);

    // cross-correlator
    float complex rxy;
    fir_filter_cccf_push(_q->crosscorr, _x);
    fir_filter_cccf_execute(_q->crosscorr, &rxy);

#if DEBUG_OFDMOQAMFRAME64SYNC
    cfwindow_push(_q->debug_rxy, rxy);
#endif

    if (cabsf(rxy) > (_q->rxy_thresh)*(_q->num_subcarriers)) {
        printf("rxy[1] : %12.8f\n", cabsf(rxy));
        //_q->state = OFDMOQAMFRAME64SYNC_STATE_RXPAYLOAD;
    }

}

void ofdmoqamframe64sync_execute_rxpayload(ofdmoqamframe64sync _q, float complex _x)
{
    // push 64 samples into buffer
    _q->timer++;
    if (_q->timer < _q->num_subcarriers)
        return;

    // reset timer
    _q->timer = 0;

    // execute analysis filter banks
    //printf("rxpayload returning prematurely\n");
    //return;
    firpfbch_analyzer_run(_q->ca0, _q->X0);
    firpfbch_analyzer_run(_q->ca1, _q->X1);

    // gain correction (equalizer)
    unsigned int i;
    for (i=0; i<_q->num_subcarriers; i++) {
        _q->X0[i] *= _q->G[i] * _q->zeta;
        _q->X1[i] *= _q->G[i] * _q->zeta;
    }

    // TODO : extract pilots

    // TODO : compute pilot phase correction (fit to first-order polynomial)

    // strip data subcarriers
    unsigned int j=0;
    int sctype;
    for (i=0; i<_q->num_subcarriers; i++) {
        sctype = ofdmoqamframe64_getsctype(i);
        if (sctype==OFDMOQAMFRAME64_SCTYPE_NULL) {
            // disabled subcarrier
        } else if (sctype==OFDMOQAMFRAME64_SCTYPE_PILOT) {
            // pilot subcarrier : use p/n sequence for pilot phase
        } else {
            // data subcarrier
            if ((i%2)==0) {
                // even subcarrier
                _q->data[j] = crealf(_q->X0[i]) + 
                              cimagf(_q->X1[i]) * _Complex_I;
            } else {
                // odd subcarrier
                _q->data[j] = cimagf(_q->X0[i]) * _Complex_I +
                              crealf(_q->X1[i]);
            }
            // scaling factor
            _q->data[j] /= _q->zeta;
            j++;
        }
    }
    assert(j==48);

//#if DEBUG_OFDMFRAME64SYNC
    for (i=0; i<48; i++)
        cfwindow_push(_q->debug_framesyms,_q->data[i]);
//#endif

}
