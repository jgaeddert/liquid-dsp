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

#include "liquid.experimental.h"
#include "liquid.internal.h"

#define DEBUG_OFDMFRAME64SYNC             1
#define DEBUG_OFDMFRAME64SYNC_PRINT       0
#define DEBUG_OFDMFRAME64SYNC_FILENAME    "ofdmframe64sync_internal_debug.m"
#define DEBUG_OFDMFRAME64SYNC_BUFFER_LEN  (1024)

// auto-correlation integration length
#define OFDMFRAME64SYNC_AUTOCORR_LEN      (96)

struct ofdmframe64sync_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    // fast Fourier transform
    float complex * x;      // FFT time-domain buffer
    float complex * X;      // FFT freq-domain buffer
    FFT_PLAN fft;

    // initial gain correction / signal detection
    agc_crcf sigdet;        // automatic gain control for signal detection
    float g;                // flat gain estimation

    // equalization
    msequence ms_pilot;     // P/N sequence pilot phase generator
    float complex G[64];    // complex channel gain correction
    float x_phase[4];       // pilot subcarrier index
    float y_phase[4];       // pilot subcarrier phase
    float p_phase[2];       // polynomial fit

    nco_crcf nco_pilot;     // pilot phase nco

    float phi_prime;        // previous average pilot phase
    float dphi;             // differential pilot phase
    float m_prime;          // previous phase slope
    float m_hat;            // filtered phase slope
    unsigned int num_symbols_rx;

    // numerically-controlled oscillator for carrier offset correction
    nco_crcf nco_rx;

    // PLCP SHORT : delay correlator
    autocorr_cccf delay_correlator;
    float complex rxx_max;

    // PLCP LONG: cross correlator
    dotprod_cccf cross_correlator;
    float complex rxy;
    float complex rxy0;
    float complex rxy1;
    windowcf rxy_buffer;
    windowcf Lt_buffer;
    float complex Lt0[64], Lf0[64]; // received PLCP long sequence (first)
    float complex Lt1[64], Lf1[64]; // received PLCP long sequence (second)
    float complex G0[64], G1[64];
    int backoff;
    float complex B[64];            // subcarrier phase rotation due to backoff

    // timer
    unsigned int timer;
    float complex symbol[80];   // symbol data buffer
    float complex data[48];     // synchronized data subcarriers

    float nu_hat0;      // carrier frequency offset estimation (coarse)
    float nu_hat1;      // carrier frequency offset estimation (fine)

    // callback function and scaling factor
    ofdmframe64sync_callback callback;
    void * userdata;

    // receiver state
    enum {
        OFDMFRAME64SYNC_STATE_PLCPSHORT=0,  // seek PLCP short sequence
        OFDMFRAME64SYNC_STATE_PLCPLONG0,    // seek first PLCP long seq.
        OFDMFRAME64SYNC_STATE_PLCPLONG1,    // seek second PLCP long seq.
        OFDMFRAME64SYNC_STATE_RXPAYLOAD     // receive payload symbols
    } state;

#if DEBUG_OFDMFRAME64SYNC
    windowcf debug_x;
    windowcf debug_rxx;
    windowcf debug_rxy;
    windowcf debug_framesyms;
    windowf debug_pilotphase;
    windowf debug_pilotphase_hat;
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

    q->fft = FFT_CREATE_PLAN(q->num_subcarriers, q->x, q->X, FFT_DIR_FORWARD, FFT_METHOD);

    // initial gain correction / signal detection
    q->sigdet = agc_crcf_create();
    agc_crcf_set_target(q->sigdet,1.0f);
    agc_crcf_set_bandwidth(q->sigdet,0.1f);

    // carrier offset correction
    q->nco_rx = nco_crcf_create(LIQUID_VCO);
    q->nco_pilot = nco_crcf_create(LIQUID_VCO);
    nco_crcf_pll_set_bandwidth(q->nco_pilot,0.01f);

    // cyclic prefix correlation windows
    q->delay_correlator = autocorr_cccf_create(OFDMFRAME64SYNC_AUTOCORR_LEN,16);

    // cross-correlator
    unsigned int i;
    float complex h[64];
    for (i=0; i<64; i++)
        h[i] = conjf(ofdmframe64_plcp_Lt[i]);
    q->cross_correlator = dotprod_cccf_create(h,64);
    q->rxy_buffer = windowcf_create(64);
    q->Lt_buffer = windowcf_create(160);
    
#if DEBUG_OFDMFRAME64SYNC
    q->debug_x   = windowcf_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxx = windowcf_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxy = windowcf_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    q->debug_framesyms = windowcf_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    q->debug_pilotphase = windowf_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    q->debug_pilotphase_hat = windowf_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
#endif

    // pilot sequence generator
    q->ms_pilot = msequence_create_default(8);
    q->x_phase[0] = -21.0f;
    q->x_phase[1] =  -7.0f;
    q->x_phase[2] =   7.0f;
    q->x_phase[3] =  21.0f;

    q->backoff = 2;
    float phi = (float)(q->backoff)*2.0f*M_PI/64.0f;
    for (i=0; i<64; i++)
        q->B[i] = liquid_cexpjf(i*phi);

    q->callback = _callback;
    q->userdata = _userdata;

    ofdmframe64sync_reset(q);

    return q;
}

void ofdmframe64sync_destroy(ofdmframe64sync _q)
{
#if DEBUG_OFDMFRAME64SYNC
    ofdmframe64sync_debug_print(_q);
    windowcf_destroy(_q->debug_x);
    windowcf_destroy(_q->debug_rxx);
    windowcf_destroy(_q->debug_rxy);
    windowcf_destroy(_q->debug_framesyms);
    windowf_destroy(_q->debug_pilotphase);
    windowf_destroy(_q->debug_pilotphase_hat);
#endif

    free(_q->x);
    free(_q->X);
    windowcf_destroy(_q->rxy_buffer);
    windowcf_destroy(_q->Lt_buffer);
    FFT_DESTROY_PLAN(_q->fft);

    agc_crcf_destroy(_q->sigdet);
    msequence_destroy(_q->ms_pilot);
    autocorr_cccf_destroy(_q->delay_correlator);
    dotprod_cccf_destroy(_q->cross_correlator);
    nco_crcf_destroy(_q->nco_rx);
    nco_crcf_destroy(_q->nco_pilot);
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

    _q->g = 1.0f;
    agc_crcf_reset(_q->sigdet);
    _q->state = OFDMFRAME64SYNC_STATE_PLCPSHORT;
    autocorr_cccf_clear(_q->delay_correlator);
    _q->rxx_max = 0.0f;
    nco_crcf_set_frequency(_q->nco_rx, 0.0f);
    nco_crcf_set_phase(_q->nco_rx, 0.0f);
    nco_crcf_reset(_q->nco_pilot);

    // reset symbol timer
    _q->timer = 0;

    _q->num_symbols_rx = 0;
    _q->phi_prime = 0.0f;
    _q->dphi = 0.0f;
    _q->m_hat = 0.0f;
    _q->m_prime = 0.0f;

#if 0
    unsigned int i;
    for (i=0; i<64; i++) {
        // clear PLCP long buffers
        _q->Lt0[i] = 0.0f;
        _q->Lt1[i] = 0.0f;

        // reset gain
        _q->G[i] = 1.0f;
    }
#endif
}

void ofdmframe64sync_execute(ofdmframe64sync _q,
                             float complex * _x,
                             unsigned int _n)
{
    unsigned int i;
    float complex x;
    for (i=0; i<_n; i++) {
        x = _x[i];
#if DEBUG_OFDMFRAME64SYNC
        windowcf_push(_q->debug_x,x);
#endif

        // coarse gain correction
        x *= _q->g;
        
        // carrier frequency offset estimation
        nco_crcf_mix_up(_q->nco_rx, x, &x);

        switch (_q->state) {
        case OFDMFRAME64SYNC_STATE_PLCPSHORT:
            ofdmframe64sync_execute_plcpshort(_q,x);
            break;
        case OFDMFRAME64SYNC_STATE_PLCPLONG0:
            ofdmframe64sync_execute_plcplong0(_q,x);
            break;
        case OFDMFRAME64SYNC_STATE_PLCPLONG1:
            ofdmframe64sync_execute_plcplong1(_q,x);
            break;
        case OFDMFRAME64SYNC_STATE_RXPAYLOAD:
            ofdmframe64sync_execute_rxpayload(_q,x);
            break;
        default:;
        }
    }
}

//
// internal
//

void ofdmframe64sync_debug_print(ofdmframe64sync _q)
{
#if DEBUG_OFDMFRAME64SYNC
    FILE * fid = fopen(DEBUG_OFDMFRAME64SYNC_FILENAME,"w");
    if (!fid) {
        fprintf(stderr,"error: ofdmframe64_debug_print(), could not open file for writing\n");
        exit(1);
    }
    fprintf(fid,"%% %s : auto-generated file\n", DEBUG_OFDMFRAME64SYNC_FILENAME);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"n = %u;\n", DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    unsigned int i;
    float complex * rc;

    fprintf(fid,"nu_hat = %12.4e;\n", _q->nu_hat0 + _q->nu_hat1);

    // gain vectors
    for (i=0; i<64; i++) {
        fprintf(fid,"G(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G[i]), cimagf(_q->G[i]));
        fprintf(fid,"G0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G0[i]), cimagf(_q->G0[i]));
        fprintf(fid,"G1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->G1[i]), cimagf(_q->G1[i]));
    }
 
    fprintf(fid,"x = zeros(1,n);\n");
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(x),0:(n-1),imag(x));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    fprintf(fid,"rxx = zeros(1,n);\n");
    windowcf_read(_q->debug_rxx, &rc);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxx));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xx}|');\n");

    fprintf(fid,"rxy = zeros(1,n);\n");
    windowcf_read(_q->debug_rxy, &rc);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxy));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    fprintf(fid,"s = [2:27 39:64];\n");
    fprintf(fid,"Lt0 = zeros(1,64);\n");
    fprintf(fid,"Lt1 = zeros(1,64);\n");
    for (i=0; i<64; i++) {
        fprintf(fid,"Lt0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->Lt0[i]), cimagf(_q->Lt0[i]));
        fprintf(fid,"Lf0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->Lf0[i]), cimagf(_q->Lf0[i]));
        fprintf(fid,"Lt1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->Lt1[i]), cimagf(_q->Lt1[i]));
        fprintf(fid,"Lf1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(_q->Lf1[i]), cimagf(_q->Lf1[i]));
    }
    //fprintf(fid,"Lf0 = fft(Lt0).*G;\n");
    //fprintf(fid,"Lf1 = fft(Lt1).*G;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(Lf0(s)),imag(Lf0(s)),'x','MarkerSize',1,...\n");
    fprintf(fid,"     real(Lf1(s)),imag(Lf1(s)),'x','MarkerSize',1);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title('PLCP Long Sequence (after gain correction)');\n");

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

    // frame symbols
    fprintf(fid,"framesyms = zeros(1,n);\n");
    windowcf_read(_q->debug_framesyms, &rc);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(framesyms),imag(framesyms),'x','MarkerSize',1);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title('Frame Symbols');\n");

    // pilot phase
    float * r;
    fprintf(fid,"pilotphase = zeros(1,n);\n");
    windowf_read(_q->debug_pilotphase, &r);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"pilotphase(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"pilotphase_hat = zeros(1,n);\n");
    windowf_read(_q->debug_pilotphase_hat, &r);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"pilotphase_hat(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"figure; plot(0:127,pilotphase([n-128+1]:n),'-k','LineWidth',1,0:127,pilotphase_hat([n-128+1]:n),'-k','LineWidth',2);\n");

    fclose(fid);
    printf("ofdmframe64sync/debug: results written to %s\n", DEBUG_OFDMFRAME64SYNC_FILENAME);
#endif
}

void ofdmframe64sync_execute_plcpshort(ofdmframe64sync _q,
                                       float complex _x)
{
    // run AGC, clip output
    float complex y;
    agc_crcf_execute(_q->sigdet, _x, &y);
    //if (agc_crcf_get_signal_level(_q->sigdet) < -15.0f)
    //    return;
    if (cabsf(y) > 2.0f)
        y = 2.0f*liquid_cexpjf(cargf(y));

    // run auto-correlator
    float complex rxx;
    autocorr_cccf_push(_q->delay_correlator, y);
    autocorr_cccf_execute(_q->delay_correlator, &rxx);

#if DEBUG_OFDMFRAME64SYNC
    windowcf_push(_q->debug_rxx,rxx);
#endif

    if (cabsf(rxx) > 0.75f*OFDMFRAME64SYNC_AUTOCORR_LEN) {
        // TODO : wait for auto-correlation to peak before changing state

#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("rxx = %12.8f (angle : %12.8f);\n", cabsf(rxx),cargf(rxx)/16.0f);
#endif
        _q->nu_hat0 = -cargf(rxx)/16.0f;
        nco_crcf_set_frequency(_q->nco_rx, -cargf(rxx)/16.0f);
        _q->state = OFDMFRAME64SYNC_STATE_PLCPLONG0;
        _q->timer = 0;
        _q->g = agc_crcf_get_gain(_q->sigdet);
    }
}

void ofdmframe64sync_execute_plcplong0(ofdmframe64sync _q,
                                       float complex _x)
{
    // run cross-correlator
    float complex rxy, *rc;
    windowcf_push(_q->Lt_buffer, _x);
    windowcf_push(_q->rxy_buffer, _x);
    windowcf_read(_q->rxy_buffer, &rc);
    dotprod_cccf_execute(_q->cross_correlator, rc, &rxy);

#if DEBUG_OFDMFRAME64SYNC
    windowcf_push(_q->debug_rxy,rxy);
#endif

    _q->timer++;

    if (cabsf(rxy) > 48.0f) {
#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("rxy = %12.8f (angle : %12.8f);\n", cabsf(rxy),cargf(rxy));
#endif
        //nco_crcf_set_phase(_q->nco_rx, -cargf(rxy));

        // store sequence
        // TODO : back off PLCP long by a sample or 2 to help ensure no ISI
        //memmove(_q->Lt0, rc, 64*sizeof(float complex));

        _q->state = OFDMFRAME64SYNC_STATE_PLCPLONG1;
        _q->timer = 0;
    }

    if (_q->timer > 320)
        ofdmframe64sync_reset(_q);
}

void ofdmframe64sync_execute_plcplong1(ofdmframe64sync _q,
                                       float complex _x)
{
    // push sample into cross-correlator buffer
    float complex rxy, *rc;
    windowcf_push(_q->Lt_buffer, _x);
    windowcf_push(_q->rxy_buffer, _x);

#if DEBUG_OFDMFRAME64SYNC
    windowcf_read(_q->rxy_buffer, &rc);
    dotprod_cccf_execute(_q->cross_correlator, rc, &rxy);
    windowcf_push(_q->debug_rxy,rxy);
#endif

    _q->timer++;
    if (_q->timer < 64)
        return;

    // reset timer
    _q->timer = 0;

    // run cross-correlator
    // TODO : back off PLCP long by a sample or 2 to help ensure no ISI
    windowcf_read(_q->rxy_buffer, &rc);
    dotprod_cccf_execute(_q->cross_correlator, rc, &rxy);

    // at this point we expect the cross-correlator output to be
    // high; if it's not, then the symbol 

    if (cabsf(rxy) > 48.0f) {
#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("rxy = %12.8f (angle : %12.8f);\n", cabsf(rxy),cargf(rxy));
#endif

        // store sequence
        //memmove(_q->Lt1, rc, 64*sizeof(float complex));
        windowcf_read(_q->Lt_buffer, &rc);
        // estimate frequency offset
        unsigned int j;
        float complex rxx=0.0f;
        for (j=0; j<32+64; j++)
            rxx += rc[j] * conj(rc[j+64]);
        _q->nu_hat1 = cargf(rxx)/64.0f;
        // correct frequency offset
        float theta=0.0f;
        for (j=0; j<160; j++) {
            rc[j] *= liquid_cexpjf(theta);
            theta += _q->nu_hat1;
        }
        // compute cross-correlation
        dotprod_cccf_execute(_q->cross_correlator, rc+32, &_q->rxy0);
        dotprod_cccf_execute(_q->cross_correlator, rc+32+64, &_q->rxy1);
#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("|rxy0| = %12.8f\n", cabsf(_q->rxy0));
        printf("|rxy1| = %12.8f\n", cabsf(_q->rxy1));
#endif

        memmove(_q->Lt0, rc+32-_q->backoff,     64*sizeof(float complex));
        memmove(_q->Lt1, rc+32+64-_q->backoff,  64*sizeof(float complex));

        // correct phase term, backoff
        float theta0 = cargf(_q->rxy0);
        float theta1 = cargf(_q->rxy1);
        for (j=0; j<64; j++) {
            _q->Lt0[j] *= liquid_cexpjf(theta0);
            _q->Lt1[j] *= liquid_cexpjf(theta1);
            //theta0 += -2.0f*M_PI*(float)(backoff)/64.0f;
            //theta1 += -2.0f*M_PI*(float)(backoff)/64.0f;
        }

        // run fine CFO estimation and correct offset for
        // PLCP long sequences
        //ofdmframe64sync_estimate_cfo_plcplong(_q);
        nco_crcf_adjust_frequency(_q->nco_rx, _q->nu_hat1);
#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("nu_hat0 = %12.8f;\n", _q->nu_hat0);
        printf("nu_hat1 = %12.8f;\n", _q->nu_hat1);
        printf("nu_hat  = %12.8f;\n", _q->nco_rx->d_theta);
#endif
        //ofdmframe64sync_correct_cfo_plcplong(_q);

        // compute DFT, estimate channel gains
        // TODO : determine when synchronizer should use flat gain estimation
        ofdmframe64sync_estimate_gain_plcplong(_q);
        //ofdmframe64sync_estimate_gain_plcplong_flat(_q);

        // change state
        _q->state = OFDMFRAME64SYNC_STATE_RXPAYLOAD;
    } else {
        // cross-correlator output not sufficiently high: reset synchronizer
        ofdmframe64sync_reset(_q);
    }
}

void ofdmframe64sync_compute_plcplong0(ofdmframe64sync _q)
{
    // first PLCP long sequence
    memmove(_q->x, _q->Lt0, 64*sizeof(float complex));
    FFT_EXECUTE(_q->fft);
    memmove(_q->Lf0, _q->X, 64*sizeof(float complex));
}

void ofdmframe64sync_compute_plcplong1(ofdmframe64sync _q)
{
    // second PLCP long sequence
    memmove(_q->x, _q->Lt1, 64*sizeof(float complex));
    FFT_EXECUTE(_q->fft);
    memmove(_q->Lf1, _q->X, 64*sizeof(float complex));
}
 
void ofdmframe64sync_estimate_gain_plcplong(ofdmframe64sync _q)
{
    // compute FFT on PLCP long sequences
    ofdmframe64sync_compute_plcplong0(_q);
    ofdmframe64sync_compute_plcplong1(_q);

    unsigned int i;
    float g0, theta0;
    float g1, theta1;
    for (i=0; i<64; i++) {
        if (i==0 || (i>26 && i<38)) {
            // disabled subcarrier
            _q->G0[i] = 0.0f;
            _q->G1[i] = 0.0f;
            _q->G[i]  = 0.0f;
        } else {
            // compute subcarrier gains
            _q->G0[i] = 1.0f / (_q->Lf0[i] * _q->B[i] * conj(ofdmframe64_plcp_Lf[i]));
            _q->G1[i] = 1.0f / (_q->Lf1[i] * _q->B[i] * conj(ofdmframe64_plcp_Lf[i]));

            // average amplitude, phase of subcarrier gains (note
            // that residual phase offset is taken care of by pilot
            // subcarriers)
            g0 = cabsf(_q->G0[i]);
            g1 = cabsf(_q->G1[i]);
            theta0 = cargf(_q->G0[i]);
            theta1 = cargf(_q->G1[i]);
            if (theta0 < 0) theta0 += 2.0f*M_PI;    // ensure 0 <= theta0 <= 2*pi
            if (theta1 < 0) theta1 += 2.0f*M_PI;    // ensure 0 <= theta0 <= 2*pi
#if 1
            // average amplitude and phase
            _q->G[i] = 0.5f*(g0+g1)*liquid_cexpjf(0.5f*(theta0+theta1));
#else
            // average amplitude; retain phase from first estimate
            _q->G[i] = 0.5f*(g0+g1)*liquid_cexpjf(theta0);
#endif
            //_q->G[i] = _q->G0[i]; // use only first estimate
        }
    }

    // TODO: choose smoothing factor and range appropriately
    ofdmframe64sync_smooth_gain(_q, 0.8f, 6);

#if DEBUG_OFDMFRAME64SYNC
    // correct long sequence (plotting purposes only)
    for (i=0; i<64; i++) {
        _q->Lf0[i] *= _q->G[i]*_q->B[i];
        _q->Lf1[i] *= _q->G[i]*_q->B[i];
    }
#endif
}

// smooth subcarrier gains
void ofdmframe64sync_smooth_gain(ofdmframe64sync _q,
                                 float _alpha,
                                 unsigned int _range)
{
    if (_range == 0)
        return;

    // validate inputs
    if (_alpha < 0.0f) {
        fprintf(stderr,"error: ofdmframe64sync_smooth_gain(), smoothing factor is negative\n");
        exit(1);
    } else if (_range > 32) {
        fprintf(stderr,"error: ofdmframe64sync_smooth_gain(), range is too large\n");
        exit(1);
    }

    float complex G_hat[64];// temporary gain array
    float w;                // weighting factor
    float w_total;          // sum weighting factor
    int t = (int)(_range);  // subcarrier smoothing bound
    int j;                  // subcarrier distance index
    unsigned int n;         // subcarrier index
    int sctype;
    unsigned int i;
    for (i=0; i<64; i++) {
        G_hat[i] = 0.0f;
        sctype = ofdmframe64_getsctype(i);
        if (sctype != OFDMFRAME64_SCTYPE_NULL) {
            // average adjacent subcarrier gains (smoothing)
            w_total = 0.0f;
            for (j=-t; j<=t; j++) {
                // compute subcarrier index
                n = (i+j+64)%64;

                // ignore null subcarriers
                if (ofdmframe64_getsctype(n) == OFDMFRAME64_SCTYPE_NULL)
                    continue;

                // compute weighting factor
                w = expf(-_alpha*fabsf(j*j));
                w_total += w;

                // average gain
                G_hat[i] += _q->G[n] * w;
            }
            G_hat[i] /= w_total;
        }

        // set new, smoothed gain value
        _q->G[i] = G_hat[i];
    }
}

void ofdmframe64sync_estimate_gain_plcplong_flat(ofdmframe64sync _q)
{
    // compute FFT on PLCP long sequences
    ofdmframe64sync_compute_plcplong0(_q);
    ofdmframe64sync_compute_plcplong1(_q);

    unsigned int i;
    int sctype;
    float g=0.0f;
    for (i=0; i<64; i++) {
        sctype = ofdmframe64_getsctype(i);
        if (sctype != OFDMFRAME64_SCTYPE_NULL) {
#if DEBUG_OFDMFRAME64SYNC
            // compute individual subcarrier gain, compensating for
            // fft backoff (plotting purposes only)
            _q->G0[i] = 1.0f / (_q->Lf0[i] * _q->B[i] * conj(ofdmframe64_plcp_Lf[i]));
            _q->G1[i] = 1.0f / (_q->Lf1[i] * _q->B[i] * conj(ofdmframe64_plcp_Lf[i]));
#endif

            // average amplitude of subcarriers (note that residual
            // phase offset is taken care of by pilot subcarriers)
            g += cabsf(_q->Lf0[i]) + cabsf(_q->Lf1[i]);
        } else {
            _q->G0[i] = 0.0f;
            _q->G1[i] = 0.0f;
        }
    }
    
    // average signal level over all 52 enabled subcarriers (both
    // PLCP long sequences), invert
    g = (2.0f * 52.0f) / g;

    for (i=0; i<64; i++) {
        // compute flat subcarrier gain
        _q->G[i] = g;
#if DEBUG_OFDMFRAME64SYNC
        // correct long sequence (plotting purposes only)
        // compensating for fft timing backoff
        _q->Lf0[i] *= _q->G[i]*_q->B[i];
        _q->Lf1[i] *= _q->G[i]*_q->B[i];
#endif
    }
}

void ofdmframe64sync_estimate_cfo_plcplong(ofdmframe64sync _q)
{
    float complex r=0.0f;
    unsigned int i;
    for (i=0; i<64; i++)
        r += _q->Lt0[i] * conjf(_q->Lt1[i]);

    _q->nu_hat1 = cargf(r) / 64.0f;
}

void ofdmframe64sync_correct_cfo_plcplong(ofdmframe64sync _q)
{
    // mix Lt0,Lt1 by nu_hat1 (compensate for fine CFO estimation)
    unsigned int i;
    float theta=0.0f;
    for (i=0; i<64; i++) {
        _q->Lt0[i] *= liquid_cexpjf(theta);
        _q->Lt1[i] *= liquid_cexpjf(theta);
        theta += _q->nu_hat1;
    }
}

void ofdmframe64sync_execute_rxpayload(ofdmframe64sync _q, float complex _x)
{
    _q->symbol[_q->timer] = _x;
    _q->timer++;
    if (_q->timer < 80)
        return;

    // reset timer
    _q->timer = 0;

    // copy buffer and execute FFT
    // NOTE: the -1 is used for backoff to help ensure that the FFT window
    //       does not overlap the next OFDM symbol
    // TODO: compensate equalizer phase for timing backoff
    memmove(_q->x, _q->symbol+_q->cp_len-_q->backoff, 64*sizeof(float complex));
    FFT_EXECUTE(_q->fft);

    // gain correction (equalizer)
    unsigned int i;
    for (i=0; i<64; i++) {
        _q->X[i] *= _q->G[i]*_q->B[i];
    }

    // pilot phase correction
    unsigned int pilot_phase = msequence_advance(_q->ms_pilot);
    float complex p = pilot_phase ? 1.0f : -1.0f;

    _q->y_phase[0] = cargf(_q->X[11]*conjf(p));  // -21
    _q->y_phase[1] = cargf(_q->X[25]*conjf(p));  //  -7
    _q->y_phase[2] = cargf(_q->X[39]*conjf(p));  //   7
    _q->y_phase[3] = cargf(_q->X[53]*conjf(p));  //  21

    // try to unwrap phase
    for (i=1; i<4; i++) {
        while ((_q->y_phase[i] - _q->y_phase[i-1]) >  M_PI)
            _q->y_phase[i] -= 2*M_PI;
        while ((_q->y_phase[i] - _q->y_phase[i-1]) < -M_PI)
            _q->y_phase[i] += 2*M_PI;
    }

    // fit phase to 1st-order polynomial (2 coefficients)
    polyf_fit(_q->x_phase, _q->y_phase, 4, _q->p_phase, 2);

    float theta_hat = nco_crcf_get_phase(_q->nco_pilot);
    float phase_error = _q->p_phase[0] - theta_hat;
    nco_crcf_pll_step(_q->nco_pilot, phase_error);
    nco_crcf_step(_q->nco_pilot);

#if DEBUG_OFDMFRAME64SYNC
    windowf_push(_q->debug_pilotphase,      _q->p_phase[0]);
    windowf_push(_q->debug_pilotphase_hat,  theta_hat);
#endif

    // extract average pilot phase difference and unwrap
    _q->dphi = _q->num_symbols_rx == 0 ? 0.0f : _q->p_phase[0] - _q->phi_prime;
    while (_q->dphi >  M_PI) _q->dphi -= 2.0f*M_PI;
    while (_q->dphi < -M_PI) _q->dphi += 2.0f*M_PI;
    _q->phi_prime = _q->p_phase[0];

    /*
    printf("phi(%3u) = %12.8f; dphi(%3u) = %12.8f;\n", _q->num_symbols_rx+1,
                                                       _q->phi_prime,
                                                       _q->num_symbols_rx+1,
                                                       _q->dphi);
    */
    // adjust nco frequency based on some small percentage of
    // the differential average pilot phase
    nco_crcf_adjust_frequency(_q->nco_rx, -0.1f*_q->dphi / 64.0f);

    // filter phase slope
    float zeta = 0.02f;
    _q->m_hat = zeta*_q->m_prime + (1.0f - zeta)*_q->m_hat;
    _q->m_prime = _q->p_phase[1];
    _q->p_phase[1] = _q->m_hat;
    //printf("p(%3u) = %12.8f;\n", _q->num_symbols_rx+1, _q->p_phase[1]);

    // compensate for phase/time shift
    float theta;
    /*
    _q->p_phase[0] = theta_hat;
    _q->p_phase[1] = 0.0f;
    */
    for (i=0; i<64; i++) {
        theta = polyf_val(_q->p_phase, 2, (float)(i)-32.0f);
        _q->X[i] *= liquid_cexpjf(-theta);
    }
    nco_crcf_step(_q->nco_pilot);

    // TODO: perform additional polynomial gain compensation

    // strip data subcarriers
    unsigned int j=0;
    int sctype;
    for (i=0; i<64; i++) {
        sctype = ofdmframe64_getsctype(i);
        if (sctype==OFDMFRAME64_SCTYPE_NULL) {
            // disabled subcarrier
        } else if (sctype==OFDMFRAME64_SCTYPE_PILOT) {
            // pilot subcarrier : use p/n sequence for pilot phase
        } else {
            // data subcarrier
            _q->data[j++] = _q->X[i];
        }

    }
    assert(j==48);

    _q->num_symbols_rx++;

#if DEBUG_OFDMFRAME64SYNC
    for (i=0; i<48; i++)
        windowcf_push(_q->debug_framesyms,_q->data[i]);
#endif

    if (_q->callback != NULL) {
        int retval = _q->callback(_q->data, _q->userdata);
        if (retval == -1) {
            printf("exiting prematurely\n");
            ofdmframe64sync_destroy(_q);
            exit(0);
        } else if (retval == 1) {
#if DEBUG_OFDMFRAME64SYNC_PRINT
            printf("resetting synchronizer\n");
#endif
            ofdmframe64sync_reset(_q);
        } else {
            // do nothing
        }
    }
}

