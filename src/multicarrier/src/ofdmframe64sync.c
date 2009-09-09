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

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

#define DEBUG_OFDMFRAME64SYNC             1
#define DEBUG_OFDMFRAME64SYNC_PRINT       1
#define DEBUG_OFDMFRAME64SYNC_FILENAME    "ofdmframe64sync_internal_debug.m"
#define DEBUG_OFDMFRAME64SYNC_BUFFER_LEN  (1024)

struct ofdmframe64sync_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    float complex * x; // time-domain buffer
    float complex * X; // freq-domain buffer

    // initial gain correction / signal detection
    agc sigdet;
    float g;

    // gain correction...
    float complex G[64];
    float x_phase[4];       // 
    float y_phase[4];       // 
    float p_phase[2];       // polynomial fit to phase

    // carrier offset correction
    nco nco_rx;

    // PLCP SHORT : delay correlator
    autocorr_cccf delay_correlator;
    float complex rxx_max;

    // PLCP LONG: cross correlator
    dotprod_cccf cross_correlator;
    float complex rxy;
    cfwindow rxy_buffer;
    float complex Lt0[64], Lf0[64]; // received PLCP long sequence (first)
    float complex Lt1[64], Lf1[64]; // received PLCP long sequence (second)
    float complex G0[64], G1[64];

    // timer
    unsigned int symbol_timer;
    float complex symbol[80];
    float complex data[48];

    float zeta;         // scaling factor
    float nu_hat0;      // carrier frequency offset estimation (coarse)
    float nu_hat1;      // carrier frequency offset estimation (fine)
    float dt_hat;       // symbol timing offset estimation

    msequence ms_pilot;

#if HAVE_FFTW3_H
    fftwf_plan fft;
#else
    fftplan fft;
#endif

    ofdmframe64sync_callback callback;
    void * userdata;

    // state
    enum {
        OFDMFRAME64SYNC_STATE_PLCPSHORT=0,
        OFDMFRAME64SYNC_STATE_PLCPLONG0,
        OFDMFRAME64SYNC_STATE_PLCPLONG1,
        OFDMFRAME64SYNC_STATE_RXPAYLOAD
    } state;

#if DEBUG_OFDMFRAME64SYNC
    cfwindow debug_x;
    cfwindow debug_rxx;
    cfwindow debug_rxy;
    cfwindow debug_framesyms;
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

    // initial gain correction / signal detection
    q->sigdet = agc_create(1.0f, 0.1f);

    // carrier offset correction
    q->nco_rx = nco_create();

    // cyclic prefix correlation windows
    q->delay_correlator = autocorr_cccf_create(64,16);
    q->zeta = sqrtf(1.0f / 64.0f * 52.0f / 64.0f);

    //
    unsigned int i;
    float complex h[64];
    for (i=0; i<64; i++)
        h[i] = conjf(ofdmframe64_plcp_Lt[i]);
    q->cross_correlator = dotprod_cccf_create(h,64);
    q->rxy_buffer = cfwindow_create(64);
    
#if DEBUG_OFDMFRAME64SYNC
    q->debug_x   = cfwindow_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxx = cfwindow_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    q->debug_rxy = cfwindow_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
    q->debug_framesyms = cfwindow_create(DEBUG_OFDMFRAME64SYNC_BUFFER_LEN);
#endif

    // pilot sequence generator
    q->ms_pilot = msequence_create(8);
    q->x_phase[0] = -21.0f;
    q->x_phase[1] =  -7.0f;
    q->x_phase[2] =   7.0f;
    q->x_phase[3] =  21.0f;

    q->callback = _callback;
    q->userdata = _userdata;

    ofdmframe64sync_reset(q);

    return q;
}

void ofdmframe64sync_destroy(ofdmframe64sync _q)
{
#if DEBUG_OFDMFRAME64SYNC
    ofdmframe64sync_debug_print(_q);
    cfwindow_destroy(_q->debug_x);
    cfwindow_destroy(_q->debug_rxx);
    cfwindow_destroy(_q->debug_rxy);
    cfwindow_destroy(_q->debug_framesyms);
#endif

    free(_q->x);
    free(_q->X);
    cfwindow_destroy(_q->rxy_buffer);
#if HAVE_FFTW3_H
    fftwf_destroy_plan(_q->fft);
#else
    fft_destroy_plan(_q->fft);
#endif

    agc_destroy(_q->sigdet);
    msequence_destroy(_q->ms_pilot);
    autocorr_cccf_destroy(_q->delay_correlator);
    dotprod_cccf_destroy(_q->cross_correlator);
    nco_destroy(_q->nco_rx);
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
    agc_reset(_q->sigdet);
    _q->state = OFDMFRAME64SYNC_STATE_PLCPSHORT;
    autocorr_cccf_clear(_q->delay_correlator);
    _q->rxx_max = 0.0f;
    nco_set_frequency(_q->nco_rx, 0.0f);
    nco_set_phase(_q->nco_rx, 0.0f);

    unsigned int i;
    for (i=0; i<64; i++) {
        // clear PLCP long buffers
        _q->Lt0[i] = 0.0f;
        _q->Lt1[i] = 0.0f;

        // reset gain
        _q->G[i] = 1.0f;
    }

    // reset symbol timer
    _q->symbol_timer = 0;
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
        cfwindow_push(_q->debug_x,x);
#endif

        // coarse gain correction
        x *= _q->g;
        
        // carrier frequency offset estimation
        nco_mix_up(_q->nco_rx, x, &x);

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
    cfwindow_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),real(x),0:(n-1),imag(x));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    fprintf(fid,"rxx = zeros(1,n);\n");
    cfwindow_read(_q->debug_rxx, &rc);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(0:(n-1),abs(rxx));\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('|r_{xx}|');\n");

    fprintf(fid,"rxy = zeros(1,n);\n");
    cfwindow_read(_q->debug_rxy, &rc);
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

    // frame symbols
    fprintf(fid,"framesyms = zeros(1,n);\n");
    cfwindow_read(_q->debug_framesyms, &rc);
    for (i=0; i<DEBUG_OFDMFRAME64SYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(framesyms),imag(framesyms),'x','MarkerSize',1);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title('Frame Symbols');\n");

    fclose(fid);
    printf("ofdmframe64sync/debug: results written to %s\n", DEBUG_OFDMFRAME64SYNC_FILENAME);
#endif
}

void ofdmframe64sync_execute_plcpshort(ofdmframe64sync _q,
                                       float complex _x)
{
    // run AGC, clip output
    float complex y;
    agc_execute(_q->sigdet, _x, &y);
    //if (agc_get_signal_level(_q->sigdet) < -15.0f)
    //    return;
    if (cabsf(y) > 2.0f)
        y = 2.0f*cexpf(_Complex_I*cargf(y));

    // run auto-correlator
    float complex rxx;
    autocorr_cccf_push(_q->delay_correlator, y);
    autocorr_cccf_execute(_q->delay_correlator, &rxx);

#if DEBUG_OFDMFRAME64SYNC
    cfwindow_push(_q->debug_rxx,rxx);
#endif

    if (cabsf(rxx) > 48.0f) {
        // TODO : wait for auto-correlation to peak before changing state

#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("rxx = %12.8f (angle : %12.8f);\n", cabsf(rxx),cargf(rxx)/16.0f);
#endif
        _q->nu_hat0 = -cargf(rxx)/16.0f;
        nco_set_frequency(_q->nco_rx, -cargf(rxx)/16.0f);
        _q->state = OFDMFRAME64SYNC_STATE_PLCPLONG0;
        _q->symbol_timer = 0;
        _q->g = agc_get_gain(_q->sigdet);
    }
}

void ofdmframe64sync_execute_plcplong0(ofdmframe64sync _q,
                                       float complex _x)
{
    // run cross-correlator
    float complex rxy, *rc;
    cfwindow_push(_q->rxy_buffer, _x);
    cfwindow_read(_q->rxy_buffer, &rc);
    dotprod_cccf_execute(_q->cross_correlator, rc, &rxy);

#if DEBUG_OFDMFRAME64SYNC
    cfwindow_push(_q->debug_rxy,rxy);
#endif

    _q->symbol_timer++;

    if (cabsf(rxy) > 48.0f) {
#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("rxy = %12.8f (angle : %12.8f);\n", cabsf(rxy),cargf(rxy));
#endif
        //nco_set_phase(_q->nco_rx, -cargf(rxy));

        // store sequence
        memmove(_q->Lt0, rc, 64*sizeof(float complex));

        _q->state = OFDMFRAME64SYNC_STATE_PLCPLONG1;
        _q->symbol_timer = 0;
    }

    if (_q->symbol_timer > 320)
        ofdmframe64sync_reset(_q);
}

void ofdmframe64sync_execute_plcplong1(ofdmframe64sync _q,
                                       float complex _x)
{
    // push sample into cross-correlator buffer
    float complex rxy, *rc;
    cfwindow_push(_q->rxy_buffer, _x);

#if DEBUG_OFDMFRAME64SYNC
    cfwindow_read(_q->rxy_buffer, &rc);
    dotprod_cccf_execute(_q->cross_correlator, rc, &rxy);
    cfwindow_push(_q->debug_rxy,rxy);
#endif

    _q->symbol_timer++;
    if (_q->symbol_timer < 64)
        return;

    // reset timer
    _q->symbol_timer = 0;

    // run cross-correlator
    cfwindow_read(_q->rxy_buffer, &rc);
    dotprod_cccf_execute(_q->cross_correlator, rc, &rxy);

    // at this point we expect the cross-correlator output to be
    // high; if it's not, then the symbol 

    if (cabsf(rxy) > 48.0f) {
#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("rxy = %12.8f (angle : %12.8f);\n", cabsf(rxy),cargf(rxy));
#endif

        // store sequence
        memmove(_q->Lt1, rc, 64*sizeof(float complex));

        // run fine CFO estimation and correct offset for
        // PLCP long sequences
        ofdmframe64sync_estimate_cfo_plcplong(_q);
        nco_adjust_frequency(_q->nco_rx, _q->nu_hat1);
#if DEBUG_OFDMFRAME64SYNC_PRINT
        printf("nu_hat = %12.8f;\n", _q->nco_rx->d_theta);
#endif
        ofdmframe64sync_correct_cfo_plcplong(_q);

        // compute DFT, estimate channel gains
        ofdmframe64sync_estimate_gain_plcplong(_q);

        // change state
        _q->state = OFDMFRAME64SYNC_STATE_RXPAYLOAD;
    } else {
        // cross-correlator output not sufficiently high: reset synchronizer
        ofdmframe64sync_reset(_q);
    }
}

void ofdmframe64sync_estimate_gain_plcplong(ofdmframe64sync _q)
{
    // first PLCP long sequence
    memmove(_q->x, _q->Lt0, 64*sizeof(float complex));
#if HAVE_FFTW3_H
    fftwf_execute(_q->fft);
#else
    fft_execute(_q->fft);
#endif
    memmove(_q->Lf0, _q->X, 64*sizeof(float complex));

    // second PLCP long sequence
    memmove(_q->x, _q->Lt1, 64*sizeof(float complex));
#if HAVE_FFTW3_H
    fftwf_execute(_q->fft);
#else
    fft_execute(_q->fft);
#endif
    memmove(_q->Lf1, _q->X, 64*sizeof(float complex));

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
            _q->G0[i] = 1.0f / (_q->Lf0[i] * conj(ofdmframe64_plcp_Lf[i]));
            _q->G1[i] = 1.0f / (_q->Lf1[i] * conj(ofdmframe64_plcp_Lf[i]));

            // average amplitude, phase of subcarrier gains (note
            // that residual phase offset is taken care of by pilot
            // subcarriers)
            g0 = cabsf(_q->G0[i]);
            g1 = cabsf(_q->G1[i]);
            theta0 = cargf(_q->G0[i]);
            theta1 = cargf(_q->G1[i]);
            if (theta0 < 0) theta0 += 2.0f*M_PI;    // ensure 0 <= theta0 <= 2*pi
            if (theta1 < 0) theta1 += 2.0f*M_PI;    // ensure 0 <= theta0 <= 2*pi
#if 0
            // average amplitude and phase
            _q->G[i] = 0.5f*(g0+g1)*cexpf(_Complex_I*0.5f*(theta0+theta1));
#else
            // average amplitude; retain phase from first estimate
            _q->G[i] = 0.5f*(g0+g1)*cexpf(_Complex_I*theta0);
#endif
            //_q->G[i] = _q->G0[i]; // use only first estimate
        }
#if DEBUG_OFDMFRAME64SYNC
        // correct long sequence (plotting purposes only)
        _q->Lf0[i] *= _q->G[i];
        _q->Lf1[i] *= _q->G[i];
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
        _q->Lt0[i] *= cexpf(_Complex_I*theta);
        _q->Lt1[i] *= cexpf(_Complex_I*theta);
        theta += _q->nu_hat1;
    }
}

void ofdmframe64sync_execute_rxpayload(ofdmframe64sync _q, float complex _x)
{
    _q->symbol[_q->symbol_timer] = _x;
    _q->symbol_timer++;
    if (_q->symbol_timer < 80)
        return;

    // reset timer
    _q->symbol_timer = 0;

    // copy buffer and execute FFT
    memmove(_q->x, _q->symbol+16, 64*sizeof(float complex));
#if HAVE_FFTW3_H
    fftwf_execute(_q->fft);
#else
    fft_execute(_q->fft);
#endif

    // gain correction (equalizer)
    unsigned int i;
    for (i=0; i<64; i++) {
        _q->X[i] *= _q->G[i];
    }
    _q->y_phase[0] = cargf(_q->X[11]);  // -21
    _q->y_phase[1] = cargf(_q->X[25]);  //  -7
    _q->y_phase[2] = cargf(_q->X[39]);  //   7
    _q->y_phase[3] = cargf(_q->X[53]);  //  21

    // try to unwrap phase
    for (i=1; i<4; i++) {
        while ((_q->y_phase[i] - _q->y_phase[i-1]) >  M_PI)
            _q->y_phase[i] -= 2*M_PI;
        while ((_q->y_phase[i] - _q->y_phase[i-1]) < -M_PI)
            _q->y_phase[i] += 2*M_PI;
    }

    // pilot phase correction
    unsigned int pilot_phase = msequence_advance(_q->ms_pilot);
    if (pilot_phase==0) {
        for (i=0; i<4; i++)
            _q->y_phase[i] -= M_PI;
    }

    // fit phase to 1st-order polynomial (2 coefficients)
    polyfit(_q->x_phase, _q->y_phase, 4, _q->p_phase, 2);

    // compensate for phase/time shift
    float theta;
    for (i=0; i<64; i++) {
        theta = polyval(_q->p_phase, 2, (float)(i)-32.0f);
        _q->X[i] *= cexpf(-_Complex_I*theta);
    }

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

#if DEBUG_OFDMFRAME64SYNC
    for (i=0; i<48; i++)
        cfwindow_push(_q->debug_framesyms,_q->data[i]);
#endif

    if (_q->callback != NULL) {
        int retval = _q->callback(_q->data, _q->userdata);
        if (retval == -1) {
            printf("exiting prematurely\n");
            ofdmframe64sync_destroy(_q);
            exit(0);
        } else if (retval == 1) {
            printf("resetting synchronizer\n");
            ofdmframe64sync_reset(_q);
        } else {
            // do nothing
        }
    }
}

