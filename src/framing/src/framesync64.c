/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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

#define FRAMESYNC64_SQUELCH_TIMEOUT (32)

#define FRAME64_PN_LEN              (64)

#define DEBUG_FRAMESYNC64           1
#define DEBUG_FRAMESYNC64_PRINT     0
#define DEBUG_FILENAME              "framesync64_internal_debug.m"
#define DEBUG_BUFFER_LEN            (4096)

void framesync64_debug_print(framesync64 _fs);

// framesync64 object structure
struct framesync64_s {
    modem demod_payload;    // payload demodulator (bpsk)
    modem demod_header;     // preamble/header/payload demodulator (qpsk)
    packetizer p_header;    // header packetizer
    packetizer p_payload;   // payload packetizer

    // synchronizer objects
    agc_crcf agc_rx;        // automatic gain control
    symsync_crcf mfdecim;   // symbol synchronizer (timing recovery)
    nco_crcf nco_rx;        // oscillator and phase-locked loop
    bsync_rrrf fsync;       // p/n sequence correlator

    // generic user-configurable properties
    framesyncprops_s props;

    // squelch
    int squelch_status;     // status of AGC squelch

    // status variables
    enum {
        FRAMESYNC64_STATE_SEEKPN=0,     // seek p/n sequence
        FRAMESYNC64_STATE_RXHEADER,     // receive header data
        FRAMESYNC64_STATE_RXPAYLOAD,    // receive payload data
        FRAMESYNC64_STATE_RESET         // reset synchronizer
    } state;
    unsigned int num_symbols_collected; // symbols collected counter
    unsigned int header_key;            // header cyclic redundancy check
    unsigned int payload_key;           // payload cyclic redundancy check
    int header_valid;                   // header valid?
    int payload_valid;                  // payload valid?

    // callback
    framesync64_callback callback;      // user-defined callback function
    void * userdata;                    // user-defined data structure
    framesyncstats_s framestats;        // frame statistics object

    // header
    unsigned char header_sym[84];       // header symbols (modem output)
    unsigned char header_enc[21];       // header data (encoded)
    unsigned char header[12];           // header data (decoded)

    // payload
    unsigned char payload_sym[396];     // payload symbols (modem output)
    unsigned char payload_enc[99];      // payload data (encoded)
    unsigned char payload[64];          // payload data (decoded)

    // SINDR estimate (signal to interference, noise, and distortion
    // ratio), average modem error vector magnitude
    float evm_hat;

    // medium access control
    unsigned int csma_enabled;
    framesync_csma_callback csma_lock;
    framesync_csma_callback csma_unlock;
    void * csma_userdata;


#if DEBUG_FRAMESYNC64
    windowf  debug_agc_rssi;
    windowcf debug_agc_out;
    windowcf debug_x;
    windowcf debug_rxy;
    windowcf debug_nco_rx_out;
    windowcf debug_framesyms;
    windowf  debug_nco_phase;
    windowf  debug_nco_freq;
#endif
};

// create framesync64 object
//  _props          :   properties structure pointer (default if NULL)
//  _callback       :   callback function invoked when frame is received
//  _userdata       :   user-defined data object passed to callback
framesync64 framesync64_create(framesyncprops_s * _props,
                               framesync64_callback _callback,
                               void * _userdata)
{
    framesync64 fs = (framesync64) malloc(sizeof(struct framesync64_s));
    fs->callback = _callback;
    fs->userdata = _userdata;

    // set fixed properties of the frame statistics
    fs->framestats.mod_scheme   = LIQUID_MODEM_QPSK;
    fs->framestats.mod_bps      = 2;
    fs->framestats.check        = LIQUID_CRC_16;
    fs->framestats.fec0         = LIQUID_FEC_HAMMING128;
    fs->framestats.fec1         = LIQUID_FEC_NONE;

    // set properties (initial memmove to prevent internal warnings)
    memmove(&fs->props, &framesyncprops_default, sizeof(framesyncprops_s));
    if (_props != NULL)
        framesync64_setprops(fs,_props);
    else
        framesync64_setprops(fs, &framesyncprops_default);

    // agc, rssi, squelch
    fs->agc_rx = agc_crcf_create();
    agc_crcf_set_bandwidth(fs->agc_rx, fs->props.agc_bw0);
    agc_crcf_set_gain_limits(fs->agc_rx, fs->props.agc_gmin, fs->props.agc_gmax);

    agc_crcf_squelch_activate(fs->agc_rx);
    agc_crcf_squelch_set_threshold(fs->agc_rx, fs->props.squelch_threshold);
    agc_crcf_squelch_set_timeout(fs->agc_rx, FRAMESYNC64_SQUELCH_TIMEOUT);

    agc_crcf_squelch_enable_auto(fs->agc_rx);
    fs->squelch_status = LIQUID_AGC_SQUELCH_SIGNALHI;

    // pll, nco
    fs->nco_rx = nco_crcf_create(LIQUID_VCO);
    nco_crcf_pll_set_bandwidth(fs->nco_rx, fs->props.pll_bw0);

    // bsync (p/n synchronizer)
    unsigned int i;
    msequence ms = msequence_create(6, 0x0043, 1);
    float pn_sequence[FRAME64_PN_LEN];
    for (i=0; i<FRAME64_PN_LEN; i++)
        pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    fs->fsync = bsync_rrrf_create(FRAME64_PN_LEN, pn_sequence);
    msequence_destroy(ms);

    // design symsync (k=2)
    unsigned int npfb = 32;
    unsigned int m=3;
    float beta=0.7f;
    fs->mfdecim = symsync_crcf_create_rnyquist(LIQUID_RNYQUIST_ARKAISER, 2, m, beta, npfb);

    // create header/payload packetizers
    fs->p_header  = packetizer_create(12, LIQUID_CRC_16, LIQUID_FEC_NONE, LIQUID_FEC_HAMMING128);
    fs->p_payload = packetizer_create(64, LIQUID_CRC_16, LIQUID_FEC_NONE, LIQUID_FEC_HAMMING128);

    // create demod
    fs->demod_payload = modem_create(LIQUID_MODEM_QPSK);
    fs->demod_header  = modem_create(LIQUID_MODEM_BPSK);

    // set status flags
    fs->state = FRAMESYNC64_STATE_SEEKPN;
    fs->num_symbols_collected = 0;

    // reset, open bandwidths
    framesync64_reset(fs);
    framesync64_open_bandwidth(fs);

#if DEBUG_FRAMESYNC64
    fs->debug_agc_rssi  =  windowf_create(DEBUG_BUFFER_LEN);
    fs->debug_agc_out   = windowcf_create(DEBUG_BUFFER_LEN);
    fs->debug_x         = windowcf_create(DEBUG_BUFFER_LEN);
    fs->debug_rxy       = windowcf_create(DEBUG_BUFFER_LEN);
    fs->debug_nco_rx_out= windowcf_create(DEBUG_BUFFER_LEN);
    fs->debug_framesyms = windowcf_create(DEBUG_BUFFER_LEN);
    fs->debug_nco_phase =  windowf_create(DEBUG_BUFFER_LEN);
    fs->debug_nco_freq  =  windowf_create(DEBUG_BUFFER_LEN);
#endif

    // advanced mode : csma
    fs->csma_enabled = 0;
    fs->csma_lock = NULL;
    fs->csma_unlock = NULL;
    fs->csma_userdata = NULL;

    return fs;
}

// get framesync64 properties
//  _fs     :   frame synchronizer object
//  _props  :   frame synchronizer properties structure pointer
void framesync64_getprops(framesync64 _fs,
                          framesyncprops_s * _props)
{
    memmove(_props, &_fs->props, sizeof(framesyncprops_s));
}

// set framesync64 properties
//  _fs     :   frame synchronizer object
//  _props  :   frame synchronizer properties structure pointer
void framesync64_setprops(framesync64 _fs,
                          framesyncprops_s * _props)
{
    // TODO : framesync64_setprops() validate input

    if (_props->k       != _fs->props.k     ||
        _props->npfb    != _fs->props.npfb  ||
        _props->m       != _fs->props.m     ||
        _props->beta    != _fs->props.beta)
    {
        fprintf(stderr,"warning: framesync64_setprops(), ignoring filter change\n");
        // TODO : destroy/recreate filter
    }
    memmove(&_fs->props, _props, sizeof(framesyncprops_s));
}

// destroy frame synchronizer object, freeing all internal memory
void framesync64_destroy(framesync64 _fs)
{
#if DEBUG_FRAMESYNC64
    framesync64_debug_print(_fs);

    // clean up debug windows
    windowf_destroy( _fs->debug_agc_rssi);
    windowcf_destroy(_fs->debug_agc_out);
    windowcf_destroy(_fs->debug_x);
    windowcf_destroy(_fs->debug_rxy);
    windowcf_destroy(_fs->debug_nco_rx_out);
    windowcf_destroy(_fs->debug_framesyms);
    windowf_destroy( _fs->debug_nco_phase);
    windowf_destroy( _fs->debug_nco_freq);
#endif

    // destroy synchronization objects
    agc_crcf_destroy(_fs->agc_rx);      // automatic gain control
    symsync_crcf_destroy(_fs->mfdecim); // symbol synchronizer
    nco_crcf_destroy(_fs->nco_rx);      // nco/pll for carrier recovery
    bsync_rrrf_destroy(_fs->fsync);     // p/n sequence correlator
    packetizer_destroy(_fs->p_header);  // header packetizer
    packetizer_destroy(_fs->p_payload); // payload packetizer

    // destroy modem objects
    modem_destroy(_fs->demod_header);
    modem_destroy(_fs->demod_payload);

    // free main object memory
    free(_fs);
}

// print frame synchronizer object internals
void framesync64_print(framesync64 _fs)
{
    printf("framesync64:\n");
    printf("    agc signal min/max  :   %5.1f dB / %5.1fdB\n", -10*log10f(_fs->props.agc_gmax), -10*log10f(_fs->props.agc_gmin) );
    printf("    agc b/w open/closed :   %8.2e / %8.2e\n", _fs->props.agc_bw0, _fs->props.agc_bw1);
    printf("    sym b/w open/closed :   %8.2e / %8.2e\n", _fs->props.sym_bw0, _fs->props.sym_bw1);
    printf("    pll b/w open/closed :   %8.2e / %8.2e\n", _fs->props.pll_bw0, _fs->props.pll_bw1);
    printf("    samples/symbol      :   %u\n", _fs->props.k);
    printf("    filter length       :   %u\n", _fs->props.m);
    printf("    num filters (ppfb)  :   %u\n", _fs->props.npfb);
    printf("    filter excess b/w   :   %6.4f\n", _fs->props.beta);
    printf("    squelch             :   %s\n", _fs->props.squelch_enabled     ? "enabled" : "disabled");
    printf("    auto-squelch        :   %s\n", _fs->props.autosquelch_enabled ? "enabled" : "disabled");
    printf("    squelch threshold   :   %6.2f dB\n", _fs->props.squelch_threshold);
    printf("    csma                :   %s\n", _fs->csma_enabled ? "enabled" : "disabled");
    printf("    ----\n");
    printf("    p/n sequence len    :   %u\n", FRAME64_PN_LEN);
    printf("    payload len         :   %u bytes\n", 64);

    // TODO : print statistics...
}

// reset frame synchronizer object
void framesync64_reset(framesync64 _fs)
{
    // reset synchronization objects
    agc_crcf_unlock(_fs->agc_rx);       // automatic gain control (unlock)
    symsync_crcf_clear(_fs->mfdecim);   // symbol synchronizer (clear state)
    symsync_crcf_unlock(_fs->mfdecim);  // symbol synchronizer (unlock)
    nco_crcf_reset(_fs->nco_rx);        // nco/pll (reset phase)

    // SNR estimate
    _fs->evm_hat = 0.0f;

    // enable/disable squelch
    if (_fs->props.squelch_enabled)
        agc_crcf_squelch_activate(_fs->agc_rx);
    else
        agc_crcf_squelch_deactivate(_fs->agc_rx);

    // enable/disable auto-squelch
    if (_fs->props.autosquelch_enabled)
        agc_crcf_squelch_enable_auto(_fs->agc_rx);
    else
        agc_crcf_squelch_disable_auto(_fs->agc_rx);

    // TODO open bandwidth?
}

// execute frame synchronizer
//  _fs     :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void framesync64_execute(framesync64 _fs,
                         float complex *_x,
                         unsigned int _n)
{
    unsigned int i, j, nw;
    float complex agc_rx_out;
    float complex mfdecim_out[4];
    float complex nco_rx_out;
    float phase_error;
    unsigned int demod_sym;

    for (i=0; i<_n; i++) {
        // run agc
        agc_crcf_execute(_fs->agc_rx, _x[i], &agc_rx_out);

#if DEBUG_FRAMESYNC64
        windowcf_push(_fs->debug_x, _x[i]);
        windowf_push(_fs->debug_agc_rssi, agc_crcf_get_signal_level(_fs->agc_rx));
        windowcf_push(_fs->debug_agc_out, agc_rx_out);
#endif

        // squelch: block agc output from synchronizer only if
        // 1. received signal strength indicator has not exceeded squelch
        //    threshold at any time within the past <squelch_timeout> samples
        // 2. mode is FRAMESYNC64_STATE_SEEKPN (seek p/n sequence)
        _fs->squelch_status = agc_crcf_squelch_get_status(_fs->agc_rx);

        // if squelch is enabled, skip remaining of synchronizer
        // NOTE : if squelch is deactivated, the default status
        //        value is LIQUID_AGC_SQUELCH_SIGNALHI
        if (_fs->squelch_status == LIQUID_AGC_SQUELCH_RISE) {
            // invoke csma lock function, if enabled
            framesync64_csma_lock(_fs);

        } else if (_fs->squelch_status == LIQUID_AGC_SQUELCH_TIMEOUT) {
            // invoke csma unlock function, if enabled
            framesync64_csma_unlock(_fs);

            // reset on timeout (very important!)
            framesync64_reset(_fs);

        } else if (_fs->squelch_status == LIQUID_AGC_SQUELCH_ENABLED) {
            continue;
        }

        // symbol synchronizer
        symsync_crcf_execute(_fs->mfdecim, &agc_rx_out, 1, mfdecim_out, &nw);

        for (j=0; j<nw; j++) {
            // mix down, demodulate, run PLL
            nco_crcf_mix_down(_fs->nco_rx, mfdecim_out[j], &nco_rx_out);

            // run demodulator and retrieve phase error
            if (_fs->state == FRAMESYNC64_STATE_SEEKPN) {
                // use preamble/header demodulator
                modem_demodulate(_fs->demod_header, nco_rx_out, &demod_sym);
                phase_error = modem_get_demodulator_phase_error(_fs->demod_header);
            } else {
                // use payload demodulator
                modem_demodulate(_fs->demod_payload, nco_rx_out, &demod_sym);
                phase_error = modem_get_demodulator_phase_error(_fs->demod_payload);
            }

            // step pll, nco objects
            nco_crcf_pll_step(_fs->nco_rx, phase_error);
            nco_crcf_step(_fs->nco_rx);

#if DEBUG_FRAMESYNC64
            windowf_push(_fs->debug_nco_phase, _fs->nco_rx->theta);
            windowf_push(_fs->debug_nco_freq,  _fs->nco_rx->d_theta);
            windowcf_push(_fs->debug_nco_rx_out, nco_rx_out);
#endif

            // run state-specific execute method
            switch (_fs->state) {
            case FRAMESYNC64_STATE_SEEKPN:
                framesync64_execute_seekpn(_fs, nco_rx_out, demod_sym);
                break;
            case FRAMESYNC64_STATE_RXHEADER:
                framesync64_execute_rxheader(_fs, nco_rx_out, demod_sym);
                break;
            case FRAMESYNC64_STATE_RXPAYLOAD:
                framesync64_execute_rxpayload(_fs, nco_rx_out, demod_sym);
                break;
            case FRAMESYNC64_STATE_RESET:
                framesync64_execute_reset(_fs, nco_rx_out, demod_sym);
                break;
            default:;
            }
        }
    }
}

// 
// internal
//

// open bandwidth of synchronizers (acquisition mode)
void framesync64_open_bandwidth(framesync64 _fs)
{
    // open bandwidth of automatic gain control
    agc_crcf_set_bandwidth(_fs->agc_rx, _fs->props.agc_bw0);

    // open bandwidth of symbol synchronizer
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->props.sym_bw0);

    // open bandwidth of nco/pll
    nco_crcf_pll_set_bandwidth(_fs->nco_rx, _fs->props.pll_bw0);
}

// close bandwidth of synchronizers (tracking mode)
void framesync64_close_bandwidth(framesync64 _fs)
{
    // close bandwidth of automatic gain control
    agc_crcf_set_bandwidth(_fs->agc_rx, _fs->props.agc_bw1);

    // close bandwidth of symbol synchronizer
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->props.sym_bw1);

    // close bandwidth of nco/pll
    nco_crcf_pll_set_bandwidth(_fs->nco_rx, _fs->props.pll_bw1);
}

// 
// state-specific execute methods
//

// execute synchronizer, seeking p/n sequence
//  _fs     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_seekpn(framesync64 _fs,
                                float complex _x,
                                unsigned int _sym)
{
    // run cross-correlator to find p/n sequence
    float rxy;
    bsync_rrrf_correlate(_fs->fsync, _x, &rxy);

#if DEBUG_FRAMESYNC64
    windowcf_push(_fs->debug_rxy, rxy);
#endif

    // check if p/n sequence is found (correlation value
    // exceeds threshold)
    if (fabsf(rxy) > 0.7f) {
        // close bandwidth
        framesync64_close_bandwidth(_fs);

        // adjust phase of receiver NCO based on p/n correlation phase
        nco_crcf_adjust_phase(_fs->nco_rx, M_PI - cargf(rxy));

        // deactivate squelch as not to suppress signal in the
        // middle of the frame
        agc_crcf_squelch_deactivate(_fs->agc_rx);

        // update synchronizer state
        _fs->state = FRAMESYNC64_STATE_RXHEADER;
    }
}

// execute synchronizer, receiving header
//  _fs     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_rxheader(framesync64 _fs,
                                  float complex _x,
                                  unsigned int _sym)
{
    // append symbol to buffer
    _fs->header_sym[_fs->num_symbols_collected] = (unsigned char) _sym;
    _fs->num_symbols_collected++;

    // check to see if full header has been received
    if (_fs->num_symbols_collected==84) {
        // reset symbol counter
        _fs->num_symbols_collected = 0;

        // decode frame header
        framesync64_decode_header(_fs);

        // update synchronizer state
        _fs->state = FRAMESYNC64_STATE_RXPAYLOAD;
    }

}

// execute synchronizer, receiving payload
//  _fs     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_rxpayload(framesync64 _fs,
                                   float complex _x,
                                   unsigned int _sym)
{
#if DEBUG_FRAMESYNC64
    windowcf_push(_fs->debug_framesyms, _x);
#endif

    // append symbol to buffer
    _fs->payload_sym[_fs->num_symbols_collected] = (unsigned char) _sym;
    _fs->num_symbols_collected++;

    // SNR estimate
    float evm;
    evm = modem_get_demodulator_evm(_fs->demod_header);
    _fs->evm_hat += evm;

    // check to see if full payload has been received
    if (_fs->num_symbols_collected==396) {
        // reset symbol counter
        _fs->num_symbols_collected = 0;

        // decode frame payload
        framesync64_decode_payload(_fs);

        // framestats: compute EVM estimate, rssi
        // 477 = 84 + 396 = total number of observed symbols
        _fs->framestats.evm  =  10*log10f( (_fs->evm_hat / 477.0f) );
        _fs->framestats.rssi =  10*log10(agc_crcf_get_signal_level(_fs->agc_rx));
        _fs->evm_hat = 0.0f;

        // framestats: set pointer to frame symbols
        _fs->framestats.framesyms = NULL;
        _fs->framestats.num_framesyms = 0;

        // invoke callback method
        _fs->callback(_fs->header,  _fs->header_valid,
                      _fs->payload, _fs->payload_valid,
                      _fs->framestats,
                      _fs->userdata);

        // update synchronizer state
        _fs->state = FRAMESYNC64_STATE_RESET;
        //_fs->state = FRAMESYNC64_STATE_SEEKPN;
    }
}

// execute synchronizer, resetting object
//  _fs     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void framesync64_execute_reset(framesync64 _fs,
                               float complex _x,
                               unsigned int _sym)
{
    // open bandwidth
    framesync64_open_bandwidth(_fs);

    // re-activate squelch
    if (_fs->props.squelch_enabled)
        agc_crcf_squelch_activate(_fs->agc_rx);

    // reset oscillator
    nco_crcf_reset(_fs->nco_rx);

    // update synchronizer state
    _fs->state = FRAMESYNC64_STATE_SEEKPN;
}


// enable csma and set external callback functions
//  _fs             :   frame synchronizer object
//  _csma_lock      :   callback to be invoked when signal is high
//  _csma_unlock    :   callback to be invoked when signal is again low
//  _csma_userdata  :   structure passed to callback functions
void framesync64_set_csma_callbacks(framesync64 _fs,
                                    framesync_csma_callback _csma_lock,
                                    framesync_csma_callback _csma_unlock,
                                    void * _csma_userdata)
{
    // enable csma
    _fs->csma_enabled = 1;

    // set internal callback functions
    _fs->csma_lock = _csma_lock;
    _fs->csma_unlock = _csma_unlock;

    // set internal user data
    _fs->csma_userdata = _csma_userdata;
}

// if enabled, invoke external csma lock callback
void framesync64_csma_lock(framesync64 _fs)
{
    if (_fs->csma_enabled && _fs->csma_lock != NULL)
        _fs->csma_lock( _fs->csma_userdata );
}

// if enabled, invoke external csma unlock callback
void framesync64_csma_unlock(framesync64 _fs)
{
    if (_fs->csma_enabled && _fs->csma_unlock != NULL)
        _fs->csma_unlock( _fs->csma_userdata );
}


// 
// decoding methods
//

// decode header
void framesync64_decode_header(framesync64 _fs)
{
    unsigned int i;
    for (i=0; i<21; i++)
        framesync64_syms_to_byte(_fs->header_sym+(4*i), _fs->header_enc+i);

#if DEBUG_FRAMESYNC64_PRINT
    printf("header ENCODED (rx):\n");
    for (i=0; i<64; i++) {
        printf("%2x ", _fs->header_enc[i]);
        if (!((i+1)%16)) printf("\n");
    }
    printf("\n");
#endif

    // unscramble header data
    unscramble_data(_fs->header_enc, 21);

    // decode header and validate crc
    _fs->header_valid = packetizer_decode(_fs->p_header, _fs->header_enc, _fs->header);


#if DEBUG_FRAMESYNC64_PRINT
    printf("header (rx):\n");
    for (i=0; i<32; i++) {
        printf("%2x ", _fs->header[i]);
        if (!((i+1)%8)) printf("\n");
    }
    printf("\n");
#endif
}

void framesync64_decode_payload(framesync64 _fs)
{
    unsigned int i;
    for (i=0; i<99; i++)
        framesync64_syms_to_byte(&(_fs->payload_sym[4*i]), &(_fs->payload_enc[i]));

    // unscramble payload data
    unscramble_data(_fs->payload_enc, 99);

    // decode payload and validate crc
    _fs->payload_valid = packetizer_decode(_fs->p_payload, _fs->payload_enc, _fs->payload);

#if DEBUG_FRAMESYNC64_PRINT
    printf("payload (rx):\n");
    for (i=0; i<64; i++) {
        printf("%2x ", _fs->payload[i]);
        if (!((i+1)%8)) printf("\n");
    }
    printf("\n");
#endif

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
void framesync64_debug_print(framesync64 _fs)
{
#if DEBUG_FRAMESYNC64
    unsigned int i;
    float * r;
    float complex * rc;
    FILE* fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file", DEBUG_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    // write agc_rssi
    fprintf(fid,"agc_rssi = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowf_read(_fs->debug_agc_rssi, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(10*log10(agc_rssi))\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");

    // write agc out
    fprintf(fid,"agc_out = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowcf_read(_fs->debug_agc_out, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"agc_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(agc_out),real(agc_out), 1:length(agc_out),imag(agc_out));\n");
    fprintf(fid,"ylabel('agc-out');\n");


    // write x
    fprintf(fid,"x = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowcf_read(_fs->debug_x, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    // write rxy
    fprintf(fid,"rxy = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowcf_read(_fs->debug_rxy, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(abs(rxy))\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    // write nco_rx_out
    fprintf(fid,"nco_rx_out = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowcf_read(_fs->debug_nco_rx_out, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"nco_rx_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_rx_out,'x')\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"title('All symbols');\n");
    fprintf(fid,"axis([-1.2 1.2 -1.2 1.2]);\n");
    fprintf(fid,"axis square;\n");

    // write framesyms
    fprintf(fid,"framesyms = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowcf_read(_fs->debug_framesyms, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(framesyms,'x','MarkerSize',1)\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"title('Frame Symbols');\n");
    fprintf(fid,"axis([-1.2 1.2 -1.2 1.2]);\n");
    fprintf(fid,"axis square;\n");

    // write nco_phase
    fprintf(fid,"nco_phase = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowf_read(_fs->debug_nco_phase, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"nco_phase(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_phase)\n");
    fprintf(fid,"ylabel('nco phase [radians]');\n");

    // write nco_freq
    fprintf(fid,"nco_freq = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    windowf_read(_fs->debug_nco_freq, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"nco_freq(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_freq)\n");
    fprintf(fid,"ylabel('nco freq');\n");


    fprintf(fid,"\n\n");
    fclose(fid);

    printf("framesync64/debug: results written to %s\n", DEBUG_FILENAME);
#endif
}
