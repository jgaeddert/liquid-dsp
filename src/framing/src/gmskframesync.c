/*
 * Copyright (c) 2011, 2013 Joseph Gaeddert
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
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_GMSKFRAMESYNC             1
#define DEBUG_GMSKFRAMESYNC_PRINT       0
#define DEBUG_GMSKFRAMESYNC_FILENAME    "gmskframesync_debug.m"
#define DEBUG_GMSKFRAMESYNC_BUFFER_LEN  (2000)

// enable pre-demodulation filter (remove out-of-band noise)
#define GMSKFRAMESYNC_PREFILTER         0

// push buffered p/n sequence through synchronizer
void gmskframesync_pushpn(gmskframesync _q);

// update instantaneous frequency estimate
void gmskframesync_update_fi(gmskframesync _q,
                             float complex _x);

// update symbol synchronizer internal state (filtered error, index, etc.)
//  _q      :   frame synchronizer
//  _x      :   input sample
//  _y      :   output symbol
int gmskframesync_update_symsync(gmskframesync _q,
                                 float         _x,
                                 float *       _y);

// execute stages
void gmskframesync_execute_detectframe(gmskframesync _q, float complex _x);
void gmskframesync_execute_rxpreamble( gmskframesync _q, float complex _x);
void gmskframesync_execute_rxheader(   gmskframesync _q, float complex _x);
void gmskframesync_execute_rxpayload(  gmskframesync _q, float complex _x);

// decode header
void gmskframesync_decode_header(gmskframesync _q);

// decode payload
void gmskframesync_decode_payload(gmskframesync _q);

// gmskframesync object structure
struct gmskframesync_s {
    unsigned int k;                 // filter samples/symbol
    unsigned int m;                 // filter semi-length (symbols)
    float BT;                       // filter bandwidth-time product
    framesync_callback callback;    // user-defined callback function
    void * userdata;                // user-defined data structure
    framesyncstats_s framestats;    // frame statistic object
    
    //
    float complex x_prime;          // received sample state
    float fi_hat;                   // instantaneous frequency estimate
    
    // timing recovery objects, states
    firpfb_rrrf mf;                 // matched filter decimator
    firpfb_rrrf dmf;                // derivative matched filter decimator
    unsigned int npfb;              // number of filters in symsync
    float pfb_q;                    // filtered timing error
    float pfb_soft;                 // soft filterbank index
    int pfb_index;                  // hard filterbank index
    int pfb_timer;                  // filterbank output flag
    float symsync_out;              // symbol synchronizer output

    // synchronizer objects
    detector_cccf frame_detector;   // pre-demod detector
    float tau_hat;                  // fractional timing offset estimate
    float dphi_hat;                 // carrier frequency offset estimate
    float gamma_hat;                // channel gain estimate
    windowcf buffer;                // pre-demod buffered samples, size: k*(pn_len+m)
    nco_crcf nco_coarse;            // coarse carrier frequency recovery
    
    // preamble
    unsigned int preamble_len;      // number of symbols in preamble
    float * preamble_pn;            // preamble p/n sequence

    // status variables
    enum {
        STATE_DETECTFRAME=0,        // detect frame (seek p/n sequence)
        STATE_RXPREAMBLE,           // receive p/n sequence
        STATE_RXHEADER,             // receive header data
        STATE_RXPAYLOAD,            // receive payload data
    } state;
    unsigned int pn_counter;        // counter: num of p/n syms received
    unsigned int header_counter;    // counter: num of header syms received
    unsigned int payload_counter;   // counter: num of payload syms received
    // debugging structures
#if DEBUG_GMSKFRAMESYNC
    int debug_enabled;              // debugging enabled?
    int debug_objects_created;      // debugging objects created?
    windowcf debug_x;               // received samples buffer
    windowf  debug_framesyms;       // GMSK output symbols
#endif
};

// create GMSK frame synchronizer
//  _k          :   samples/symbol
//  _m          :   filter delay (symbols)
//  _BT         :   bandwidth-time factor
//  _callback   :   callback function
//  _userdata   :   user data pointer passed to callback function
gmskframesync gmskframesync_create(unsigned int       _k,
                                   unsigned int       _m,
                                   float              _BT,
                                   framesync_callback _callback,
                                   void *             _userdata)
{
    // TODO : validate input

    gmskframesync q = (gmskframesync) malloc(sizeof(struct gmskframesync_s));
    q->k        = 2;        // 
    q->m        = 3;        // 
    q->BT       = 0.35f;    // 
    q->callback = _callback;
    q->userdata = _userdata;

#if GMSKFRAMESYNC_PREFILTER
    q->prefilter = iirfilt_crcf_create_prototype(LIQUID_IIRDES_BUTTER,
                                                 LIQUID_IIRDES_LOWPASS,
                                                 LIQUID_IIRDES_SOS,
                                                 3,
                                                 0.5f*(1 + q->BT) / (float)(q->k),
                                                 0.0f,
                                                 1.0f,
                                                 60.0f);
#endif

    unsigned int i;

    // frame detector
    q->preamble_len = 63;
    q->preamble_pn = (float*)malloc(q->preamble_len*sizeof(float));
    float complex preamble_samples[q->preamble_len*q->k];
    msequence ms = msequence_create(6, 0x6d, 1);
    gmskmod mod = gmskmod_create(q->k, q->m, q->BT);

    for (i=0; i<q->preamble_len + q->m; i++) {
        unsigned char bit = msequence_advance(ms);

        // save p/n sequence
        if (i < q->preamble_len)
            q->preamble_pn[i] = bit ? 1.0f : -1.0f;
        
        // modulate/interpolate
        if (i < q->m) gmskmod_modulate(mod, bit, &preamble_samples[0]);
        else          gmskmod_modulate(mod, bit, &preamble_samples[(i-q->m)*q->k]);
    }

    gmskmod_destroy(mod);
    msequence_destroy(ms);

#if 0
    // print sequence
    for (i=0; i<q->preamble_len*q->k; i++)
        printf("preamble(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(preamble_samples[i]), cimagf(preamble_samples[i]));
#endif
    // create frame detector
    float threshold = 0.5f;     // detection threshold
    float dphi_max  = 0.05f;    // maximum carrier offset allowable
    q->frame_detector = detector_cccf_create(preamble_samples, q->preamble_len*q->k, threshold, dphi_max);
    q->buffer = windowcf_create(q->k*(q->preamble_len+q->m));

    // create symbol timing recovery filters
    q->npfb = 32;   // number of filters in the bank
    q->mf   = firpfb_rrrf_create_rnyquist( LIQUID_RNYQUIST_GMSKRX,q->npfb,q->k,q->m,q->BT);
    q->dmf  = firpfb_rrrf_create_drnyquist(LIQUID_RNYQUIST_GMSKRX,q->npfb,q->k,q->m,q->BT);

    // create down-coverters for carrier phase tracking
    q->nco_coarse = nco_crcf_create(LIQUID_NCO);
#if 0
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
#endif

#if DEBUG_GMSKFRAMESYNC
    // debugging structures
    q->debug_enabled         = 0;
    q->debug_objects_created = 0;
    q->debug_x               = NULL;
    q->debug_framesyms       = NULL;
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
    // destroy debugging objects
    if (_q->debug_objects_created) {
        windowcf_destroy(_q->debug_x);
        windowf_destroy( _q->debug_framesyms);
    }
#endif

    // destroy synchronizer objects
#if GMSKFRAMESYNC_PREFILTER
    iirfilt_crcf_destroy(_q->prefilter);// pre-demodulator filter
#endif
    // preamble
    detector_cccf_destroy(_q->frame_detector);
    windowcf_destroy(_q->buffer);
    free(_q->preamble_pn);
    
    //
    firpfb_rrrf_destroy(_q->mf);                // matched filter
    firpfb_rrrf_destroy(_q->dmf);               // derivative matched filter
    nco_crcf_destroy(_q->nco_coarse);           // coarse NCO

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
    // reset state
    _q->state = STATE_DETECTFRAME;
    
    // clear pre-demod buffer
    windowcf_clear(_q->buffer);

    // reset internal objects
    detector_cccf_reset(_q->frame_detector);
    
    // reset carrier recovery objects
    nco_crcf_reset(_q->nco_coarse);

    // reset sample state
    _q->x_prime = 0.0f;
    _q->fi_hat  = 0.0f;
    
    // reset symbol timing recovery state
    firpfb_rrrf_clear(_q->mf);
    firpfb_rrrf_clear(_q->dmf);
    _q->pfb_q = 0.0f;   // filtered error signal
        
}

// execute frame synchronizer
//  _q      :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void gmskframesync_execute(gmskframesync   _q,
                           float complex * _x,
                           unsigned int    _n)
{
    // push through synchronizer
    unsigned int i;
    for (i=0; i<_n; i++) {
        float complex xf;   // input sample
#if GMSKFRAMESYNC_PREFILTER
        iirfilt_crcf_execute(_q->prefilter, _x[i], &xf);
#else
        xf = _x[i];
#endif

#if DEBUG_GMSKFRAMESYNC
        if (_q->debug_enabled)
            windowcf_push(_q->debug_x, xf);
#endif

        switch (_q->state) {
        case STATE_DETECTFRAME:
            // look for p/n sequence
            gmskframesync_execute_detectframe(_q, xf);
            break;

        case STATE_RXPREAMBLE:
            // receive p/n sequence symbols
            gmskframesync_execute_rxpreamble(_q, xf);
            break;

        case STATE_RXHEADER:
            // receive header
            gmskframesync_execute_rxheader(_q, xf);
            break;

        case STATE_RXPAYLOAD:
            // receive payload
            gmskframesync_execute_rxpayload(_q, xf);
            break;
        }
    }
}

// 
// internal methods
//

// update symbol synchronizer internal state (filtered error, index, etc.)
//  _q      :   frame synchronizer
//  _x      :   input sample
//  _y      :   output symbol
int gmskframesync_update_symsync(gmskframesync _q,
                                 float         _x,
                                 float *       _y)
{
    // push sample into filterbanks
    firpfb_rrrf_push(_q->mf,  _x);
    firpfb_rrrf_push(_q->dmf, _x);

    //
    float mf_out  = 0.0f;    // matched-filter output
    float dmf_out = 0.0f;    // derivatived matched-filter output

    int sample_available = 0;

    // compute output if timeout
    if (_q->pfb_timer <= 0) {
        sample_available = 1;

        // reset timer
        _q->pfb_timer = 2;  // k samples/symbol

        firpfb_rrrf_execute(_q->mf,  _q->pfb_index, &mf_out);
        firpfb_rrrf_execute(_q->dmf, _q->pfb_index, &dmf_out);

        // update filtered timing error
        // lo  bandwidth parameters: {0.92, 1.20}, about 100 symbols settling time
        // med bandwidth parameters: {0.98, 0.20}, about 200 symbols settling time
        // hi  bandwidth parameters: {0.99, 0.05}, about 500 symbols settling time
        _q->pfb_q = 0.99f*_q->pfb_q + 0.05f*crealf( conjf(mf_out)*dmf_out );

        // accumulate error into soft filterbank value
        _q->pfb_soft += _q->pfb_q;

        // compute actual filterbank index
        _q->pfb_index = roundf(_q->pfb_soft);

        // contrain index to be in [0, npfb-1]
        while (_q->pfb_index < 0) {
            _q->pfb_index += _q->npfb;
            _q->pfb_soft  += _q->npfb;

            // adjust pfb output timer
            _q->pfb_timer--;
        }
        while (_q->pfb_index > _q->npfb-1) {
            _q->pfb_index -= _q->npfb;
            _q->pfb_soft  -= _q->npfb;

            // adjust pfb output timer
            _q->pfb_timer++;
        }
    }

    // decrement symbol timer
    _q->pfb_timer--;

    // set output and return
    *_y = mf_out;
    
    return sample_available;
}

// push buffered p/n sequence through synchronizer
void gmskframesync_pushpn(gmskframesync _q)
{
    unsigned int i;

    // reset filterbanks
    firpfb_rrrf_clear(_q->mf);
    firpfb_rrrf_clear(_q->dmf);

    // read buffer
    float complex * rc;
    windowcf_read(_q->buffer, &rc);

    // compute delay and filterbank index
    //  tau_hat < 0 :   delay = 2*k*m-1, index = round(   tau_hat *npfb), flag = 0
    //  tau_hat > 0 :   delay = 2*k*m-2, index = round((1-tau_hat)*npfb), flag = 0
    assert(_q->tau_hat < 0.5f && _q->tau_hat > -0.5f);
    unsigned int delay = 2*_q->k*_q->m - 1; // samples to buffer before computing output
    _q->pfb_soft       = -_q->tau_hat*_q->npfb;
    _q->pfb_index      = (int) roundf(_q->pfb_soft);
    while (_q->pfb_index < 0) {
        delay         -= 1;
        _q->pfb_index += _q->npfb;
        _q->pfb_soft  += _q->npfb;
    }
    _q->pfb_timer = 0;

    // set coarse carrier frequency offset
    nco_crcf_set_frequency(_q->nco_coarse, _q->dphi_hat);
    
    unsigned int buffer_len = (_q->preamble_len + _q->m) * _q->k;
    for (i=0; i<buffer_len; i++) {
        if (i < delay) {
            float complex y;
            nco_crcf_mix_down(_q->nco_coarse, rc[i]/(_q->gamma_hat*_q->k), &y);
            nco_crcf_step(_q->nco_coarse);

            // update instantanenous frequency estimate
            gmskframesync_update_fi(_q, y);

            // push initial samples into filterbanks
            firpfb_rrrf_push(_q->mf,  _q->fi_hat);
            firpfb_rrrf_push(_q->dmf, _q->fi_hat);
        } else {
            // run remaining samples through p/n sequence recovery
            gmskframesync_execute_rxpreamble(_q, rc[i]);
        }
    }
}

// update instantaneous frequency estimate
void gmskframesync_update_fi(gmskframesync _q,
                             float complex _x)
{
    // compute differential phase
    _q->fi_hat = cargf(conjf(_q->x_prime)*_x) * _q->k;

    // update internal state
    _q->x_prime = _x;
}

void gmskframesync_execute_detectframe(gmskframesync _q,
                                       float complex _x)
{
    // push sample into pre-demod p/n sequence buffer
    windowcf_push(_q->buffer, _x);

    // push through pre-demod synchronizer
    int detected = detector_cccf_correlate(_q->frame_detector,
                                           _x,
                                           &_q->tau_hat,
                                           &_q->dphi_hat,
                                           &_q->gamma_hat);

    // check if frame has been detected
    if (detected) {
        printf("***** frame detected! tau-hat:%8.4f, dphi-hat:%8.4f, gamma:%8.2f dB\n",
                _q->tau_hat, _q->dphi_hat, 20*log10f(_q->gamma_hat));

#if 0
        // push buffered samples through synchronizer
        gmskframesync_pushpn(_q);

        // update state
        _q->state = STATE_RXPREAMBLE;
#else
        // set state (still need a few more samples before entire p/n
        // sequence has been received)
        _q->state = STATE_RXPREAMBLE;
#endif
    }
}

void gmskframesync_execute_rxpreamble(gmskframesync _q,
                                      float complex _x)
{
}

void gmskframesync_execute_rxheader(gmskframesync _q,
                                    float complex _x)
{
#if 0
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
            _q->state = STATE_RXPAYLOAD;
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
#endif
}

void gmskframesync_execute_rxpayload(gmskframesync _q,
                                     float complex _x)
{
#if 0
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
#endif
}

// decode header and re-configure payload decoder
void gmskframesync_decode_header(gmskframesync _q)
{
#if 0
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
#endif
}


// decode payload and set internal framestats object
void gmskframesync_decode_payload(gmskframesync _q)
{
}

void gmskframesync_debug_enable(gmskframesync _q)
{
    // create debugging objects if necessary
#if DEBUG_GMSKFRAMESYNC
    if (_q->debug_x == NULL)
        _q->debug_x = windowcf_create(DEBUG_GMSKFRAMESYNC_BUFFER_LEN);

    if (_q->debug_framesyms == NULL)
        _q->debug_framesyms  = windowf_create(DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    
    // set debugging flags
    _q->debug_enabled = 1;
    _q->debug_objects_created = 1;
#else
    fprintf(stderr,"gmskframesync_debug_enable(): compile-time debugging disabled\n");
#endif
}

void gmskframesync_debug_disable(gmskframesync _q)
{
#if DEBUG_GMSKFRAMESYNC
    _q->debug_enabled = 0;
#else
    fprintf(stderr,"gmskframesync_debug_disable(): compile-time debugging disabled\n");
#endif
}

void gmskframesync_debug_print(gmskframesync _q,
                               const char *  _filename)
{
#if DEBUG_GMSKFRAMESYNC
    if (!_q->debug_objects_created) {
        fprintf(stderr,"error: gmskframe_debug_print(), debugging objects don't exist; enable debugging first\n");
        return;
    }

    FILE* fid = fopen(_filename,"w");
    if (!fid) {
        fprintf(stderr, "error: gmskframesync_debug_print(), could not open '%s' for writing\n", _filename);
        return;
    }
    fprintf(fid,"%% %s: auto-generated file", _filename);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    fprintf(fid,"num_samples = %u;\n", DEBUG_GMSKFRAMESYNC_BUFFER_LEN);
    fprintf(fid,"t = 0:(num_samples-1);\n");
    unsigned int i;
    float complex * rc;

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

#if 0
    float * r;
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
#endif

    fclose(fid);

    printf("gmskframesync/debug: results written to '%s'\n", _filename);
#else
    fprintf(stderr,"gmskframesync_debug_print(): compile-time debugging disabled\n");
#endif

}
