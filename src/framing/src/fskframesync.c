/*
 * Copyright (c) 2007 - 2016 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// fskframesync.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_FSKFRAMESYNC             1
#define DEBUG_FSKFRAMESYNC_PRINT       0
#define DEBUG_FSKFRAMESYNC_FILENAME    "fskframesync_debug.m"
#define DEBUG_FSKFRAMESYNC_BUFFER_LEN  (2000)

// execute stages
void fskframesync_execute_detectframe(fskframesync _q, float complex _x);
void fskframesync_execute_rxheader(   fskframesync _q, float complex _x);
void fskframesync_execute_rxpayload(  fskframesync _q, float complex _x);

// decode header
void fskframesync_decode_header(fskframesync _q);

// fskframesync object structure
struct fskframesync_s {
    unsigned int    m;                  // demodulator bits/symbol
    unsigned int    k;                  // demodulator samples/symbol
    float           bandwidth;          // demodulator bandwidth
    unsigned int    M;                  // demodulator constellation size, M=2^m
    fskdem          dem;                // demodulator object (M-FSK)
    float complex * buf;                // demodulator transmit buffer [size: k x 1]

    framesync_callback  callback;       // user-defined callback function
    void *              userdata;       // user-defined data structure
    framesyncstats_s    framestats;     // frame statistic object

    // synchronizer objects, states
    firpfb_crcf     pfb;                // timing recovery
    nco_crcf        nco;                // coarse carrier frequency recovery
    firfilt_rrrf    detector;           // frame correlator detector
    windowcf        buf_rx;             // pre-demod buffered samples, size: k*(pn_len+m)

    // header
#if 0
    unsigned int    header_dec_len;     // header decoded message length
    crc_scheme      header_crc;         // header validity check
    fec_scheme      header_fec0;        // header inner code
    fec_scheme      header_fec1;        // header outer code
    packetizer      header_encoder;     // header encoder
    unsigned int    header_enc_len;     // header encoded message length
    unsigned char * header_dec;         // header uncoded [size: header_dec_len x 1]
    unsigned char * header_enc;         // header encoded [size: header_enc_len x 1]
    unsigned int    header_sym_len;     // header symbols length
#else
    unsigned int    header_dec_len;     //
    unsigned int    header_sym_len;     //
    unsigned char * header_dec;         // header uncoded [size: header_dec_len x 1]
    unsigned char * header_sym;         // header: unmodulated symbols
    qpacketmodem    header_encoder;     //
#endif

    // payload
#if 0
    unsigned int    payload_dec_len;    // payload decoded message length
    crc_scheme      payload_crc;        // payload validity check
    fec_scheme      payload_fec0;       // payload inner code
    fec_scheme      payload_fec1;       // payload outer code
    packetizer      payload_encoder;    // payload encoder
    unsigned int    payload_enc_len;    // payload encoded message length
    unsigned char * payload_enc;        // paylaod encoded [size: payload_enc_len x 1]
    unsigned int    payload_sym_len;    // payload symbols length
#else
    unsigned int    payload_dec_len;    //
    crc_scheme      payload_crc;        // payload validity check
    fec_scheme      payload_fec0;       // payload inner code
    fec_scheme      payload_fec1;       // payload outer code
    unsigned int    payload_sym_len;    //
    unsigned char * payload_sym;        //
    qpacketmodem    payload_encoder;    //
#endif

    // framing state
    enum {
                    STATE_DETECTFRAME=0,// preamble
                    STATE_RXHEADER,     // header
                    STATE_RXPAYLOAD,    // payload (frame)
    }               state;
    int             frame_assembled;    // frame assembled flag
    int             frame_complete;     // frame completed flag
    unsigned int    sample_counter;     // output sample counter
    unsigned int    symbol_counter;     // output symbol counter
    unsigned int    timer;              // sample timer
    // debugging structures
#if DEBUG_FSKFRAMESYNC
    int             debug_enabled;          // debugging enabled?
    int             debug_objects_created;  // debugging objects created?
    windowcf        debug_x;                // received samples buffer
#endif
};

// create GMSK frame synchronizer
//  _callback   :   callback function
//  _userdata   :   user data pointer passed to callback function
fskframesync fskframesync_create(framesync_callback _callback,
                                 void *             _userdata)
{
    fskframesync q = (fskframesync) malloc(sizeof(struct fskframesync_s));
    
    // set static values
    q->callback  = _callback;
    q->userdata  = _userdata;
    q->m         = 4;
    q->M         = 1 << q->m;
    q->k         = 2 << q->m;
    q->bandwidth = 0.4f;

    // create demodulator
    q->dem = fskdem_create(q->m, q->k, q->bandwidth);
    q->buf = (float complex*) malloc( q->k * sizeof(float complex) );

    // create polyphase filterbank for timing recovery
    q->pfb = firpfb_crcf_create_kaiser(64, 5, 0.45f, 40.0f);

    // create oscillator for frequency recovery
    q->nco = nco_crcf_create(LIQUID_VCO);

    // create buffer for demodulator input
    q->buf_rx = windowcf_create(q->k);

    // create preamble frame detector from preamble symbols (over-sampled by 2)
    msequence preamble_ms = msequence_create(6, 0x6d, 1);
    unsigned int preamble_sym_len = 64;
    float * preamble = (float*) malloc( 2*preamble_sym_len*sizeof(float) );
    unsigned int i;
    for (i=0; i<preamble_sym_len; i++) {
        preamble[2*i+0] = msequence_advance(preamble_ms) ? 1.0f : -1.0f;
        preamble[2*i+1] = preamble[2*i+0];
    }
    q->detector = firfilt_rrrf_create(preamble, 2*preamble_sym_len);
    free(preamble);
    msequence_destroy(preamble_ms);

    // header objects/arrays
#if 0
    q->header_dec_len   = 10;
    q->header_crc       = LIQUID_CRC_32;
    q->header_fec0      = LIQUID_FEC_NONE;
    q->header_fec1      = LIQUID_FEC_GOLAY2412;
    q->header_encoder   = packetizer_create(q->header_dec_len,
                                            q->header_crc,
                                            q->header_fec0,
                                            q->header_fec1);
    q->header_enc_len   = packetizer_get_dec_msg_len(q->header_encoder);
    q->header_dec       = (unsigned char*)malloc(q->header_dec_len*sizeof(unsigned char));
    q->header_enc       = (unsigned char*)malloc(q->header_enc_len*sizeof(unsigned char));
    q->header_sym_len   = q->header_enc_len * 8 / q->m;
#else
    q->header_dec_len   = 10;
    q->header_dec       = (unsigned char*)malloc(q->header_dec_len*sizeof(unsigned char));
    q->header_encoder   = qpacketmodem_create();
    qpacketmodem_configure(q->header_encoder,
                           q->header_dec_len,
                           LIQUID_CRC_32,
                           LIQUID_FEC_NONE,
                           LIQUID_FEC_GOLAY2412,
                           LIQUID_MODEM_QPSK);  // TODO: set bits/sym appropriately
    q->header_sym_len   = qpacketmodem_get_frame_len(q->header_encoder);
    q->header_sym       = (unsigned char*)malloc(q->header_sym_len*sizeof(unsigned char));
#endif

    // payload objects/arrays
#if 0
    q->payload_dec_len  = 10;
    q->payload_crc      = LIQUID_CRC_32;
    q->payload_fec0     = LIQUID_FEC_NONE;
    q->payload_fec1     = LIQUID_FEC_GOLAY2412;
    q->payload_encoder  = packetizer_create(q->payload_dec_len,
                                            q->payload_crc,
                                            q->payload_fec0,
                                            q->payload_fec1);
    q->payload_enc_len  = packetizer_get_dec_msg_len(q->payload_encoder);
    q->payload_enc      = (unsigned char*)malloc(q->payload_enc_len*sizeof(unsigned char));
    q->payload_sym_len  = 0;    // TODO: set this appropriately
#else
    q->payload_dec_len  = 10;
    q->payload_crc      = LIQUID_CRC_32;
    q->payload_fec0     = LIQUID_FEC_NONE;
    q->payload_fec1     = LIQUID_FEC_GOLAY2412;
    q->payload_encoder  = qpacketmodem_create();
    qpacketmodem_configure(q->payload_encoder,
                           q->payload_dec_len,
                           q->payload_crc,
                           q->payload_fec0,
                           q->payload_fec1,
                           LIQUID_MODEM_QPSK);  // TODO: set bits/sym appropriately
    q->payload_sym_len  = qpacketmodem_get_frame_len(q->payload_encoder);
    q->payload_sym      = (unsigned char*)malloc(q->payload_sym_len*sizeof(unsigned char));
#endif

#if DEBUG_FSKFRAMESYNC
    // debugging structures
    q->debug_enabled         = 0;
    q->debug_objects_created = 0;
    q->debug_x               = NULL;
#endif

    // reset synchronizer
    fskframesync_reset(q);

    // return synchronizer object
    return q;
}


// destroy frame synchronizer object, freeing all internal memory
void fskframesync_destroy(fskframesync _q)
{
#if DEBUG_FSKFRAMESYNC
    // destroy debugging objects
    if (_q->debug_objects_created) {
        windowcf_destroy(_q->debug_x);
    }
#endif

    // destroy modulator
    fskdem_destroy(_q->dem);
    free(_q->buf);

    // reset symbol timing recovery state
    firpfb_crcf_destroy(_q->pfb);

    // reset carrier recovery objects
    nco_crcf_destroy(_q->nco);

    // clear pre-demod buffer
    windowcf_destroy(_q->buf_rx);

    // reset internal objects
    firfilt_rrrf_destroy(_q->detector);

    // destroy/free header objects/arrays
#if 0
    free(_q->header_dec);
    free(_q->header_enc);
    packetizer_destroy(_q->header_encoder);
#else
    free(_q->header_dec);
    free(_q->header_sym);
    qpacketmodem_destroy(_q->header_encoder);
#endif

    // destroy/free payload objects/arrays
#if 0
    free(_q->payload_enc);
    packetizer_destroy(_q->payload_encoder);
#else
    free(_q->payload_sym);
    qpacketmodem_destroy(_q->payload_encoder);
#endif

    // free main object memory
    free(_q);
}

// print frame synchronizer object internals
void fskframesync_print(fskframesync _q)
{
    printf("fskframesync:\n");
}

// reset frame synchronizer object
void fskframesync_reset(fskframesync _q)
{
    // reset symbol timing recovery state
    firpfb_crcf_reset(_q->pfb);

    // reset carrier recovery objects
    nco_crcf_reset(_q->nco);

    // clear pre-demod buffer
    windowcf_clear(_q->buf_rx);

    // reset internal objects
    firfilt_rrrf_reset(_q->detector);

    // reset state and counters
    _q->state            = STATE_DETECTFRAME;
    _q->sample_counter   = 0;
    _q->symbol_counter   = 0;
    _q->timer            = 0;
}

// execute frame synchronizer
//  _q      :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void fskframesync_execute(fskframesync    _q,
                          float complex * _x,
                          unsigned int    _n)
{
    // push through synchronizer
    unsigned int i;
    for (i=0; i<_n; i++) {
        float complex xf;   // input sample
        xf = _x[i];

#if DEBUG_FSKFRAMESYNC
        if (_q->debug_enabled)
            windowcf_push(_q->debug_x, xf);
#endif

        switch (_q->state) {
        case STATE_DETECTFRAME:
            // look for p/n sequence
            fskframesync_execute_detectframe(_q, xf);
            break;

        case STATE_RXHEADER:
            // receive header
            fskframesync_execute_rxheader(_q, xf);
            break;

        case STATE_RXPAYLOAD:
            // receive payload
            fskframesync_execute_rxpayload(_q, xf);
            break;
        }
    }
}

// 
// internal methods
//

void fskframesync_execute_detectframe(fskframesync  _q,
                                      float complex _x)
{
    // push sample through timing recovery and compute output
    float complex y;
    firpfb_crcf_push(_q->pfb, _x);
    firpfb_crcf_execute(_q->pfb, 0, &y);

    // push sample into pre-demod p/n sequence buffer
    windowcf_push(_q->buf_rx, y);

    // decrement timer and determine if symbol output is ready
    _q->timer--;
    if (_q->timer)
        return;

    // reset timer
    _q->timer = _q->k;

    // run demodulator and retrieve FFT result, computing LLR sample output

    // push LLR sample into frame detector
    int detected = 0;

    // check if frame has been detected
    if (detected) {
        printf("### fskframe detected! ###\n");

        // update state...
        //_q->state = STATE_RXHEADER;
    }
}

void fskframesync_execute_rxheader(fskframesync _q,
                                    float complex _x)
{
}

void fskframesync_execute_rxpayload(fskframesync  _q,
                                    float complex _x)
{
}

// decode header and re-configure payload decoder
void fskframesync_decode_header(fskframesync _q)
{
}

void fskframesync_debug_enable(fskframesync _q)
{
    // create debugging objects if necessary
#if DEBUG_FSKFRAMESYNC
    if (!_q->debug_objects_created) {
        _q->debug_x  = windowcf_create(DEBUG_FSKFRAMESYNC_BUFFER_LEN);
    }
    
    // set debugging flags
    _q->debug_enabled = 1;
    _q->debug_objects_created = 1;
#else
    fprintf(stderr,"fskframesync_debug_enable(): compile-time debugging disabled\n");
#endif
}

void fskframesync_debug_disable(fskframesync _q)
{
#if DEBUG_FSKFRAMESYNC
    _q->debug_enabled = 0;
#else
    fprintf(stderr,"fskframesync_debug_disable(): compile-time debugging disabled\n");
#endif
}

void fskframesync_debug_export(fskframesync _q,
                               const char * _filename)
{
#if DEBUG_FSKFRAMESYNC
    if (!_q->debug_objects_created) {
        fprintf(stderr,"error: fskframe_debug_print(), debugging objects don't exist; enable debugging first\n");
        return;
    }

    FILE* fid = fopen(_filename,"w");
    if (!fid) {
        fprintf(stderr, "error: fskframesync_debug_print(), could not open '%s' for writing\n", _filename);
        return;
    }
    fprintf(fid,"%% %s: auto-generated file", _filename);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    fprintf(fid,"num_samples = %u;\n", DEBUG_FSKFRAMESYNC_BUFFER_LEN);
    fprintf(fid,"t = 0:(num_samples-1);\n");
    unsigned int i;
    float complex * rc;

    // write x
    fprintf(fid,"x = zeros(1,num_samples);\n");
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_FSKFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");
    fprintf(fid,"\n\n");

    fclose(fid);
    printf("fskframesync/debug: results written to '%s'\n", _filename);
#else
    fprintf(stderr,"fskframesync_debug_print(): compile-time debugging disabled\n");
#endif
}

