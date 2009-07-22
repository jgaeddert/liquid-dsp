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
#include <complex.h>

#include "liquid.internal.h"

#define FLEXFRAMESYNC_SYM_BW_0          (0.01f)
#define FLEXFRAMESYNC_SYM_BW_1          (0.001f)

#define FLEXFRAMESYNC_AGC_BW_0          (3e-3f)
#define FLEXFRAMESYNC_AGC_BW_1          (1e-5f)

#define FLEXFRAMESYNC_PLL_BW_0          (2e-3f)
#define FLEXFRAMESYNC_PLL_BW_1          (1e-3f)

#define FLEXFRAMESYNC_SQUELCH_THRESH    (-15.0f)
#define FLEXFRAMESYNC_SQUELCH_TIMEOUT   (32)

#define FLEXFRAMESYNC_PN_LEN            (64)


#define DEBUG_FLEXFRAMESYNC             1
#define DEBUG_FLEXFRAMESYNC_PRINT       0
#define DEBUG_FLEXFRAMESYNC_FILENAME    "flexframesync_internal_debug.m"
#define DEBUG_FLEXFRAMESYNC_BUFFER_LEN  (4096)

struct flexframesync_s {

    // synchronizer objects
    agc agc_rx;
    symsync_crcf mfdecim;
    pll pll_rx;
    nco nco_rx;
    bsync_rrrf fsync;

    //
    float rssi;
    float squelch_threshold;
    unsigned int squelch_timeout;
    unsigned int squelch_timer;

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
    fec fec_header;
    interleaver intlv_header;
    float complex header_samples[128];
    unsigned char header_sym[128];
    unsigned char header_enc[32];
    unsigned char header[15];

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
    flexframesyncprops_s props;

    // callback
    flexframesync_callback callback;
    void * userdata;

#ifdef DEBUG_FLEXFRAMESYNC
    FILE*fid;
    fwindow  debug_agc_rssi;
    cfwindow debug_agc_out;
    cfwindow debug_x;
    cfwindow debug_rxy;
    cfwindow debug_nco_rx_out;
    fwindow  debug_nco_phase;
    fwindow  debug_nco_freq;
#endif
};

flexframesync flexframesync_create(flexframesyncprops_s * _props,
                                   flexframesync_callback _callback,
                                   void * _userdata)
{
    flexframesync fs = (flexframesync) malloc(sizeof(struct flexframesync_s));
    fs->callback = _callback;
    fs->userdata = _userdata;

    // set default properties
    // TODO : read properties from user input
    flexframesync_set_default_props(fs);

    // header objects
    fs->fec_header = fec_create(FEC_CONV_V29, NULL);
    fs->mod_header = modem_create(MOD_QPSK, 2);
    fs->intlv_header = interleaver_create(32, INT_BLOCK);

    // agc, rssi, squelch
    fs->agc_rx = agc_create(1.0f, FLEXFRAMESYNC_AGC_BW_0);
    agc_set_gain_limits(fs->agc_rx, 1e-6, 1e2);
    fs->squelch_threshold = FLEXFRAMESYNC_SQUELCH_THRESH;
    fs->squelch_timeout = FLEXFRAMESYNC_SQUELCH_TIMEOUT;
    fs->squelch_timer = fs->squelch_timeout;

    // pll, nco
    fs->pll_rx = pll_create();
    fs->nco_rx = nco_create();
    pll_set_bandwidth(fs->pll_rx, FLEXFRAMESYNC_PLL_BW_1);
    pll_set_damping_factor(fs->pll_rx, 4.0f);   // increasing damping factor
                                                // reduces oscillations,
                                                // improves stability

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
    unsigned int H_len = 2*2*npfb*3 + 1;// 2*2*npfb*_m + 1;
    float H[H_len];
    design_rrc_filter(2*npfb,3,0.7f,0,H);
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

    // open bandwidth
    flexframesync_open_bandwidth(fs);

#ifdef DEBUG_FLEXFRAMESYNC
    fs->debug_agc_rssi  =  fwindow_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_agc_out   = cfwindow_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_x         = cfwindow_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_rxy       = cfwindow_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_rx_out= cfwindow_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_phase=   fwindow_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_freq =   fwindow_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
#endif

    return fs;
}

void flexframesync_destroy(flexframesync _fs)
{
    // destroy synchronizer objects
    agc_destroy(_fs->agc_rx);
    pll_destroy(_fs->pll_rx);
    nco_destroy(_fs->nco_rx);
    bsync_rrrf_destroy(_fs->fsync);
    symsync_crcf_destroy(_fs->mfdecim);

    // destroy preamble objects
    modem_destroy(_fs->mod_preamble);

    // destroy header objects
    fec_destroy(_fs->fec_header);
    modem_destroy(_fs->mod_header);
    interleaver_destroy(_fs->intlv_header);

    // free payload objects
    modem_destroy(_fs->mod_payload);
    free(_fs->payload_samples);
    free(_fs->payload_sym);
    free(_fs->payload);

#ifdef DEBUG_FLEXFRAMESYNC
    unsigned int i;
    float * r;
    float complex * rc;
    FILE* fid = fopen(DEBUG_FLEXFRAMESYNC_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file", DEBUG_FLEXFRAMESYNC_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    // write agc_rssi
    fprintf(fid,"agc_rssi = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fwindow_read(_fs->debug_agc_rssi, &r);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(10*log10(agc_rssi))\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");

    // write agc out
    fprintf(fid,"agc_out = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    cfwindow_read(_fs->debug_agc_out, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"agc_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(agc_out),real(agc_out), 1:length(agc_out),imag(agc_out));\n");
    fprintf(fid,"ylabel('agc-out');\n");


    // write x
    fprintf(fid,"x = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    cfwindow_read(_fs->debug_x, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    // write rxy
    fprintf(fid,"rxy = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    cfwindow_read(_fs->debug_rxy, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(abs(rxy))\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    // write nco_rx_out
    fprintf(fid,"nco_rx_out = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    cfwindow_read(_fs->debug_nco_rx_out, &rc);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"nco_rx_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_rx_out,'x')\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1.2 1.2 -1.2 1.2]);\n");

    // write nco_phase
    fprintf(fid,"nco_phase = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fwindow_read(_fs->debug_nco_phase, &r);
    for (i=0; i<DEBUG_FLEXFRAMESYNC_BUFFER_LEN; i++)
        fprintf(fid,"nco_phase(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_phase)\n");
    fprintf(fid,"ylabel('nco phase [radians]');\n");

    // write nco_freq
    fprintf(fid,"nco_freq = zeros(1,%u);\n", DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fwindow_read(_fs->debug_nco_freq, &r);
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
    fwindow_destroy(_fs->debug_agc_rssi);
    cfwindow_destroy(_fs->debug_agc_out);
    cfwindow_destroy(_fs->debug_rxy);
    cfwindow_destroy(_fs->debug_x);
    cfwindow_destroy(_fs->debug_nco_rx_out);
#endif
    free(_fs);
}

void flexframesync_print(flexframesync _fs)
{
    printf("flexframesync:\n");
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
    pll_reset(_fs->pll_rx);
    //agc_set_bandwidth(_fs->agc_rx, FLEXFRAMESYNC_AGC_BW_0);
    nco_reset(_fs->nco_rx);
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
        agc_execute(_fs->agc_rx, _x[i], &agc_rx_out);
        _fs->rssi = 10*log10(agc_get_signal_level(_fs->agc_rx));
#ifdef DEBUG_FLEXFRAMESYNC
        cfwindow_push(_fs->debug_x, _x[i]);
        fwindow_push(_fs->debug_agc_rssi, agc_get_signal_level(_fs->agc_rx));
        cfwindow_push(_fs->debug_agc_out, agc_rx_out);
#endif

        // squelch: block agc output from synchronizer only if
        // 1. received signal strength indicator has not exceeded squelch
        //    threshold at any time within the past <squelch_timeout> samples
        // 2. mode is FLEXFRAMESYNC_STATE_SEEKPN (seek p/n sequence)
        if (_fs->rssi < _fs->squelch_threshold &&
            _fs->state == FLEXFRAMESYNC_STATE_SEEKPN)
        {
            if (_fs->squelch_timer > 1) {
                // signal low, but we haven't reached timout yet; decrement
                // counter and continue
                _fs->squelch_timer--;
            } else if (_fs->squelch_timer == 1) {
                // squelch timeout: signal has been too low for too long

                //printf("squelch enabled\n");
                _fs->squelch_timer = 0;
                flexframesync_reset(_fs);
                continue;
            } else {
                // squelch enabled: ignore sample (wait for high signal)
                continue;
            }
        } else {
            // signal high: reset timer and continue
            _fs->squelch_timer = _fs->squelch_timeout;
        }

        // symbol synchronizer
        symsync_crcf_execute(_fs->mfdecim, &agc_rx_out, 1, mfdecim_out, &nw);

        for (j=0; j<nw; j++) {
            // mix down, demodulate, run PLL
            nco_mix_down(_fs->nco_rx, mfdecim_out[j], &nco_rx_out);
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
            }

            //if (_fs->rssi < _fs->squelch_threshold)
            //    phase_error *= 0.01f;

            pll_step(_fs->pll_rx, _fs->nco_rx, phase_error);
            /*
            float fmax = 0.05f;
            if (_fs->nco_rx->d_theta >  fmax) _fs->nco_rx->d_theta =  fmax;
            if (_fs->nco_rx->d_theta < -fmax) _fs->nco_rx->d_theta = -fmax;
            */
            nco_step(_fs->nco_rx);
#ifdef DEBUG_FLEXFRAMESYNC
            fwindow_push(_fs->debug_nco_phase, _fs->nco_rx->theta);
            fwindow_push(_fs->debug_nco_freq,  _fs->nco_rx->d_theta);
            cfwindow_push(_fs->debug_nco_rx_out, nco_rx_out);
#endif
            //if (_fs->rssi < _fs->squelch_threshold)
            //    continue;

            //
            switch (_fs->state) {
            case FLEXFRAMESYNC_STATE_SEEKPN:
                //
                bsync_rrrf_correlate(_fs->fsync, nco_rx_out, &rxy);
#ifdef DEBUG_FLEXFRAMESYNC
                cfwindow_push(_fs->debug_rxy, rxy);
#endif
                if (fabsf(rxy) > 0.7f) {
                    //printf("|rxy| = %8.4f, angle: %8.4f\n",cabsf(rxy),cargf(rxy));
                    // close bandwidth
                    flexframesync_close_bandwidth(_fs);
                    nco_adjust_phase(_fs->nco_rx, cargf(rxy));
                    _fs->state = FLEXFRAMESYNC_STATE_RXHEADER;
                }
                break;
            case FLEXFRAMESYNC_STATE_RXHEADER:
                //_fs->header_sym[_fs->num_symbols_collected] = (unsigned char) demod_sym;
                _fs->header_samples[_fs->num_symbols_collected] = nco_rx_out;
                _fs->num_symbols_collected++;
                if (_fs->num_symbols_collected==128) {
                    _fs->num_symbols_collected = 0;
                    flexframesync_demodulate_header(_fs);
                    flexframesync_decode_header(_fs);
                    if (_fs->header_valid) {
                        _fs->state = FLEXFRAMESYNC_STATE_RXPAYLOAD;
                    } else {
                        printf("***** header invalid!\n");
                        // TODO : invoke callback anyway?
                        _fs->state = FLEXFRAMESYNC_STATE_RESET;
                    }
                }
                break;
            case FLEXFRAMESYNC_STATE_RXPAYLOAD:
                _fs->payload_sym[_fs->num_symbols_collected] = (unsigned char) demod_sym;
                _fs->num_symbols_collected++;
                // TODO: fix hard-coded value
                if (_fs->num_symbols_collected==_fs->num_payload_symbols) {
                    _fs->num_symbols_collected = 0;
                    flexframesync_assemble_payload(_fs);

                    // invoke callback method
                    _fs->callback(_fs->header,  _fs->header_valid,
                                  _fs->payload, _fs->payload_len,
                                  _fs->userdata);

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
                flexframesync_open_bandwidth(_fs);
                _fs->num_symbols_collected = 0;

                //flexframesync_reset(_fs);
                break;
            default:;
            }
        }
    }
    //printf("rssi: %8.4f\n", 10*log10(agc_get_signal_level(_fs->agc_rx)));
}

// 
// internal
//

// open bandwidth of synchronizer objects (acquisition mode)
void flexframesync_open_bandwidth(flexframesync _fs)
{
    agc_set_bandwidth(_fs->agc_rx, _fs->props.agc_bw0);
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->props.sym_bw0);
    pll_set_bandwidth(_fs->pll_rx, _fs->props.pll_bw0);
}

// close bandwidth of synchronizer objects (tracking mode)
void flexframesync_close_bandwidth(flexframesync _fs)
{
    agc_set_bandwidth(_fs->agc_rx, _fs->props.agc_bw1);
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->props.sym_bw1);
    pll_set_bandwidth(_fs->pll_rx, _fs->props.pll_bw1);
}

// set default user-configurable properties
void flexframesync_set_default_props(flexframesync _fs)
{
    _fs->props.agc_bw0 = FLEXFRAMESYNC_AGC_BW_0;
    _fs->props.agc_bw1 = FLEXFRAMESYNC_AGC_BW_1;

    _fs->props.sym_bw0 = FLEXFRAMESYNC_SYM_BW_0;
    _fs->props.sym_bw1 = FLEXFRAMESYNC_SYM_BW_1;

    _fs->props.pll_bw0 = FLEXFRAMESYNC_PLL_BW_0;
    _fs->props.pll_bw1 = FLEXFRAMESYNC_PLL_BW_1;

    // symbol timing recovery
    _fs->props.k = 2;       // samples per symbol
    _fs->props.npfb = 32;   // number of filters in filter-bank
    _fs->props.m = 3;       // filter length
    _fs->props.beta = 0.7f; // excess bandwidth factor

    _fs->props.squelch_threshold = FLEXFRAMESYNC_SQUELCH_THRESH;
}

void flexframesync_configure_payload_buffers(flexframesync _fs)
{
    //printf("flexframesync : payload symbols : %u\n", _fs->num_payload_symbols);

    // compute payload length (symbols)
    _fs->num_payload_symbols = 8*(_fs->payload_len);
    _fs->num_payload_symbols /= _fs->bps_payload;
    _fs->num_payload_symbols += _fs->num_payload_symbols % _fs->bps_payload;

    // required payload allocation size, considering the total number of
    // bits might not divide evenly by the modulation depth
    unsigned int payload_numalloc_req;
    payload_numalloc_req  = _fs->num_payload_symbols * _fs->bps_payload;
    payload_numalloc_req /= 8;
    payload_numalloc_req += payload_numalloc_req % _fs->bps_payload;

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
    // de-interleave
    interleaver_deinterleave(_fs->intlv_header, _fs->header_enc, _fs->header_enc);

    // run decoder
    fec_decode(_fs->fec_header, 15, _fs->header_enc, _fs->header);

    // unscramble header
    unscramble_data(_fs->header, 15);

    // strip off crc32
    unsigned int header_key=0;
    header_key |= ( _fs->header[11] << 24 );
    header_key |= ( _fs->header[12] << 16 );
    header_key |= ( _fs->header[13] <<  8 );
    header_key |= ( _fs->header[14]       );
    _fs->header_key = header_key;

    // validate crc
    _fs->header_valid = crc32_validate_message(_fs->header,11,_fs->header_key);

    // strip off modulation scheme/depth
    unsigned int mod_scheme = (_fs->header[10] >> 4) & 0x0f;
    unsigned int mod_depth  = (_fs->header[10]     ) & 0x0f;

    // strip off payload length
    unsigned int payload_len = (_fs->header[8] << 8) | (_fs->header[9]);
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
    for (i=0; i<8; i++)
        printf(" %.2x", _fs->header[i]);
    printf("\n");
#endif
}

void flexframesync_demodulate_header(flexframesync _fs)
{
    unsigned int i, sym;

    // run demodulator
    for (i=0; i<128; i++) {
        modem_demodulate(_fs->mod_header, _fs->header_samples[i], &sym);
        _fs->header_sym[i] = (unsigned char)sym;
    }

    // pack header symbols
    unsigned char byte;
    for (i=0; i<32; i++) {
        byte = 0;
        byte |= (_fs->header_sym[4*i+0] << 6);
        byte |= (_fs->header_sym[4*i+1] << 4);
        byte |= (_fs->header_sym[4*i+2] << 2);
        byte |= (_fs->header_sym[4*i+3]     );
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


