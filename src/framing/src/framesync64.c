/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
 *                                      Institute & State University
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
// framesync64.c
//
// basic frame synchronizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "liquid.internal.h"

#define DEBUG_FRAMESYNC64           1
#define DEBUG_FRAMESYNC64_PRINT     0
#define DEBUG_FILENAME              "framesync64_internal_debug.m"
#define DEBUG_BUFFER_LEN            (1600)

void framesync64_debug_print(framesync64 _q);
void framesync64_execute_seekpn(framesync64   _q,
                                float complex _x);

// push buffered p/n sequence through synchronizer
void framesync64_execute_pushpn(framesync64 _q);

void framesync64_execute_rxpayload(framesync64   _q,
                                   float complex _x);
void framesync64_decode_payload(framesync64 _q);

// advanced mode
void framesync64_csma_lock(framesync64 _q);
void framesync64_csma_unlock(framesync64 _q);

// framesync64 object structure
struct framesync64_s {
    float complex pn_sequence[64];  // 64-symbol p/n sequence
    float complex pn_syms[64];      // received p/n symbols
    float complex payload_syms[552];// payload symbols

    // synchronizer objects
    detector_cccf frame_detector;   // pre-demod detector
    windowcf buffer;                // pre-demod buffered samples, size: k*(pn_len+m)
    firpfb_crcf mf;                 // matched filter decimator
    firpfb_crcf dmf;                // derivative matched filter decimator
    
    // status variables
    enum {
        STATE_SEEKPN=0,             // seek p/n sequence
        STATE_RXPN,                 // receive p/n sequence
        STATE_RXPAYLOAD,            // receive payload data
    } state;
    float tau_hat;                  // fractional timing offset estimate
    float dphi_hat;                 // carrier frequency offset estimate
    float gamma_hat;                // channel gain estimate

#if DEBUG_FRAMESYNC64
    windowcf debug_x;                   // debug: raw input samples
#endif
};

// create framesync64 object
//  _props          :   properties structure pointer (default if NULL)
//  _callback       :   callback function invoked when frame is received
//  _userdata       :   user-defined data object passed to callback
framesync64 framesync64_create(framesyncprops_s *   _props,
                               framesync64_callback _callback,
                               void *               _userdata)
{
    framesync64 q = (framesync64) malloc(sizeof(struct framesync64_s));

    unsigned int i;

    // generate p/n sequence
    msequence ms = msequence_create(6, 0x0043, 1);
    for (i=0; i<64; i++)
        q->pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    // interpolate p/n sequence with matched filter
    unsigned int k  = 2;    // samples/symbol
    unsigned int m  = 3;    // filter delay (symbols)
    float beta      = 0.5f; // excess bandwidth factor
    float complex seq[k*64];
    interp_crcf interp = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_ARKAISER,k,m,beta,0);
    for (i=0; i<64+m; i++) {
        // compensate for filter delay
        if (i < m) interp_crcf_execute(interp, q->pn_sequence[i],    &seq[0]);
        else       interp_crcf_execute(interp, q->pn_sequence[i%64], &seq[2*(i-m)]);
    }
    interp_crcf_destroy(interp);

    // create frame detector
    float threshold = 0.4f;
    float dphi_max  = 0.05f;
    q->frame_detector = detector_cccf_create(seq, k*64, threshold, dphi_max);

    // create internal synchronizer objects
    unsigned int npfb = 32;
    q->buffer = windowcf_create(2*(64+3));
    q->mf  = firpfb_crcf_create_rnyquist(LIQUID_RNYQUIST_ARKAISER,npfb,k,m,beta);
    q->dmf = firpfb_crcf_create_drnyquist(LIQUID_RNYQUIST_ARKAISER,npfb,k,m,beta);

    // reset state

#if DEBUG_FRAMESYNC64
    q->debug_x = windowcf_create(DEBUG_BUFFER_LEN);
#endif

    return q;
}

// get framesync64 properties
//  _q     :   frame synchronizer object
//  _props  :   frame synchronizer properties structure pointer
void framesync64_getprops(framesync64 _q,
                          framesyncprops_s * _props)
{
}

// set framesync64 properties
//  _q     :   frame synchronizer object
//  _props  :   frame synchronizer properties structure pointer
void framesync64_setprops(framesync64 _q,
                          framesyncprops_s * _props)
{
}

// destroy frame synchronizer object, freeing all internal memory
void framesync64_destroy(framesync64 _q)
{
#if DEBUG_FRAMESYNC64
    framesync64_debug_print(_q);

    // clean up debug windows
    windowcf_destroy(_q->debug_x);
#endif

    // destroy synchronization objects
    detector_cccf_destroy(_q->frame_detector);
    windowcf_destroy(_q->buffer);
    firpfb_crcf_destroy(_q->mf);
    firpfb_crcf_destroy(_q->dmf);

    // free main object memory
    free(_q);
}

// print frame synchronizer object internals
void framesync64_print(framesync64 _q)
{
    printf("framesync64:\n");
}

// reset frame synchronizer object
void framesync64_reset(framesync64 _q)
{
    // reset binary pre-demod synchronizer
    detector_cccf_reset(_q->frame_detector);

    // clear pre-demod buffer
    windowcf_clear(_q->buffer);
    memset(_q->pn_syms,      0x00,  64*sizeof(float complex));
    memset(_q->payload_syms, 0x00, 138*sizeof(float complex));

    // reset state
    _q->state = STATE_SEEKPN;
}

// execute frame synchronizer
//  _q     :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void framesync64_execute(framesync64     _q,
                         float complex * _x,
                         unsigned int    _n)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
#if DEBUG_FRAMESYNC64
        windowcf_push(_q->debug_x,   _x[i]);
#endif
        switch (_q->state) {
        case STATE_SEEKPN:
            framesync64_execute_seekpn(_q, _x[i]);
            break;
        case STATE_RXPAYLOAD:
            framesync64_execute_rxpayload(_q, _x[i]);
            break;
        default:
            fprintf(stderr,"error: framesync64_exeucte(), unknown/unsupported state\n");
            exit(1);
        }
    }
}

// 
// internal
//

// execute synchronizer, seeking p/n sequence
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_seekpn(framesync64   _q,
                                float complex _x)
{
    float tau_hat   = 0.0f;
    float dphi_hat  = 0.0f;
    float gamma_hat = 0.0f;
    int   detected  = 0;

    //
    windowcf_push(_q->buffer, _x);

    // push through pre-demod synchronizer
    detected = detector_cccf_correlate(_q->frame_detector, _x, &tau_hat, &dphi_hat, &gamma_hat);
    if (detected) {
        printf("***** frame detected! tau-hat:%8.4f, dphi-hat:%8.4f, gamma:%8.2f dB\n",
                tau_hat, dphi_hat, 20*log10f(gamma_hat));

        // TODO: push buffered samples through synchronizer
        _q->tau_hat   = tau_hat;
        _q->dphi_hat  = dphi_hat;
        _q->gamma_hat = gamma_hat;
        framesync64_execute_pushpn(_q);

        // update state
        _q->state = STATE_RXPAYLOAD;
    }
}

// push buffered p/n sequence through synchronizer
void framesync64_execute_pushpn(framesync64 _q)
{
    float complex * rc;
    windowcf_read(_q->buffer, &rc);
    
    unsigned int i;
    unsigned int n=0;
    unsigned int delay = 0;
    // TODO: set filterbank index based on tau_hat
    unsigned int index = 0;
    for (i=0; i<(64+3)*2; i++) {
        firpfb_crcf_push(_q->mf, rc[i] * 0.5f / _q->gamma_hat);

        if (i < delay)
            continue;

        if ( (i%2)==1 )
            firpfb_crcf_execute(_q->mf, index, &_q->pn_syms[n++]);

        if (n == 64)
            break;
    }
    printf("n=%u\n", n);
}

// execute synchronizer, receiving payload
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_rxpayload(framesync64   _q,
                                   float complex _x)
{
}

// enable csma and set external callback functions
//  _q             :   frame synchronizer object
//  _csma_lock      :   callback to be invoked when signal is high
//  _csma_unlock    :   callback to be invoked when signal is again low
//  _csma_userdata  :   structure passed to callback functions
void framesync64_set_csma_callbacks(framesync64 _q,
                                    framesync_csma_callback _csma_lock,
                                    framesync_csma_callback _csma_unlock,
                                    void * _csma_userdata)
{
}

// if enabled, invoke external csma lock callback
void framesync64_csma_lock(framesync64 _q)
{
}

// if enabled, invoke external csma unlock callback
void framesync64_csma_unlock(framesync64 _q)
{
}


// 
// decoding methods
//

void framesync64_decode_payload(framesync64 _q)
{
}

// convert four 2-bit symbols into one 8-bit byte
//  _syms   :   input symbols [size: 4 x 1]
//  _byte   :   output byte
void framesync64_syms_to_byte(unsigned char * _syms,
                              unsigned char * _byte)
{
    unsigned char b=0;
    b |= (_syms[0] << 6) & 0xc0;
    b |= (_syms[1] << 4) & 0x30;
    b |= (_syms[2] << 2) & 0x0c;
    b |= (_syms[3]     ) & 0x03;
    *_byte = b;
}

// huge method to write debugging data to file
void framesync64_debug_print(framesync64 _q)
{
#if DEBUG_FRAMESYNC64
    unsigned int i;
    float complex * rc;
    FILE* fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file", DEBUG_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    // write x
    fprintf(fid,"x = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    // write pre-demod sample buffer
    fprintf(fid,"presync_samples = zeros(1,2*(64+3));\n");
    windowcf_read(_q->buffer, &rc);
    for (i=0; i<2*(64+3); i++)
        fprintf(fid,"presync_samples(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));

    // write p/n sequence
    fprintf(fid,"pn_sequence = zeros(1,64);\n");
    rc = _q->pn_sequence;
    for (i=0; i<64; i++)
        fprintf(fid,"pn_sequence(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));

    // write p/n symbols
    fprintf(fid,"pn_syms = zeros(1,64);\n");
    rc = _q->pn_syms;
    for (i=0; i<64; i++)
        fprintf(fid,"pn_syms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));

    // write payload symbols
    fprintf(fid,"payload_syms = zeros(1,138);\n");
    rc = _q->payload_syms;
    for (i=0; i<138; i++)
        fprintf(fid,"payload_syms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(real(payload_syms),imag(payload_syms),'x',...\n");
    fprintf(fid,"     real(pn_syms),     imag(pn_syms),     'x');\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"legend('p/n syms','payload syms','location','northeast');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.3);\n");
    fprintf(fid,"axis square;\n");

    fprintf(fid,"\n\n");
    fclose(fid);

    printf("framesync64/debug: results written to %s\n", DEBUG_FILENAME);
#endif
}
