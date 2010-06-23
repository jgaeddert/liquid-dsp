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
//
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

#include "liquid.internal.h"

#define FLEXFRAMESYNC_SQUELCH_TIMEOUT   (32)

#define FLEXFRAMESYNC_PN_LEN            (64)


#define DEBUG_FLEXFRAMESYNC             1
#define DEBUG_FLEXFRAMESYNC_PRINT       0
#define DEBUG_FLEXFRAMESYNC_FILENAME    "flexframesync_internal_debug.m"
#define DEBUG_FLEXFRAMESYNC_BUFFER_LEN  (4096)

#if DEBUG_FLEXFRAMESYNC
void flexframesync_output_debug_file(flexframesync _fs);
#endif

struct flexframesync_s {

    // synchronizer objects
    agc_crcf agc_rx;
    symsync_crcf mfdecim;
    nco_crcf nco_rx;
    bsync_rrrf fsync;

    //
    float rssi;
    int squelch_status;

    // status variables
    enum {
        FLEXFRAMESYNC_STATE_SEEKPN=0,
        FLEXFRAMESYNC_STATE_RXHEADER,
        FLEXFRAMESYNC_STATE_RXPAYLOAD,
        FLEXFRAMESYNC_STATE_RESET
    } state;
    unsigned int num_symbols_collected;
    unsigned int header_key;
    bool header_valid;

    // preamble
    modem mod_preamble;
    unsigned int pnsequence_len;

    // header
    modem mod_header;
    packetizer p_header;
    float complex header_samples[256];
    unsigned char header_sym[256];
    unsigned char header_enc[32];
    unsigned char header[12];

    // SINDR estimate (signal to interference, noise,
    // and distortion ratio)
    float evm;          // instantaneous error vector magnitude
    float evm_hat;      // averaged EVM
    float SINDRdB_hat;  // estimated SINDR (dB)

    // header properties
    modulation_scheme ms_payload;
    unsigned int bps_payload;
    unsigned int payload_len;

    // payload
    unsigned int num_payload_symbols;
    modem mod_payload;
    float complex * payload_samples;
    unsigned char * payload_sym;
    unsigned char * payload;
    unsigned int payload_samples_numalloc;
    unsigned int payload_sym_numalloc;
    unsigned int payload_numalloc;

    // generic user-configurable properties
    framesyncprops_s props;

    // callback
    flexframesync_callback callback;
    void * userdata;

#ifdef DEBUG_FLEXFRAMESYNC
    FILE*fid;
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

flexframesync flexframesync_create(framesyncprops_s * _props,
                                   flexframesync_callback _callback,
                                   void * _userdata)
{
    flexframesync fs = (flexframesync) malloc(sizeof(struct flexframesync_s));
    fs->callback = _callback;
    fs->userdata = _userdata;

    // set properties (initial memmove to prevent internal warnings)
    memmove(&fs->props, &framesyncprops_default, sizeof(framesyncprops_s));
    if (_props != NULL)
        flexframesync_setprops(fs,_props);
    else
        flexframesync_setprops(fs, &framesyncprops_default);

    // header objects
    fs->mod_header = modem_create(MOD_BPSK, 1);
    fs->p_header = packetizer_create(12, FEC_HAMMING74, FEC_NONE);
    assert(packetizer_get_enc_msg_len(fs->p_header)==32);

    // agc, rssi, squelch
    fs->agc_rx = agc_crcf_create();
    agc_crcf_set_target(fs->agc_rx, 1.0f);
    agc_crcf_set_bandwidth(fs->agc_rx, fs->props.agc_bw0);
    agc_crcf_set_gain_limits(fs->agc_rx, fs->props.agc_gmin, fs->props.agc_gmax);

    agc_crcf_squelch_activate(fs->agc_rx);
    agc_crcf_squelch_set_threshold(fs->agc_rx, fs->props.squelch_threshold);
    agc_crcf_squelch_set_timeout(fs->agc_rx, FLEXFRAMESYNC_SQUELCH_TIMEOUT);

    agc_crcf_squelch_enable_auto(fs->agc_rx);
    fs->squelch_status = LIQUID_AGC_SQUELCH_SIGNALHI;

    // pll, nco
    fs->nco_rx = nco_crcf_create(LIQUID_NCO);
    nco_crcf_pll_set_bandwidth(fs->nco_rx, fs->props.pll_bw0);

    // bsync (p/n synchronizer)
    // TODO : add separate method to configure p/n sequence
    unsigned int i;
    fs->pnsequence_len = FLEXFRAMESYNC_PN_LEN;
    // TODO : adjust msequence based on p/n sequence length
    msequence ms = msequence_create(6);
    float pn_sequence[fs->pnsequence_len];
    for (i=0; i<fs->pnsequence_len; i++)
        pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    fs->fsync = bsync_rrrf_create(fs->pnsequence_len, pn_sequence);
    msequence_destroy(ms);

    // design symsync (k=2)
    unsigned int npfb = 32;
    unsigned int m=3;
    float beta=0.7f;
    unsigned int H_len = 2*2*npfb*m + 1;// 2*2*npfb*_m + 1;
    float H[H_len];
    design_rrc_filter(2*npfb,m,beta,0,H);
    fs->mfdecim =  symsync_crcf_create(2, npfb, H, H_len-1);

    // 
    fs->mod_preamble = modem_create(MOD_BPSK, 1);

    // flexible frame properties (default values to be over-written
    // when frame header is received and decoded)
    fs->ms_payload  = MOD_PSK;
    fs->bps_payload = 1;
    fs->payload_len = 0;

    // set status flags
    fs->state = FLEXFRAMESYNC_STATE_SEEKPN;
    fs->num_symbols_collected = 0;

    // payload buffers, objects
    fs->mod_payload = modem_create(fs->ms_payload, fs->bps_payload);
    fs->payload_samples = NULL;
    fs->payload_sym = NULL;
    fs->payload = NULL;
    fs->payload_samples_numalloc = 0;
    fs->payload_sym_numalloc = 0;
    fs->payload_numalloc = 0;

    // reset, open bandwidth
    flexframesync_reset(fs);
    flexframesync_open_bandwidth(fs);

#ifdef DEBUG_FLEXFRAMESYNC
    fs->debug_agc_rssi  =  windowf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_agc_out   = windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_x         = windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_rxy       = windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_rx_out= windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_framesyms = windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_phase=   windowf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_freq =   windowf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
#endif

    return fs;
}

void flexframesync_destroy(flexframesync _fs)
{
#if DEBUG_FLEXFRAMESYNC
    flexframesync_output_debug_file(_fs);
#endif

    // destroy synchronizer objects
    agc_crcf_destroy(_fs->agc_rx);
    nco_crcf_destroy(_fs->nco_rx);
    bsync_rrrf_destroy(_fs->fsync);
    symsync_crcf_destroy(_fs->mfdecim);

    // destroy preamble objects
    modem_destroy(_fs->mod_preamble);

    // destroy header objects
    modem_destroy(_fs->mod_header);
    packetizer_destroy(_fs->p_header);

    // free payload objects
    modem_destroy(_fs->mod_payload);
    free(_fs->payload_samples);
    free(_fs->payload_sym);
    free(_fs->payload);

    free(_fs);
}

void flexframesync_getprops(flexframesync _fs, framesyncprops_s * _props)
{
    memmove(_props, &_fs->props, sizeof(framesyncprops_s));
}

void flexframesync_setprops(flexframesync _fs, framesyncprops_s * _props)
{
    // TODO : flexframesync_setprops() validate input

    if (_props->k       != _fs->props.k     ||
        _props->npfb    != _fs->props.npfb  ||
        _props->m       != _fs->props.m     ||
        _props->beta    != _fs->props.beta)
    {
        printf("warning: flexframesync_setprops(), ignoring filter change\n");
        // TODO : destroy/recreate filter
    }
    memmove(&_fs->props, _props, sizeof(framesyncprops_s));
}

void flexframesync_print(flexframesync _fs)
{
    printf("flexframesync:\n");
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
    printf("    ----\n");
    printf("    p/n sequence len    :   %u\n", _fs->pnsequence_len);
    printf("    modulation scheme   :   %u-%s\n",
        1<<(_fs->bps_payload),
        modulation_scheme_str[_fs->ms_payload]);
    printf("    payload len         :   %u bytes\n", _fs->payload_len);
    printf("    num payload symbols :   %u\n", _fs->num_payload_symbols);
}

void flexframesync_reset(flexframesync _fs)
{
    symsync_crcf_clear(_fs->mfdecim);
    //agc_crcf_set_bandwidth(_fs->agc_rx, FLEXFRAMESYNC_AGC_BW_0);
    agc_crcf_unlock(_fs->agc_rx);
    symsync_crcf_unlock(_fs->mfdecim);
    nco_crcf_reset(_fs->nco_rx);

    // SINDR estimate
    _fs->evm_hat = 0.0f;
    _fs->SINDRdB_hat = 0.0f;

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
}

// TODO: break flexframesync_execute method into manageable pieces
void flexframesync_execute(flexframesync _fs, float complex *_x, unsigned int _n)
{
    unsigned int i, j, nw;
    float complex agc_rx_out;
    float complex mfdecim_out[4];
    float complex nco_rx_out;
    float phase_error;
    //float complex rxy;
    float rxy;
    unsigned int demod_sym;

    for (i=0; i<_n; i++) {
        // agc
        agc_crcf_execute(_fs->agc_rx, _x[i], &agc_rx_out);
        _fs->rssi = agc_crcf_get_signal_level(_fs->agc_rx);
#ifdef DEBUG_FLEXFRAMESYNC
        windowcf_push(_fs->debug_x, _x[i]);
        windowf_push(_fs->debug_agc_rssi, agc_crcf_get_signal_level(_fs->agc_rx));
        windowcf_push(_fs->debug_agc_out, agc_rx_out);
#endif

        // squelch: block agc output from synchronizer only if
        // 1. received signal strength indicator has not exceeded squelch
        //    threshold at any time within the past <squelch_timeout> samples
        // 2. mode is FLEXFRAMESYNC_STATE_SEEKPN (seek p/n sequence)
        _fs->squelch_status = agc_crcf_squelch_get_status(_fs->agc_rx);
#if DEBUG_FLEXFRAMESYNC_PRINT
        if (_fs->squelch_status == LIQUID_AGC_SQUELCH_TIMEOUT)
            printf("squelch active\n");
#endif
        // if squelch is enabled, skip remaining of synchronizer
        // NOTE : if squelch is deactivated, the default status
        //        value is LIQUID_AGC_SQUELCH_SIGNALHI
        if (_fs->squelch_status == LIQUID_AGC_SQUELCH_TIMEOUT) {
            // reset on timeout (very important!)
            flexframesync_reset(_fs);
        } else if (_fs->squelch_status == LIQUID_AGC_SQUELCH_ENABLED) {
            continue;
        }

        // symbol synchronizer
        symsync_crcf_execute(_fs->mfdecim, &agc_rx_out, 1, mfdecim_out, &nw);

        for (j=0; j<nw; j++) {
            // mix down, demodulate, run PLL
            nco_crcf_mix_down(_fs->nco_rx, mfdecim_out[j], &nco_rx_out);
            if (_fs->state == FLEXFRAMESYNC_STATE_SEEKPN) {
            //if (false) {
                modem_demodulate(_fs->mod_preamble, nco_rx_out, &demod_sym);
                get_demodulator_phase_error(_fs->mod_preamble, &phase_error);
            } else if (_fs->state == FLEXFRAMESYNC_STATE_RXHEADER) {
                modem_demodulate(_fs->mod_header, nco_rx_out, &demod_sym);
                get_demodulator_phase_error(_fs->mod_header, &phase_error);
            } else {
                modem_demodulate(_fs->mod_payload, nco_rx_out, &demod_sym);
                get_demodulator_phase_error(_fs->mod_payload, &phase_error);
                phase_error *= cabsf(nco_rx_out);
            }

            //if (_fs->rssi < _fs->squelch_threshold)
            //    phase_error *= 0.01f;

            nco_crcf_pll_step(_fs->nco_rx, phase_error);
            /*
            float fmax = 0.05f;
            if (_fs->nco_rx->d_theta >  fmax) _fs->nco_rx->d_theta =  fmax;
            if (_fs->nco_rx->d_theta < -fmax) _fs->nco_rx->d_theta = -fmax;
            */
            nco_crcf_step(_fs->nco_rx);
#ifdef DEBUG_FLEXFRAMESYNC
            windowf_push(_fs->debug_nco_phase, _fs->nco_rx->theta);
            windowf_push(_fs->debug_nco_freq,  _fs->nco_rx->d_theta);
            windowcf_push(_fs->debug_nco_rx_out, nco_rx_out);
#endif
            //if (_fs->rssi < _fs->squelch_threshold)
            //    continue;

            //
            switch (_fs->state) {
            case FLEXFRAMESYNC_STATE_SEEKPN:
                //
                bsync_rrrf_correlate(_fs->fsync, nco_rx_out, &rxy);
#ifdef DEBUG_FLEXFRAMESYNC
                windowcf_push(_fs->debug_rxy, rxy);
#endif
                if (fabsf(rxy) > 0.7f) {
                    //printf("|rxy| = %8.4f, angle: %8.4f\n",cabsf(rxy),cargf(rxy));
                    // close bandwidth
                    flexframesync_close_bandwidth(_fs);
                    nco_crcf_adjust_phase(_fs->nco_rx, cargf(rxy));
                    symsync_crcf_lock(_fs->mfdecim);

                    // deactivate squelch as not to suppress signal in the
                    // middle of the frame
                    agc_crcf_squelch_deactivate(_fs->agc_rx);
                    _fs->state = FLEXFRAMESYNC_STATE_RXHEADER;
                }
                break;
            case FLEXFRAMESYNC_STATE_RXHEADER:
                //_fs->header_sym[_fs->num_symbols_collected] = (unsigned char) demod_sym;
                _fs->header_samples[_fs->num_symbols_collected] = nco_rx_out;
                _fs->num_symbols_collected++;

                // SINDR estimation
                get_demodulator_evm(_fs->mod_header, &_fs->evm);
                _fs->evm_hat += _fs->evm;
                if (_fs->num_symbols_collected==256) {
                    _fs->evm_hat /= 256.0f;
                    _fs->SINDRdB_hat = -10*log10f(_fs->evm_hat);
#if DEBUG_FLEXFRAMESYNC_PRINT
                    printf("SINDR   :   %12.8f dB\n", _fs->SINDRdB_hat);
#endif
                    _fs->num_symbols_collected = 0;
                    flexframesync_demodulate_header(_fs);
                    flexframesync_decode_header(_fs);
                    if (_fs->header_valid) {
                        agc_crcf_lock(_fs->agc_rx);
                        _fs->state = FLEXFRAMESYNC_STATE_RXPAYLOAD;
                    } else {
                        //printf("***** header invalid!\n");
                        // invoke callback anyway, but escape ignore rest of payload
                        _fs->callback(_fs->header,  _fs->header_valid,
                                      NULL,         0,
                                      _fs->userdata,
                                      NULL, 0);
                        _fs->state = FLEXFRAMESYNC_STATE_RESET;
                    }
                }
                break;
            case FLEXFRAMESYNC_STATE_RXPAYLOAD:
#ifdef DEBUG_FLEXFRAMESYNC
            windowcf_push(_fs->debug_framesyms, nco_rx_out);
#endif
            //if (_fs->rssi < _fs->squelch_threshold)
                _fs->payload_samples[_fs->num_symbols_collected] = nco_rx_out;
                _fs->payload_sym[_fs->num_symbols_collected] = (unsigned char) demod_sym;
                _fs->num_symbols_collected++;
                // TODO: fix hard-coded value
                if (_fs->num_symbols_collected==_fs->num_payload_symbols) {
                    _fs->num_symbols_collected = 0;
                    flexframesync_assemble_payload(_fs);

                    // invoke callback method
                    _fs->callback(_fs->header,  _fs->header_valid,
                                  _fs->payload, _fs->payload_len,
                                  _fs->userdata,
                                  _fs->payload_samples,
                                  _fs->num_payload_symbols);

                    _fs->state = FLEXFRAMESYNC_STATE_RESET;
//#ifdef DEBUG_FLEXFRAMESYNC
#if 0
                    printf("flexframesync exiting prematurely\n");
                    flexframesync_destroy(_fs);
                    exit(0);
#endif
                } else {
                    break;
                }
            case FLEXFRAMESYNC_STATE_RESET:
                // open bandwidth
                _fs->state = FLEXFRAMESYNC_STATE_SEEKPN;
                agc_crcf_unlock(_fs->agc_rx);
                agc_crcf_squelch_activate(_fs->agc_rx);
                symsync_crcf_unlock(_fs->mfdecim);
                flexframesync_open_bandwidth(_fs);
                _fs->num_symbols_collected = 0;
                nco_crcf_reset(_fs->nco_rx);

                // Don't actually reset the synchronizer
                //flexframesync_reset(_fs);
                break;
            default:;
            }
        }
    }
    //printf("rssi: %8.4f\n", 10*log10(agc_crcf_get_signal_level(_fs->agc_rx)));
}

// 
// internal
//

// open bandwidth of synchronizer objects (acquisition mode)
void flexframesync_open_bandwidth(flexframesync _fs)
{
    agc_crcf_set_bandwidth(_fs->agc_rx, _fs->props.agc_bw0);
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->props.sym_bw0);
    nco_crcf_pll_set_bandwidth(_fs->nco_rx, _fs->props.pll_bw0);
}

// close bandwidth of synchronizer objects (tracking mode)
void flexframesync_close_bandwidth(flexframesync _fs)
{
    agc_crcf_set_bandwidth(_fs->agc_rx, _fs->props.agc_bw1);
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->props.sym_bw1);
    nco_crcf_pll_set_bandwidth(_fs->nco_rx, _fs->props.pll_bw1);
}

void flexframesync_configure_payload_buffers(flexframesync _fs)
{
    //printf("flexframesync : payload symbols : %u\n", _fs->num_payload_symbols);

    div_t d;

    // compute payload length (symbols)
    d = div(8*_fs->payload_len, _fs->bps_payload);
    _fs->num_payload_symbols = d.quot + (d.rem ? 1 : 0);

    // required payload allocation size, considering the total number of
    // bits might not divide evenly by the modulation depth
    d = div(_fs->num_payload_symbols*_fs->bps_payload, 8);
    unsigned int payload_numalloc_req = d.quot + (d.rem ? 1 : 0);

    if (_fs->payload_numalloc < payload_numalloc_req) {
        _fs->payload_numalloc = payload_numalloc_req;
        _fs->payload = (unsigned char*) realloc(_fs->payload, _fs->payload_numalloc);
        //printf("    flexframsync: reallocating payload (payload data) : %u\n", _fs->payload_numalloc);
    }

    if (_fs->payload_sym_numalloc < _fs->num_payload_symbols) {
        _fs->payload_sym_numalloc = _fs->num_payload_symbols;
        _fs->payload_sym = (unsigned char*) realloc(_fs->payload_sym, _fs->payload_sym_numalloc);
        //printf("reallocating payload_sym (payload symbols) : %u\n", _fs->payload_sym_numalloc);
    }

    if (_fs->payload_samples_numalloc < _fs->num_payload_symbols) {
        _fs->payload_samples_numalloc = _fs->num_payload_symbols;
        _fs->payload_samples = (float complex*) realloc(_fs->payload_samples, _fs->payload_samples_numalloc*sizeof(float complex));
        //printf("reallocating payload_samples (modulated payload symbols) : %u\n",
        //        _fs->payload_samples_numalloc);
    }
}

void flexframesync_decode_header(flexframesync _fs)
{
    // run packet decoder
    _fs->header_valid =
    packetizer_decode(_fs->p_header, _fs->header_enc, _fs->header);

    // unscramble header
    unscramble_data(_fs->header, 12);

    // strip off modulation scheme/depth
    unsigned int mod_scheme = (_fs->header[11] >> 4) & 0x0f;
    unsigned int mod_depth  = (_fs->header[11]     ) & 0x0f;

    // strip off payload length
    unsigned int payload_len = (_fs->header[9] << 8) | (_fs->header[10]);
    _fs->payload_len = payload_len;

    // configure payload receiver
    if (_fs->header_valid) {
        // configure modem
        if (mod_scheme != _fs->ms_payload || mod_depth != _fs->bps_payload) {
#if DEBUG_FLEXFRAMESYNC_PRINT
            printf("flexframesync : configuring payload modem : %u-%s\n",
                    1<<mod_depth,
                    modulation_scheme_str[mod_scheme]);
#endif
            _fs->ms_payload = mod_scheme;
            _fs->bps_payload = mod_depth;
            modem_destroy(_fs->mod_payload);
            _fs->mod_payload = modem_create(_fs->ms_payload, _fs->bps_payload);
        }
        flexframesync_configure_payload_buffers(_fs);
    }
    
#if DEBUG_FLEXFRAMESYNC_PRINT
    // print results
    printf("flexframesync_decode_header():\n");
    printf("    mod scheme  : %u-%s\n", 1<<mod_depth, modulation_scheme_str[mod_scheme]);
    printf("    payload len : %u\n", payload_len);
    printf("    header key  : 0x%.8x\n", header_key);
    printf("    header crc  : %s\n", _fs->header_valid ? "pass" : "FAIL");

    printf("    user data   :");
    unsigned int i;
    for (i=0; i<9; i++)
        printf(" %.2x", _fs->header[i]);
    printf("\n");
#endif
}

void flexframesync_demodulate_header(flexframesync _fs)
{
    unsigned int i, sym;

    // run demodulator
    for (i=0; i<256; i++) {
        modem_demodulate(_fs->mod_header, _fs->header_samples[i], &sym);
        _fs->header_sym[i] = (unsigned char)sym;
    }

    // pack header symbols
    unsigned char byte;
    for (i=0; i<32; i++) {
        byte = 0;
        byte |= (_fs->header_sym[8*i+0] << 7);
        byte |= (_fs->header_sym[8*i+1] << 6);
        byte |= (_fs->header_sym[8*i+2] << 5);
        byte |= (_fs->header_sym[8*i+3] << 4);
        byte |= (_fs->header_sym[8*i+4] << 3);
        byte |= (_fs->header_sym[8*i+5] << 2);
        byte |= (_fs->header_sym[8*i+6] << 1);
        byte |= (_fs->header_sym[8*i+7]     );
        _fs->header_enc[i] = byte;
    }
}

void flexframesync_assemble_payload(flexframesync _fs)
{
    // pack (8-bit) bytes from (bps_payload-bit) symbols
    unsigned int num_written;
    repack_bytes(_fs->payload_sym, _fs->bps_payload, _fs->num_payload_symbols,
                 _fs->payload,     8,                _fs->payload_numalloc,
                 &num_written);
}


#ifdef DEBUG_FLEXFRAMESYNC
void flexframesync_output_debug_file(flexframesync _fs)
{
    unsigned int i;
    float * r;
    float complex * rc;
    FILE* fid = fopen(DEBUG_FLEXFRAMESYNC_FILENAME,"w");
    if (!fid) {
        printf("error: flexframesync_output_debug_file(), could not open file for writing\n");
        return;
    }
    fprintf(fid,"%% %s: auto-generated file", DEBUG_FLEXFRAMESYNC_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    // write agc_rssi
    fprintf(fid,"agc_rssi = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    windowf_read(_fs->debug_agc_rssi, &r);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(10*log10(agc_rssi))\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");

    // write agc out
    fprintf(fid,"agc_out = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    windowcf_read(_fs->debug_agc_out, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"agc_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(agc_out),real(agc_out), 1:length(agc_out),imag(agc_out));\n");
    fprintf(fid,"ylabel('agc-out');\n");


    // write x
    fprintf(fid,"x = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    windowcf_read(_fs->debug_x, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    // write rxy
    fprintf(fid,"rxy = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    windowcf_read(_fs->debug_rxy, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(abs(rxy))\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    // write nco_rx_out
    fprintf(fid,"nco_rx_out = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    windowcf_read(_fs->debug_nco_rx_out, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"nco_rx_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_rx_out,'x')\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid,"axis square;\n");

    // write frame symbols
    fprintf(fid,"framesyms = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    windowcf_read(_fs->debug_framesyms, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(framesyms,'x','MarkerSize',1)\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid,"axis square;\n");

    // write nco_phase
    fprintf(fid,"nco_phase = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    windowf_read(_fs->debug_nco_phase, &r);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"nco_phase(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_phase)\n");
    fprintf(fid,"ylabel('nco phase [radians]');\n");

    // write nco_freq
    fprintf(fid,"nco_freq = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    windowf_read(_fs->debug_nco_freq, &r);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"nco_freq(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_freq)\n");
    fprintf(fid,"ylabel('nco freq');\n");


    fprintf(fid,"\n\n");
    fclose(fid);

    printf("flexframesync/debug: results written to %s\n", DEBUG_FLEXFRAMESYNC_FILENAME);

    // clean up debug windows
    windowf_destroy(_fs->debug_agc_rssi);
    windowcf_destroy(_fs->debug_agc_out);
    windowcf_destroy(_fs->debug_rxy);
    windowcf_destroy(_fs->debug_x);
    windowcf_destroy(_fs->debug_nco_rx_out);
    windowcf_destroy(_fs->debug_framesyms);
}
#endif
