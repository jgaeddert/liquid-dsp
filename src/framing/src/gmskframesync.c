/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// gmskframesync.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>

#include "liquid.internal.h"

#define DEBUG_GMSKFRAMESYNC             0
#define DEBUG_GMSKFRAMESYNC_PRINT       0
#define DEBUG_GMSKFRAMESYNC_FILENAME    "gmskframesync_internal_debug.m"
#define DEBUG_GMSKFRAMESYNC_BUFFER_LEN  (2000)

// gmskframesync object structure
struct gmskframesync_s {
    unsigned int k;                     // filter samples/symbol
    unsigned int m;                     // filter semi-length (symbols)
    float BT;                           // filter bandwidth-time product

    // synchronizer parameters, objects
    float complex x_prime;              // received signal state
    float rssi_hat;                     // rssi estimate
    float evm_hat;                      // evm estimate
    unsigned int npfb;                  // number of filterbanks
    symsync_rrrf symsync;               // symbol synchronizer, matched filter

    // preamble
    bsequence bx;                       // binary sequence correlator (ms-equence)
    bsequence by;                       // binary sequence correlator (received sequence)
    
    // header
    unsigned char * header_user;        // user-defined header [GMSKFRAME_H_USER]
    unsigned char * header_enc;         // encoded header [GMSKFRAME_H_ENC]
    unsigned char * header_dec;         // uncoded header [GMSKFRAME_H_DEC]
    packetizer p_header;                // header packetizer
    int header_valid;                   // header valid flag

    // payload
    packetizer p_payload;               // payload packetizer
    crc_scheme check;                   // data validity check (e.g. CRC)
    fec_scheme fec0;                    // inner forward error correction
    fec_scheme fec1;                    // outer forward error correction
    unsigned int dec_msg_len;           // 
    unsigned int enc_msg_len;           // 
    unsigned char * payload_enc;        // encoded payload
    unsigned char * payload_dec;        // decoded payload
    int payload_valid;                  // payload valid flag

    // callback
    gmskframesync_callback callback;    // user-defined callback function
    void * userdata;                    // user-defined data structure
    framesyncstats_s framestats;        //

    // framing state
    enum {
        GMSKFRAMESYNC_STATE_SEEKPN,     // preamble
        GMSKFRAMESYNC_STATE_RXHEADER,   // header
        GMSKFRAMESYNC_STATE_RXPAYLOAD,  // payload
    } state;
    unsigned int num_symbols_received;

    // debugging macros
#if DEBUG_GMSKFRAMESYNC
    windowf  debug_agc_rssi;            // rssi buffer
    windowcf debug_x;                   // received samples buffer
    windowf  debug_framesyms;           // GMSK output symbols
    windowf  debug_bxy;                 // cross-correlator output
#endif
};

// create gmskframesync object
//  _props          :   properties structure pointer (default if NULL)
//  _callback       :   callback function invoked when frame is received
//  _userdata       :   user-defined data object passed to callback
gmskframesync gmskframesync_create(unsigned int _k,
                                   unsigned int _m,
                                   float _BT,
                                   gmskframesync_callback _callback,
                                   void * _userdata)
{
    // TODO : validate input

    gmskframesync q = (gmskframesync) malloc(sizeof(struct gmskframesync_s));
    q->k  = _k;
    q->m  = _m;
    q->BT = _BT;
    q->callback = _callback;
    q->userdata = _userdata;

    // create synchronizer objects
    q->npfb = 32;
    q->symsync = symsync_rrrf_create_rnyquist(LIQUID_RNYQUIST_GMSKRX,
                                              q->k,
                                              q->m,
                                              q->BT,
                                              q->npfb);
    symsync_rrrf_set_output_rate(q->symsync, 1);

    // create/allocate preamble objects/arrays
    msequence ms = msequence_create_default(6);
    q->bx = bsequence_create(64);
    q->by = bsequence_create(64);
    unsigned int i;
    for (i=0; i<64; i++) {
        unsigned int bit = msequence_advance(ms);
        bsequence_push(q->bx, bit);
    }
    bsequence_clear(q->by);
    msequence_destroy(ms);

    // create/allocate header objects/arrays
    q->header_user= (unsigned char*)malloc(GMSKFRAME_H_USER*sizeof(unsigned char));
    q->header_enc = (unsigned char*)malloc(GMSKFRAME_H_ENC*sizeof(unsigned char));
    q->header_dec = (unsigned char*)malloc(GMSKFRAME_H_DEC*sizeof(unsigned char));
    q->p_header   = packetizer_create(GMSKFRAME_H_DEC,
                                      GMSKFRAME_H_CRC,
                                      GMSKFRAME_H_FEC,
                                      LIQUID_FEC_NONE);

    // create/allocate payload objects/arrays
    q->dec_msg_len  = 0;
    q->check        = LIQUID_CRC_32;
    q->fec0         = LIQUID_FEC_NONE;
    q->fec1         = LIQUID_FEC_NONE;
    q->p_payload = packetizer_create(q->dec_msg_len,
                                     q->check,
                                     q->fec0,
                                     q->fec1);
    q->enc_msg_len = packetizer_get_enc_msg_len(q->p_payload);
    q->payload_enc = (unsigned char*) malloc(q->enc_msg_len*sizeof(unsigned char));
    q->payload_dec = (unsigned char*) malloc(q->dec_msg_len*sizeof(unsigned char));
    //q->payload_len = 8*q->enc_msg_len;

    
    // initialize...
    q->rssi_hat = 1.0f;

#if DEBUG_GMSKFRAMESYNC
    // debugging
    q->debug_agc_rssi   = windowf_create(DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    q->debug_x          = windowcf_create(DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    q->debug_framesyms  = windowf_create(DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    q->debug_bxy = windowf_create(DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
#endif

    // reset synchronizer
    gmskframesync_reset(q);

    // return synchronizer object
    return q;
}


// destroy frame synchronizer object, freeing all internal memory
void gmskframesync_destroy(gmskframesync _q)
{
#if DEBUG_GMSKFRAMESYNC
    // output debugging file
    gmskframesync_output_debug_file(_q, DEBUG_GMSKFRAMESYNC_FILENAME);

    // destroy debugging windows
    windowf_destroy(_q->debug_agc_rssi);
    windowcf_destroy(_q->debug_x);
    windowf_destroy(_q->debug_framesyms);
    windowf_destroy(_q->debug_bxy);
#endif

    // destroy synchronizer objects
    symsync_rrrf_destroy(_q->symsync);  // symbol synchronizer, matched filter

    // destroy/free preamble objects/arrays
    bsequence_destroy(_q->bx);
    bsequence_destroy(_q->by);
    
    // destroy/free header objects/arrays
    free(_q->header_user);
    free(_q->header_enc);
    free(_q->header_dec);
    packetizer_destroy(_q->p_header);

    // destroy/free payload objects/arrays
    free(_q->payload_enc);
    free(_q->payload_dec);
    packetizer_destroy(_q->p_payload);

    // free main object memory
    free(_q);
}

// print frame synchronizer object internals
void gmskframesync_print(gmskframesync _q)
{
    printf("gmskframesync:\n");
}

// reset frame synchronizer object
void gmskframesync_reset(gmskframesync _q)
{
    // reset sample state
    _q->x_prime = 0.0f;
    _q->evm_hat = 0.0f;

    // reset synchronizer objects
    symsync_rrrf_clear(_q->symsync);
    symsync_rrrf_set_lf_bw(_q->symsync, 0.1f);
    bsequence_clear(_q->by);

    // reset state
    _q->state = GMSKFRAMESYNC_STATE_SEEKPN;
    _q->num_symbols_received = 0;
}

// execute frame synchronizer
//  _q      :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void gmskframesync_execute(gmskframesync _q,
                           float complex * _x,
                           unsigned int _n)
{
    // synchronized sample buffer
    float buffer[4];
    unsigned int num_written=0;

    // push through synchronizer
    unsigned int i;
    for (i=0; i<_n; i++) {
        // compute differential
        float complex s = conjf(_q->x_prime)*_x[i];
        _q->x_prime = _x[i];

        // update rssi estimate
        float alpha = 0.2f;
        _q->rssi_hat = (1.0f-alpha)*_q->rssi_hat + alpha*cabsf(s);
#if DEBUG_GMSKFRAMESYNC
        windowf_push(_q->debug_agc_rssi, 10*log10f(_q->rssi_hat));
        windowcf_push(_q->debug_x, _x[i]);
#endif
        // compute phase difference
        float phi = cargf(s) * _q->k;

        // push through matched filter/symbol timing recovery
        symsync_rrrf_execute(_q->symsync, &phi, 1, buffer, &num_written);

        // demodulate
        unsigned int j;
        for (j=0; j<num_written; j++) {
#if DEBUG_GMSKFRAMESYNC
            switch (_q->state) {
            case GMSKFRAMESYNC_STATE_SEEKPN:
                break;
            case GMSKFRAMESYNC_STATE_RXHEADER:
            case GMSKFRAMESYNC_STATE_RXPAYLOAD:
                windowf_push(_q->debug_framesyms, buffer[j]);
                break;
            }
#endif
            switch (_q->state) {
            case GMSKFRAMESYNC_STATE_SEEKPN:
                // look for p/n sequence
                gmskframesync_execute_seekpn(_q, buffer[j]);
                break;

            case GMSKFRAMESYNC_STATE_RXHEADER:
                // receive header
                gmskframesync_execute_rxheader(_q, buffer[j]);
                break;

            case GMSKFRAMESYNC_STATE_RXPAYLOAD:
                // receive payload
                gmskframesync_execute_rxpayload(_q, buffer[j]);
                break;
            }
        }
    }
}

// 
// internal methods
//

void gmskframesync_execute_seekpn(gmskframesync _q,
                                  float _x)
{
    // push through binary sequence correlator
    bsequence_push(_q->by, _x > 0.0f ? 1 : 0);
    float bxy_out = (float)(bsequence_correlate(_q->bx, _q->by) - 32) / 32.0f;
#if DEBUG_GMSKFRAMESYNC
    windowf_push(_q->debug_bxy, bxy_out);
#endif

    // check threshold...
    if (bxy_out > 0.60f) {
#if DEBUG_GMSKFRAMESYNC_PRINT
        printf("***** gmskframesync: frame detected! *****\n");
#endif
        _q->state = GMSKFRAMESYNC_STATE_RXHEADER;
        symsync_rrrf_set_lf_bw(_q->symsync, 0.005f);
        _q->num_symbols_received = 0;
        memset(_q->header_enc, 0x00, GMSKFRAME_H_ENC);
    }
}

void gmskframesync_execute_rxheader(gmskframesync _q,
                                    float _x)
{
    // demodulate
    unsigned char s = _x > 0.0f ? 1 : 0;

    // update evm
    float evm = _x - (s ? 1.0f : -1.0f);
    _q->evm_hat += evm*evm;

    // save bit in buffer
    div_t d = div(_q->num_symbols_received, 8);
    _q->header_enc[d.quot] |= s << (8-d.rem-1);

    // increment symbol counter
    _q->num_symbols_received++;
    if (_q->num_symbols_received == 8*GMSKFRAME_H_ENC) {
#if DEBUG_GMSKFRAMESYNC_PRINT
        printf("***** gmskframesync: header received\n");
        printf("    header_enc      :");
        unsigned int i;
        for (i=0; i<GMSKFRAME_H_ENC; i++)
            printf(" %.2X", _q->header_enc[i]);
        printf("\n");
#endif

        // decode header
        gmskframesync_decode_header(_q);

        // clear encoded payload array
        memset(_q->payload_enc, 0x00, _q->enc_msg_len);

        // invoke callback if header is invalid
        if (_q->header_valid) {
            _q->state = GMSKFRAMESYNC_STATE_RXPAYLOAD;
            _q->num_symbols_received = 0;
        } else {
            //printf("**** header invalid!\n");
            // set framestats internals
            _q->framestats.rssi             = 10*log10f(_q->rssi_hat);
            _q->framestats.evm              = 10*log10f(_q->evm_hat / (8*GMSKFRAME_H_ENC) );
            _q->framestats.framesyms        = NULL;
            _q->framestats.num_framesyms    = 0;
            _q->framestats.mod_scheme       = LIQUID_MODEM_UNKNOWN;
            _q->framestats.mod_bps          = 1;
            _q->framestats.check            = LIQUID_CRC_UNKNOWN;
            _q->framestats.fec0             = LIQUID_FEC_UNKNOWN;
            _q->framestats.fec1             = LIQUID_FEC_UNKNOWN;

            // invoke callback method
            _q->callback(_q->header_user, _q->header_valid, NULL, 0, 0, _q->framestats, _q->userdata);

            gmskframesync_reset(_q);
        }
    }
}

void gmskframesync_execute_rxpayload(gmskframesync _q,
                                     float _x)
{
    // demodulate
    unsigned char s = _x > 0.0f ? 1 : 0;

    // TODO : update evm

    // save bit in buffer
    div_t d = div(_q->num_symbols_received, 8);
    _q->payload_enc[d.quot] |= s << (8-d.rem-1);

    // increment symbol counter
    _q->num_symbols_received++;
    if (_q->num_symbols_received == 8*_q->enc_msg_len) {
#if DEBUG_GMSKFRAMESYNC_PRINT
        printf("***** gmskframesync: payload received\n");
#endif

        // decode payload
        _q->payload_valid = packetizer_decode(_q->p_payload, _q->payload_enc, _q->payload_dec);

        // invoke callback
        // set framestats internals
        _q->framestats.rssi             = 10*log10f(_q->rssi_hat);
        _q->framestats.evm              = 10*log10f(_q->evm_hat / (8*GMSKFRAME_H_ENC) );
        _q->framestats.framesyms        = NULL;
        _q->framestats.num_framesyms    = 0;
        _q->framestats.mod_scheme       = LIQUID_MODEM_UNKNOWN;
        _q->framestats.mod_bps          = 1;
        _q->framestats.check            = _q->check;
        _q->framestats.fec0             = _q->fec0;
        _q->framestats.fec1             = _q->fec1;

        // invoke callback method
        _q->callback(_q->header_user,
                     _q->header_valid,
                     _q->payload_dec,
                     _q->dec_msg_len,
                     _q->payload_valid,
                     _q->framestats,
                     _q->userdata);

        // reset frame synchronizer
        gmskframesync_reset(_q);
    }
}

// decode header and re-configure payload decoder
void gmskframesync_decode_header(gmskframesync _q)
{
    // unscramble data
    unscramble_data(_q->header_enc, GMSKFRAME_H_ENC);

    // run packet decoder
    _q->header_valid = packetizer_decode(_q->p_header, _q->header_enc, _q->header_dec);

#if DEBUG_GMSKFRAMESYNC_PRINT
    printf("****** header extracted [%s]\n", _q->header_valid ? "valid" : "INVALID!");
#endif

    // copy user-defined header
    memmove(_q->header_user, _q->header_dec, GMSKFRAME_H_USER);
    
    if (!_q->header_valid)
        return;

    unsigned int n = GMSKFRAME_H_USER;

    // first byte is for expansion/version validation
    if (_q->header_dec[n+0] != GMSKFRAME_VERSION) {
        fprintf(stderr,"warning: gmskframesync_decode_header(), invalid framing version\n");
        _q->header_valid = 0;
    }

    // strip off payload length
    unsigned int dec_msg_len = (_q->header_dec[n+1] << 8) | (_q->header_dec[n+2]);

    // strip off CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [n+3]
    //  fec0    : least-significant 5 bits of [n+3]
    //  fec1    : least-significant 5 bits of [n+4]
    unsigned int check = (_q->header_dec[n+3] >> 5 ) & 0x07;
    unsigned int fec0  = (_q->header_dec[n+3]      ) & 0x1f;
    unsigned int fec1  = (_q->header_dec[n+4]      ) & 0x1f;

    // validate properties
    if (check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr,"warning: gmskframesync_decode_header(), decoded CRC exceeds available\n");
        check = LIQUID_CRC_UNKNOWN;
        _q->header_valid = 0;
    }
    if (fec0 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,"warning: gmskframesync_decode_header(), decoded FEC (inner) exceeds available\n");
        fec0 = LIQUID_FEC_UNKNOWN;
        _q->header_valid = 0;
    }
    if (fec1 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,"warning: gmskframesync_decode_header(), decoded FEC (outer) exceeds available\n");
        fec1 = LIQUID_FEC_UNKNOWN;
        _q->header_valid = 0;
    }

    // print results
#if DEBUG_GMSKFRAMESYNC_PRINT
    printf("    properties:\n");
    printf("      * fec (inner)     :   %s\n", fec_scheme_str[fec0][1]);
    printf("      * fec (outer)     :   %s\n", fec_scheme_str[fec1][1]);
    printf("      * CRC scheme      :   %s\n", crc_scheme_str[check][1]);
    printf("      * payload length  :   %u bytes\n", dec_msg_len);
#endif

    // configure payload receiver
    if (_q->header_valid) {
        // set new packetizer properties
        _q->dec_msg_len = dec_msg_len;
        _q->check       = check;
        _q->fec0        = fec0;
        _q->fec1        = fec1;
        
        // recreate packetizer object
        _q->p_payload = packetizer_recreate(_q->p_payload,
                                            _q->dec_msg_len,
                                            _q->check,
                                            _q->fec0,
                                            _q->fec1);

        // re-compute payload encoded message length
        _q->enc_msg_len = packetizer_get_enc_msg_len(_q->p_payload);
#if DEBUG_GMSKFRAMESYNC_PRINT
        printf("      * payload encoded :   %u bytes\n", _q->enc_msg_len);
#endif

        // re-allocate buffers accordingly
        _q->payload_enc = (unsigned char*) realloc(_q->payload_enc, _q->enc_msg_len*sizeof(unsigned char));
        _q->payload_dec = (unsigned char*) realloc(_q->payload_dec, _q->dec_msg_len*sizeof(unsigned char));
    }
    //
}

void gmskframesync_output_debug_file(gmskframesync _q,
                                     const char * _filename)
{
    FILE* fid = fopen(_filename,"w");
    if (!fid) {
        fprintf(stderr, "error: flexframesync_output_debug_file(), could not open '%s' for writing\n", _filename);
        return;
    }
    fprintf(fid,"%% %s: auto-generated file", _filename);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

#if DEBUG_GMSKFRAMESYNC
    fprintf(fid,"num_samples = %u;\n", DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    fprintf(fid,"t = 0:(num_samples-1);\n");
    unsigned int i;
    float * r;
    float complex * rc;

    // write agc_rssi
    fprintf(fid,"agc_rssi = zeros(1,num_samples);\n");
    windowf_read(_q->debug_agc_rssi, &r);
    for (i=0; i<DEBUG_GMSKFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t, agc_rssi)\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");
    fprintf(fid,"\n\n");

    // write x
    fprintf(fid,"x = zeros(1,num_samples);\n");
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_GMSKFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");
    fprintf(fid,"\n\n");

    // write framesyms
    fprintf(fid,"framesyms = zeros(1,num_samples);\n");
    windowf_read(_q->debug_framesyms, &r);
    for (i=0; i<DEBUG_GMSKFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,framesyms,'x')\n");
    fprintf(fid,"xlabel('time (symbol index)');\n");
    fprintf(fid,"ylabel('GMSK demodulator output');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"\n\n");

    // write bxy
    fprintf(fid,"bxy = zeros(1,num_samples);\n");
    windowf_read(_q->debug_bxy, &r);
    for (i=0; i<DEBUG_GMSKFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"bxy(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,bxy);\n");
    fprintf(fid,"xlabel('time (symbol index)');\n");
    fprintf(fid,"ylabel('binary cross-correlator output');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"\n\n");
#endif

    fclose(fid);

    printf("gmskframesync/debug: results written to '%s'\n", _filename);

}
