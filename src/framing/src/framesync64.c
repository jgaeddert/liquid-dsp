/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012, 2013 Joseph Gaeddert
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
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_FRAMESYNC64           1
#define DEBUG_FRAMESYNC64_PRINT     0
#define DEBUG_FILENAME              "framesync64_internal_debug.m"
#define DEBUG_BUFFER_LEN            (1600)

// push samples through detection stage
void framesync64_execute_seekpn(framesync64   _q,
                                float complex _x);

// update symbol synchronizer internal state (filtered error, index, etc.)
//  _q      :   frame synchronizer
//  _mf     :   matched-filter output
//  _dmf    :   derivative matched-filter output
void framesync64_update_symsync(framesync64   _q,
                                float complex _mf,
                                float complex _dmf);

// push buffered p/n sequence through synchronizer
void framesync64_pushpn(framesync64 _q);

// push samples through synchronizer, saving received p/n symbols
void framesync64_execute_rxpn(framesync64   _q,
                              float complex _x);

// once p/n symbols are buffered, estimate residual carrier
// frequency and phase offsets, push through fine-tuned NCO
void framesync64_syncpn(framesync64 _q);

// receive payload symbols
void framesync64_execute_rxpayload(framesync64   _q,
                                   float complex _x);

// decode payload
void framesync64_decode_payload(framesync64 _q);

// advanced mode
void framesync64_csma_lock(framesync64 _q);
void framesync64_csma_unlock(framesync64 _q);

// framesync64 object structure
struct framesync64_s {
    // callback
    framesync_callback callback;    // user-defined callback function
    void * userdata;                // user-defined data structure
    framesyncstats_s framestats;    // frame statistic object
    
    float         pn_sequence[64];  // known 64-symbol p/n sequence
    float complex pn_syms[64];      // received p/n symbols
    float complex payload_syms[600];// payload symbols

    // synchronizer objects
    detector_cccf frame_detector;   // pre-demod detector
    windowcf buffer;                // pre-demod buffered samples, size: k*(pn_len+m)
    nco_crcf nco_coarse;            // coarse carrier frequency recovery
    nco_crcf nco_fine;              // fine carrier recovery (after demod)

    // timing recovery objects, states
    firpfb_crcf mf;                 // matched filter decimator
    firpfb_crcf dmf;                // derivative matched filter decimator
    unsigned int npfb;              // number of filters in symsync
    float pfb_q;                    // 
    float pfb_soft;                 // soft filterbank index
    int pfb_index;                  // hard filterbank index
    int pfb_timer;                  // filterbank output flag

    // QPSK payload demodulator
    modem demod;

    // payload decoder
    packetizer p_payload;           // payload packetizer
    unsigned char payload_enc[150]; // encoded payload bytes
    unsigned char payload_dec[72];  // decoded pyaload bytes
    int crc_pass;                   // flag to determine if payload passed
    
    // status variables
    enum {
        STATE_DETECTFRAME=0,        // detect frame (seek p/n sequence)
        STATE_RXPN,                 // receive p/n sequence
        STATE_RXPAYLOAD,            // receive payload data
    } state;
    float tau_hat;                  // fractional timing offset estimate
    float dphi_hat;                 // carrier frequency offset estimate
    float gamma_hat;                // channel gain estimate
    unsigned int pn_counter;        // counter: num of p/n syms received
    unsigned int payload_counter;   // counter: num of payload syms received

#if DEBUG_FRAMESYNC64
    int debug_enabled;              // debugging enabled?
    int debug_objects_created;      // debugging objects created?
    windowcf debug_x;               // debug: raw input samples
    float debug_symsync_index[664]; // symbol synchronizer phase, 664 = 64 + 600
    float debug_nco_phase[664];     // fine-tuned nco phase, 664 = 64 + 600
#endif
};

// create framesync64 object
//  _callback       :   callback function invoked when frame is received
//  _userdata       :   user-defined data object passed to callback
framesync64 framesync64_create(framesync_callback _callback,
                               void *             _userdata)
{
    framesync64 q = (framesync64) malloc(sizeof(struct framesync64_s));
    q->callback = _callback;
    q->userdata = _userdata;

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
    float threshold = 0.4f;     // detection threshold
    float dphi_max  = 0.05f;    // maximum carrier offset allowable
    q->frame_detector = detector_cccf_create(seq, k*64, threshold, dphi_max);
    q->buffer = windowcf_create(2*(64+3));

    // create symbol timing recovery filters
    q->npfb = 32;   // number of filters in the bank
    q->mf   = firpfb_crcf_create_rnyquist(LIQUID_RNYQUIST_ARKAISER, q->npfb,k,m,beta);
    q->dmf  = firpfb_crcf_create_drnyquist(LIQUID_RNYQUIST_ARKAISER,q->npfb,k,m,beta);

    // create down-coverters for carrier phase tracking
    q->nco_coarse = nco_crcf_create(LIQUID_NCO);
    q->nco_fine   = nco_crcf_create(LIQUID_VCO);
    nco_crcf_pll_set_bandwidth(q->nco_fine, 0.02f);
    
    // create payload demodulator
    q->demod = modem_create(LIQUID_MODEM_QPSK);

    // create payload packet decoder
    unsigned int n   = 72;
    crc_scheme check = LIQUID_CRC_24;
    fec_scheme fec0  = LIQUID_FEC_NONE;
    fec_scheme fec1  = LIQUID_FEC_GOLAY2412;
    assert(packetizer_compute_enc_msg_len(n, check, fec0, fec1)==150);
    q->p_payload = packetizer_create(n, check, fec0, fec1);

#if DEBUG_FRAMESYNC64
    // set debugging flags, objects to NULL
    q->debug_enabled         = 0;
    q->debug_objects_created = 0;
    q->debug_x               = NULL;
#endif

    // reset state
    framesync64_reset(q);

    return q;
}

// destroy frame synchronizer object, freeing all internal memory
void framesync64_destroy(framesync64 _q)
{
#if DEBUG_FRAMESYNC64
    // clean up debug objects (if created)
    if (_q->debug_objects_created) {
        windowcf_destroy(_q->debug_x);
    }
#endif

    // destroy synchronization objects
    detector_cccf_destroy(_q->frame_detector);  // frame detector
    windowcf_destroy(_q->buffer);               // p/n sample buffer
    firpfb_crcf_destroy(_q->mf);                // matched filter
    firpfb_crcf_destroy(_q->dmf);               // derivative matched filter
    nco_crcf_destroy(_q->nco_coarse);           // coarse NCO
    nco_crcf_destroy(_q->nco_fine);             // fine-tuned NCO
    modem_destroy(_q->demod);                   // payload demodulator
    packetizer_destroy(_q->p_payload);          // payload decoder

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

    // reset carrier recovery objects
    nco_crcf_reset(_q->nco_coarse);
    nco_crcf_reset(_q->nco_fine);

    // reset symbol timing recovery state
    firpfb_crcf_clear(_q->mf);
    firpfb_crcf_clear(_q->dmf);
    _q->pfb_q = 0.0f;   // filtered error signal
        
    // reset state
    _q->state           = STATE_DETECTFRAME;
    _q->pn_counter      = 0;
    _q->payload_counter = 0;
    
    // reset frame statistics
    _q->framestats.evm = 0.0f;
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
        if (_q->debug_enabled) {
            windowcf_push(_q->debug_x, _x[i]);
        }
#endif
        switch (_q->state) {
        case STATE_DETECTFRAME:
            // detect frame (look for p/n sequence)
            framesync64_execute_seekpn(_q, _x[i]);
            break;
        case STATE_RXPN:
            // receive p/n sequence symbols
            framesync64_execute_rxpn(_q, _x[i]);
            break;
        case STATE_RXPAYLOAD:
            // receive payload symbols
            framesync64_execute_rxpayload(_q, _x[i]);
            break;
        default:
            fprintf(stderr,"error: framesync64_exeucte(), unknown/unsupported state\n");
            exit(1);
        }
    }
}

// 
// internal methods
//

// execute synchronizer, seeking p/n sequence
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_seekpn(framesync64   _q,
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
        //printf("***** frame detected! tau-hat:%8.4f, dphi-hat:%8.4f, gamma:%8.2f dB\n",
        //        _q->tau_hat, _q->dphi_hat, 20*log10f(_q->gamma_hat));

        // push buffered samples through synchronizer
        // NOTE: this will set internal state appropriately
        //       to STATE_SEEKPN
        framesync64_pushpn(_q);
    }
}

// update symbol synchronizer internal state (filtered error, index, etc.)
//  _q      :   frame synchronizer
//  _mf     :   matched-filter output
//  _dmf    :   derivative matched-filter output
void framesync64_update_symsync(framesync64   _q,
                                float complex _mf,
                                float complex _dmf)
{
    // update filtered timing error
    // hi  bandwidth parameters: {0.92, 1.20}, about 100 symbols settling time
    // med bandwidth parameters: {0.98, 0.20}, about 200 symbols settling time
    // lo  bandwidth parameters: {0.99, 0.05}, about 500 symbols settling time
    _q->pfb_q = 0.98f*_q->pfb_q + 0.20f*crealf( conjf(_mf)*_dmf );

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

// push buffered p/n sequence through synchronizer
void framesync64_pushpn(framesync64 _q)
{
    unsigned int i;

    // reset filterbanks
    firpfb_crcf_clear(_q->mf);
    firpfb_crcf_clear(_q->dmf);

    // read buffer
    float complex * rc;
    windowcf_read(_q->buffer, &rc);

    // compute delay and filterbank index
    //  tau_hat < 0 :   delay = 2*k*m-1, index = round(   tau_hat *npfb), flag = 0
    //  tau_hat > 0 :   delay = 2*k*m-2, index = round((1-tau_hat)*npfb), flag = 0
    assert(_q->tau_hat < 0.5f && _q->tau_hat > -0.5f);
    unsigned int k     = 2;         // samples/symbol
    unsigned int m     = 3;         // filter delay (symbols)
    unsigned int delay = 2*k*m - 1; // samples to buffer before computing output
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
    
    unsigned int buffer_len = (64+m)*k;
    for (i=0; i<buffer_len; i++) {
        if (i < delay) {
            float complex y;
            nco_crcf_mix_down(_q->nco_coarse, rc[i]*0.5f/_q->gamma_hat, &y);
            nco_crcf_step(_q->nco_coarse);

            // push initial samples into filterbanks
            firpfb_crcf_push(_q->mf,  y);
            firpfb_crcf_push(_q->dmf, y);
        } else {
            // run remaining samples through p/n sequence recovery
            framesync64_execute_rxpn(_q, rc[i]);
        }
    }

    // set state (still need a few more samples before entire p/n
    // sequence has been received)
    _q->state = STATE_RXPN;
}

// execute synchronizer, receiving p/n sequence
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_rxpn(framesync64   _q,
                              float complex _x)
{
    // validate input
    if (_q->pn_counter == 64) {
        fprintf(stderr,"warning: framesync64_execute_rxpn(), p/n buffer already full!\n");
        return;
    }

    // mix signal down
    float complex y;
    nco_crcf_mix_down(_q->nco_coarse, _x*0.5f/_q->gamma_hat, &y);
    nco_crcf_step(_q->nco_coarse);

    // push sample into filterbanks
    firpfb_crcf_push(_q->mf,  y);
    firpfb_crcf_push(_q->dmf, y);

    // compute output if timeout
    if (_q->pfb_timer <= 0) {
        // reset timer
        _q->pfb_timer = 2;  // k samples/symbol

        //
        float complex y;    // matched-filter output
        float complex dy;   // derivatived matched-filter output

        firpfb_crcf_execute(_q->mf,  _q->pfb_index, &y);
        firpfb_crcf_execute(_q->dmf, _q->pfb_index, &dy);

        // update pfb index
        framesync64_update_symsync(_q, y, dy);
#if DEBUG_FRAMESYNC64
        //if (_q->debug_enabled)
        _q->debug_symsync_index[_q->pn_counter] = _q->pfb_soft;
#endif

        // save output in p/n symbols buffer
        _q->pn_syms[ _q->pn_counter ] = y;

        // update p/n counter
        _q->pn_counter++;

        if (_q->pn_counter == 64) {
            framesync64_syncpn(_q);
            _q->state = STATE_RXPAYLOAD;
        }
    }
    
    // decrement symbol timer
    _q->pfb_timer--;
}

// once p/n symbols are buffered, estimate residual carrier
// frequency and phase offsets, push through fine-tuned NCO
void framesync64_syncpn(framesync64 _q)
{
    unsigned int i;

    // estimate residual carrier frequency offset from p/n symbols
    float complex dphi_metric = 0.0f;
    float complex r0 = 0.0f;
    float complex r1 = 0.0f;
    for (i=0; i<64; i++) {
        r0 = r1;
        r1 = _q->pn_syms[i]*_q->pn_sequence[i];
        dphi_metric += r1 * conjf(r0);
    }
    float dphi_hat = cargf(dphi_metric);

    // estimate carrier phase offset from p/n symbols
    float complex theta_metric = 0.0f;
    for (i=0; i<64; i++)
        theta_metric += _q->pn_syms[i]*liquid_cexpjf(-dphi_hat*i)*_q->pn_sequence[i];
    float theta_hat = cargf(theta_metric);
    // TODO: compute gain correction factor

    // initialize fine-tuned nco
    nco_crcf_set_frequency(_q->nco_fine, dphi_hat);
    nco_crcf_set_phase(    _q->nco_fine, theta_hat);

    // correct for carrier offset, pushing through phase-locked loop
    for (i=0; i<64; i++) {
        // mix signal down
        nco_crcf_mix_down(_q->nco_fine, _q->pn_syms[i], &_q->pn_syms[i]);
        
        // push through phase-locked loop
        float phase_error = cimagf(_q->pn_syms[i]*_q->pn_sequence[i]);
        nco_crcf_pll_step(_q->nco_fine, phase_error);
#if DEBUG_FRAMESYNC64
        //if (_q->debug_enabled)
        _q->debug_nco_phase[i] = nco_crcf_get_phase(_q->nco_fine);
#endif

        // update nco phase
        nco_crcf_step(_q->nco_fine);
    }
}

// execute synchronizer, receiving payload
//  _q     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_rxpayload(framesync64   _q,
                                   float complex _x)
{
    // mix signal down
    float complex y;
    nco_crcf_mix_down(_q->nco_coarse, _x*0.5f/_q->gamma_hat, &y);
    nco_crcf_step(_q->nco_coarse);

    // push sample into filterbanks
    firpfb_crcf_push(_q->mf,  y);
    firpfb_crcf_push(_q->dmf, y);

    // compute output if timeout
    if (_q->pfb_timer <= 0) {
        // reset timer
        _q->pfb_timer = 2;  // k samples/symbol

        //
        float complex y;    // matched-filter output
        float complex dy;   // derivatived matched-filter output

        firpfb_crcf_execute(_q->mf,  _q->pfb_index, &y);
        firpfb_crcf_execute(_q->dmf, _q->pfb_index, &dy);

        // update pfb index
        framesync64_update_symsync(_q, y, dy);
#if DEBUG_FRAMESYNC64
        //if (_q->debug_enabled)
        _q->debug_symsync_index[64+_q->payload_counter] = _q->pfb_soft;
#endif

        // push through fine-tuned nco
        nco_crcf_mix_down(_q->nco_fine, y, &y);
        
        // demodulate and compute phase error
        unsigned int sym_out = 0;
        modem_demodulate(_q->demod, y, &sym_out);
        float phase_error = modem_get_demodulator_phase_error(_q->demod);
        float evm         = modem_get_demodulator_evm(_q->demod);
#if DEBUG_FRAMESYNC64
        //if (_q->debug_enabled)
        _q->debug_nco_phase[64+_q->payload_counter] = nco_crcf_get_phase(_q->nco_fine);
#endif

        // update phase-locked loop and fine-tuned NCO
        nco_crcf_pll_step(_q->nco_fine, phase_error);
        nco_crcf_step(_q->nco_fine);

        // update error vector magnitude
        _q->framestats.evm += evm*evm;

        // save output in p/n symbols buffer
        _q->payload_syms[ _q->payload_counter ] = y;
        
        // pack encoded symbols
        _q->payload_enc[ _q->payload_counter/4 ] <<= 2;
        _q->payload_enc[ _q->payload_counter/4 ] |= sym_out;
        
        // increment counter
        _q->payload_counter++;

        if (_q->payload_counter == 600) {
            // decode payload and invoke callback
            framesync64_decode_payload(_q);
            
            if (_q->callback != NULL) {
                // invoke user-defined callback function
                _q->framestats.evm           = 20*log10f(sqrtf(_q->framestats.evm / 600.0f));
                _q->framestats.rssi          = 20*log10f(_q->gamma_hat);
                _q->framestats.cfo           = nco_crcf_get_frequency(_q->nco_coarse) +
                                               nco_crcf_get_frequency(_q->nco_fine) / 2.0f;
                _q->framestats.framesyms     = _q->payload_syms;
                _q->framestats.num_framesyms = 600;
                _q->framestats.mod_scheme    = LIQUID_MODEM_QPSK;
                _q->framestats.mod_bps       = 2;
                _q->framestats.check         = LIQUID_CRC_24;
                _q->framestats.fec0          = LIQUID_FEC_NONE;
                _q->framestats.fec1          = LIQUID_FEC_GOLAY2412;

                _q->callback(_q->payload_dec,
                             _q->crc_pass,
                             &_q->payload_dec[8],
                             64,
                             _q->crc_pass,
                             _q->framestats,
                             _q->userdata);
            }

            framesync64_reset(_q);
        }
    }

    // decrement symbol timer
    _q->pfb_timer--;
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

// decode payload
void framesync64_decode_payload(framesync64 _q)
{
    // unscramble data
    unscramble_data(_q->payload_enc, 150);

    // decode payload
    _q->crc_pass =
    packetizer_decode(_q->p_payload, _q->payload_enc, _q->payload_dec);
}

#if 0
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
#endif

// enable debugging
void framesync64_debug_enable(framesync64 _q)
{
    // create debugging objects if necessary
#if DEBUG_FRAMESYNC64
    if (_q->debug_objects_created)
        return;

    // create debugging objects
    _q->debug_x = windowcf_create(DEBUG_BUFFER_LEN);

    // set debugging flags
    _q->debug_enabled = 1;
    _q->debug_objects_created = 1;
#else
    fprintf(stderr,"framesync64_debug_enable(): compile-time debugging disabled\n");
#endif
}

// disable debugging
void framesync64_debug_disable(framesync64 _q)
{
    // disable debugging
#if DEBUG_FRAMESYNC64
    _q->debug_enabled = 0;
#else
    fprintf(stderr,"framesync64_debug_enable(): compile-time debugging disabled\n");
#endif
}


// print debugging information
void framesync64_debug_print(framesync64  _q,
                             const char * _filename)
{
#if DEBUG_FRAMESYNC64
    if (!_q->debug_objects_created) {
        fprintf(stderr,"error: framesync64_debug_print(), debugging objects don't exist; enable debugging first\n");
        return;
    }
    unsigned int i;
    float complex * rc;
    float         * r;
    FILE* fid = fopen(_filename,"w");
    fprintf(fid,"%% %s: auto-generated file", _filename);
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
    r = _q->pn_sequence;
    for (i=0; i<64; i++)
        fprintf(fid,"pn_sequence(%4u) = %12.4e;\n", i+1, r[i]);

    // write p/n symbols
    fprintf(fid,"pn_syms = zeros(1,64);\n");
    rc = _q->pn_syms;
    for (i=0; i<64; i++)
        fprintf(fid,"pn_syms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));

    // write payload symbols
    fprintf(fid,"payload_syms = zeros(1,600);\n");
    rc = _q->payload_syms;
    for (i=0; i<600; i++)
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

    // NCO, timing, etc.
    fprintf(fid,"symsync_index = zeros(1,664);\n");
    fprintf(fid,"nco_phase     = zeros(1,664);\n");
    for (i=0; i<664; i++) {
        fprintf(fid,"symsync_index(%4u) = %12.4e;\n", i+1, _q->debug_symsync_index[i]);
        fprintf(fid,"nco_phase(%4u)     = %12.4e;\n", i+1, _q->debug_nco_phase[i]);
    }
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(nco_phase);\n");
    fprintf(fid,"  ylabel('nco phase');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(symsync_index);\n");
    fprintf(fid,"  ylabel('symsync index');\n");
    fprintf(fid,"  grid on;\n");

    fprintf(fid,"\n\n");
    fclose(fid);

    printf("framesync64/debug: results written to %s\n", _filename);
#else
    fprintf(stderr,"framesync64_debug_print(): compile-time debugging disabled\n");
#endif
}
