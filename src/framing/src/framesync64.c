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

#include "liquid.internal.h"

#define FRAMESYNC64_SYM_BW_0        (0.08f)
#define FRAMESYNC64_SYM_BW_1        (0.05f)

#define FRAMESYNC64_AGC_BW_0        (3e-3f)
#define FRAMESYNC64_AGC_BW_1        (1e-5f)

#define FRAMESYNC64_PLL_BW_0        (2e-3f)
#define FRAMESYNC64_PLL_BW_1        (1e-4f)

#define FRAMESYNC64_SQUELCH_THRESH  (-25.0f)
#define FRAMESYNC64_SQUELCH_TIMEOUT (32)

#define FRAME64_PN_LEN              (64)

#define DEBUG_FRAMESYNC64           1
#define DEBUG_FRAMESYNC64_PRINT     0
#define DEBUG_FILENAME              "framesync64_internal_debug.m"
#define DEBUG_BUFFER_LEN            (4096)

// Internal
void framesync64_open_bandwidth(framesync64 _fs);
void framesync64_close_bandwidth(framesync64 _fs);
void framesync64_decode_header(framesync64 _fs);
void framesync64_decode_payload(framesync64 _fs);

void framesync64_syms_to_byte(unsigned char * _syms, unsigned char * _byte);

void framesync64_debug_print(framesync64 _fs);

struct framesync64_s {
    modem demod;
    modem bpsk;
    interleaver intlv;
    fec dec;

    // synchronizer objects
    agc_crcf agc_rx;
    symsync_crcf mfdecim;
    pll pll_rx;
    nco nco_rx;
    bsync_rrrf fsync;

    // synchronizer bandwidths
    float agc_bw0, agc_bw1; // automatic gain control
    float pll_bw0, pll_bw1; // phase-locked loop
    float sym_bw0, sym_bw1; // symbol timing recovery

    // squelch
    float rssi;     // received signal strength indicator [dB]
    int squelch_status;

    // status variables
    enum {
        FRAMESYNC64_STATE_SEEKPN=0,
        FRAMESYNC64_STATE_RXHEADER,
        FRAMESYNC64_STATE_RXPAYLOAD,
        FRAMESYNC64_STATE_RESET
    } state;
    unsigned int num_symbols_collected;
    unsigned int header_key;
    unsigned int payload_key;
    bool header_valid;
    bool payload_valid;

    framesync64_callback callback;
    void * userdata;

    // header
    unsigned char header_sym[256];
    unsigned char header_enc[64];
    unsigned char header[32];

    // payload
    unsigned char payload_sym[512];
    unsigned char payload_enc[128];
    unsigned char payload_intlv[128];
    unsigned char payload[64];

#if DEBUG_FRAMESYNC64
    FILE*fid;
    fwindow  debug_agc_rssi;
    cfwindow debug_agc_out;
    cfwindow debug_x;
    cfwindow debug_rxy;
    cfwindow debug_nco_rx_out;
    cfwindow debug_framesyms;
    fwindow  debug_nco_phase;
    fwindow  debug_nco_freq;
#endif
};

framesync64 framesync64_create(
//    unsigned int _k,
    unsigned int _m,
    float _beta,
    framesync64_callback _callback,
    void * _userdata)
{
    framesync64 fs = (framesync64) malloc(sizeof(struct framesync64_s));
    fs->callback = _callback;
    fs->userdata = _userdata;

    // agc, rssi, squelch
    fs->agc_rx = agc_crcf_create();
    agc_crcf_set_target(fs->agc_rx, 1.0f);
    agc_crcf_set_bandwidth(fs->agc_rx, FRAMESYNC64_AGC_BW_0);
    agc_crcf_set_gain_limits(fs->agc_rx, 1e-6, 1e4);

    agc_crcf_squelch_activate(fs->agc_rx);
    agc_crcf_squelch_set_threshold(fs->agc_rx, FRAMESYNC64_SQUELCH_THRESH);
    agc_crcf_squelch_set_timeout(fs->agc_rx, FRAMESYNC64_SQUELCH_TIMEOUT);

    agc_crcf_squelch_enable_auto(fs->agc_rx);
    fs->squelch_status = LIQUID_AGC_SQUELCH_SIGNALHI;

    // pll, nco
    fs->pll_rx = pll_create();
    fs->nco_rx = nco_create(LIQUID_VCO);
    pll_set_bandwidth(fs->pll_rx, FRAMESYNC64_PLL_BW_0);
    pll_set_damping_factor(fs->pll_rx, 4.0f);   // increasing damping factor
                                                // reduces oscillations,
                                                // improves stability

    // bsync (p/n synchronizer)
    unsigned int i;
    msequence ms = msequence_create(6);
    float pn_sequence[FRAME64_PN_LEN];
    for (i=0; i<FRAME64_PN_LEN; i++)
        pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    fs->fsync = bsync_rrrf_create(FRAME64_PN_LEN, pn_sequence);
    msequence_destroy(ms);

    // design symsync (k=2)
    unsigned int npfb = 32;
    unsigned int H_len = 2*2*npfb*_m + 1;
    float H[H_len];
    design_rrc_filter(2*npfb,_m,_beta,0,H);
    fs->mfdecim =  symsync_crcf_create(2, npfb, H, H_len-1);

    // create (de)interleaver
    fs->intlv = interleaver_create(128, INT_BLOCK);

    // create decoder
    fs->dec = fec_create(FEC_HAMMING74, NULL);

    // create demod
    fs->demod = modem_create(MOD_QPSK, 2);
    fs->bpsk = modem_create(MOD_BPSK, 1);

    // set status flags
    fs->state = FRAMESYNC64_STATE_SEEKPN;
    fs->num_symbols_collected = 0;

    // set open/closed bandwidth values
    framesync64_set_agc_bw0(fs,FRAMESYNC64_AGC_BW_0);
    framesync64_set_agc_bw1(fs,FRAMESYNC64_AGC_BW_1);
    framesync64_set_pll_bw0(fs,FRAMESYNC64_PLL_BW_0);
    framesync64_set_pll_bw1(fs,FRAMESYNC64_PLL_BW_1);
    framesync64_set_sym_bw0(fs,FRAMESYNC64_SYM_BW_0);
    framesync64_set_sym_bw1(fs,FRAMESYNC64_SYM_BW_1);
    framesync64_set_squelch_threshold(fs,FRAMESYNC64_SQUELCH_THRESH);

    // open bandwidth
    framesync64_open_bandwidth(fs);

#if DEBUG_FRAMESYNC64
    fs->debug_agc_rssi  =  fwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_agc_out   = cfwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_x         = cfwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_rxy       = cfwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_nco_rx_out= cfwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_framesyms = cfwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_nco_phase=   fwindow_create(DEBUG_BUFFER_LEN);
    fs->debug_nco_freq =   fwindow_create(DEBUG_BUFFER_LEN);
#endif

    return fs;
}

void framesync64_destroy(framesync64 _fs)
{
#if DEBUG_FRAMESYNC64
    framesync64_debug_print(_fs);
#endif

    symsync_crcf_destroy(_fs->mfdecim);
    fec_destroy(_fs->dec);
    interleaver_destroy(_fs->intlv);
    agc_crcf_destroy(_fs->agc_rx);
    pll_destroy(_fs->pll_rx);
    nco_destroy(_fs->nco_rx);
    bsync_rrrf_destroy(_fs->fsync);
    modem_destroy(_fs->bpsk);
    modem_destroy(_fs->demod);

    free(_fs);
}

void framesync64_print(framesync64 _fs)
{
    printf("framesync64:\n");
}

void framesync64_reset(framesync64 _fs)
{
    symsync_crcf_clear(_fs->mfdecim);
    pll_reset(_fs->pll_rx);
    agc_crcf_set_bandwidth(_fs->agc_rx, FRAMESYNC64_AGC_BW_0);
    nco_set_phase(_fs->nco_rx, 0.0f);
    nco_set_frequency(_fs->nco_rx, 0.0f);
}

// TODO: break framesync64_execute method into manageable pieces
void framesync64_execute(framesync64 _fs, float complex *_x, unsigned int _n)
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
        _fs->rssi = 10*log10(agc_crcf_get_signal_level(_fs->agc_rx));
#if DEBUG_FRAMESYNC64
        cfwindow_push(_fs->debug_x, _x[i]);
        fwindow_push(_fs->debug_agc_rssi, agc_crcf_get_signal_level(_fs->agc_rx));
        cfwindow_push(_fs->debug_agc_out, agc_rx_out);
#endif

        // squelch: block agc output from synchronizer only if
        // 1. received signal strength indicator has not exceeded squelch
        //    threshold at any time within the past <squelch_timeout> samples
        // 2. mode is FLEXFRAMESYNC_STATE_SEEKPN (seek p/n sequence)
        _fs->squelch_status = agc_crcf_squelch_get_status(_fs->agc_rx);
#if DEBUG_FRAMESYNC64_PRINT
        if (_fs->squelch_status == LIQUID_AGC_SQUELCH_TIMEOUT)
            printf("squelch active\n");
#endif
        // if squelch is enabled, skip remaining of synchronizer
        // NOTE : if squelch is deactivated, the default status
        //        value is LIQUID_AGC_SQUELCH_SIGNALHI
        if (_fs->squelch_status == LIQUID_AGC_SQUELCH_ENABLED)
            continue;

        // symbol synchronizer
        symsync_crcf_execute(_fs->mfdecim, &agc_rx_out, 1, mfdecim_out, &nw);

        for (j=0; j<nw; j++) {
            // mix down, demodulate, run PLL
            nco_mix_down(_fs->nco_rx, mfdecim_out[j], &nco_rx_out);
            if (_fs->state == FRAMESYNC64_STATE_SEEKPN) {
            //if (false) {
                modem_demodulate(_fs->bpsk, nco_rx_out, &demod_sym);
                get_demodulator_phase_error(_fs->bpsk, &phase_error);
            } else {
                modem_demodulate(_fs->demod, nco_rx_out, &demod_sym);
                get_demodulator_phase_error(_fs->demod, &phase_error);
            }

            pll_step(_fs->pll_rx, _fs->nco_rx, phase_error);
            /*
            float fmax = 0.05f;
            if (_fs->nco_rx->d_theta >  fmax) _fs->nco_rx->d_theta =  fmax;
            if (_fs->nco_rx->d_theta < -fmax) _fs->nco_rx->d_theta = -fmax;
            */
            nco_step(_fs->nco_rx);
#if DEBUG_FRAMESYNC64
            fwindow_push(_fs->debug_nco_phase, _fs->nco_rx->theta);
            fwindow_push(_fs->debug_nco_freq,  _fs->nco_rx->d_theta);
            cfwindow_push(_fs->debug_nco_rx_out, nco_rx_out);
#endif

            //
            switch (_fs->state) {
            case FRAMESYNC64_STATE_SEEKPN:
                //
                bsync_rrrf_correlate(_fs->fsync, nco_rx_out, &rxy);
#if DEBUG_FRAMESYNC64
                cfwindow_push(_fs->debug_rxy, rxy);
#endif
                if (fabsf(rxy) > 0.7f) {
                    //printf("|rxy| = %8.4f, angle: %8.4f\n",cabsf(rxy),cargf(rxy));
                    // close bandwidth
                    framesync64_close_bandwidth(_fs);
                    nco_adjust_phase(_fs->nco_rx, M_PI - cargf(rxy));

                    // deactivate squelch as not to suppress signal in the
                    // middle of the frame
                    agc_crcf_squelch_deactivate(_fs->agc_rx);
                    _fs->state = FRAMESYNC64_STATE_RXHEADER;
                }
                break;
            case FRAMESYNC64_STATE_RXHEADER:
                _fs->header_sym[_fs->num_symbols_collected] = (unsigned char) demod_sym;
                _fs->num_symbols_collected++;
                if (_fs->num_symbols_collected==256) {
                    _fs->num_symbols_collected = 0;
                    _fs->state = FRAMESYNC64_STATE_RXPAYLOAD;
                    framesync64_decode_header(_fs);
                }
                break;
            case FRAMESYNC64_STATE_RXPAYLOAD:
#if DEBUG_FRAMESYNC64
                cfwindow_push(_fs->debug_framesyms, nco_rx_out);
#endif
                _fs->payload_sym[_fs->num_symbols_collected] = (unsigned char) demod_sym;
                _fs->num_symbols_collected++;
                if (_fs->num_symbols_collected==512) {
                    _fs->num_symbols_collected = 0;
                    framesync64_decode_payload(_fs);

                    // invoke callback method
                    _fs->callback(_fs->header,  _fs->header_valid,
                                  _fs->payload, _fs->payload_valid,
                                  _fs->userdata);

                    _fs->state = FRAMESYNC64_STATE_RESET;
                    //_fs->state = FRAMESYNC64_STATE_SEEKPN;
//#if DEBUG_FRAMESYNC64
#if 0
                    printf("framesync64 exiting prematurely\n");
                    framesync64_destroy(_fs);
                    exit(0);
#endif
                }
                break;
            case FRAMESYNC64_STATE_RESET:
                // open bandwidth
                framesync64_open_bandwidth(_fs);
                agc_crcf_squelch_activate(_fs->agc_rx);
                _fs->state = FRAMESYNC64_STATE_SEEKPN;
                _fs->num_symbols_collected = 0;

                _fs->nco_rx->theta=0.0f;
                _fs->nco_rx->d_theta=0.0f;
                pll_reset(_fs->pll_rx);
                break;
            default:;
            }
        }
    }
    //printf("rssi: %8.4f\n", 10*log10(agc_get_signal_level(_fs->agc_rx)));
}

void framesync64_set_agc_bw0(framesync64 _fs, float _agc_bw0) { _fs->agc_bw0 = _agc_bw0;}
void framesync64_set_agc_bw1(framesync64 _fs, float _agc_bw1) { _fs->agc_bw1 = _agc_bw1;}
void framesync64_set_pll_bw0(framesync64 _fs, float _pll_bw0) { _fs->pll_bw0 = _pll_bw0;}
void framesync64_set_pll_bw1(framesync64 _fs, float _pll_bw1) { _fs->pll_bw1 = _pll_bw1;}
void framesync64_set_sym_bw0(framesync64 _fs, float _sym_bw0) { _fs->sym_bw0 = _sym_bw0;}
void framesync64_set_sym_bw1(framesync64 _fs, float _sym_bw1) { _fs->sym_bw1 = _sym_bw1;}
void framesync64_set_squelch_threshold(framesync64 _fs, float _squelch_threshold)
{
    agc_crcf_squelch_set_threshold(_fs->agc_rx, _squelch_threshold);
}

// 
// internal
//

void framesync64_open_bandwidth(framesync64 _fs)
{
    agc_crcf_set_bandwidth(_fs->agc_rx, _fs->agc_bw0);
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->sym_bw0);
    pll_set_bandwidth(_fs->pll_rx, _fs->pll_bw0);
}

void framesync64_close_bandwidth(framesync64 _fs)
{
    agc_crcf_set_bandwidth(_fs->agc_rx, _fs->agc_bw1);
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->sym_bw1);
    pll_set_bandwidth(_fs->pll_rx, _fs->pll_bw1);
}

void framesync64_decode_header(framesync64 _fs)
{
    unsigned int i;
    for (i=0; i<64; i++)
        framesync64_syms_to_byte(_fs->header_sym+(4*i), _fs->header_enc+i);

#if DEBUG_FRAMESYNC64_PRINT
    printf("header ENCODED (rx):\n");
    for (i=0; i<64; i++) {
        printf("%2x ", _fs->header_enc[i]);
        if (!((i+1)%16)) printf("\n");
    }
    printf("\n");
#endif

    // decode header
    fec_decode(_fs->dec, 32, _fs->header_enc, _fs->header);

    // unscramble header data
    unscramble_data(_fs->header, 32);

#if DEBUG_FRAMESYNC64_PRINT
    printf("header (rx):\n");
    for (i=0; i<32; i++) {
        printf("%2x ", _fs->header[i]);
        if (!((i+1)%8)) printf("\n");
    }
    printf("\n");
#endif

    // strip off crc32
    unsigned int header_key=0;
    header_key |= ( _fs->header[28] << 24 );
    header_key |= ( _fs->header[29] << 16 );
    header_key |= ( _fs->header[30] <<  8 );
    header_key |= ( _fs->header[31]       );
    _fs->header_key = header_key;
    //printf("rx: header_key:  0x%8x\n", header_key);

    // strip off crc32
    unsigned int payload_key=0;
    payload_key |= ( _fs->header[24] << 24 );
    payload_key |= ( _fs->header[25] << 16 );
    payload_key |= ( _fs->header[26] <<  8 );
    payload_key |= ( _fs->header[27]       );
    _fs->payload_key = payload_key;
    //printf("rx: payload_key: 0x%8x\n", payload_key);

    // validate crc
    _fs->header_valid = crc32_validate_message(_fs->header,28,_fs->header_key);
}

void framesync64_decode_payload(framesync64 _fs)
{
    unsigned int i;
    for (i=0; i<128; i++)
        framesync64_syms_to_byte(&(_fs->payload_sym[4*i]), &(_fs->payload_intlv[i]));

    // deinterleave payload
    interleaver_deinterleave(_fs->intlv, _fs->payload_intlv, _fs->payload_enc);
    
    // decode payload
    fec_decode(_fs->dec, 64, _fs->payload_enc, _fs->payload);

    // unscramble payload data
    unscramble_data(_fs->payload, 64);

    // validate crc
    _fs->payload_valid = crc32_validate_message(_fs->payload,64,_fs->payload_key);

#if DEBUG_FRAMESYNC64_PRINT
    printf("payload (rx):\n");
    for (i=0; i<64; i++) {
        printf("%2x ", _fs->payload[i]);
        if (!((i+1)%8)) printf("\n");
    }
    printf("\n");
#endif

}

void framesync64_syms_to_byte(unsigned char * _syms, unsigned char * _byte)
{
    unsigned char b=0;
    b |= (_syms[0] << 6) & 0xc0;
    b |= (_syms[1] << 4) & 0x30;
    b |= (_syms[2] << 2) & 0x0c;
    b |= (_syms[3]     ) & 0x03;
    *_byte = b;
}

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
    fwindow_read(_fs->debug_agc_rssi, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"agc_rssi(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(10*log10(agc_rssi))\n");
    fprintf(fid,"ylabel('RSSI [dB]');\n");

    // write agc out
    fprintf(fid,"agc_out = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    cfwindow_read(_fs->debug_agc_out, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"agc_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(agc_out),real(agc_out), 1:length(agc_out),imag(agc_out));\n");
    fprintf(fid,"ylabel('agc-out');\n");


    // write x
    fprintf(fid,"x = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    cfwindow_read(_fs->debug_x, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),real(x), 1:length(x),imag(x));\n");
    fprintf(fid,"ylabel('received signal, x');\n");

    // write rxy
    fprintf(fid,"rxy = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    cfwindow_read(_fs->debug_rxy, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"rxy(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(abs(rxy))\n");
    fprintf(fid,"ylabel('|r_{xy}|');\n");

    // write nco_rx_out
    fprintf(fid,"nco_rx_out = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    cfwindow_read(_fs->debug_nco_rx_out, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"nco_rx_out(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_rx_out,'x')\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1.2 1.2 -1.2 1.2]);\n");

    // write framesyms
    fprintf(fid,"framesyms = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    cfwindow_read(_fs->debug_framesyms, &rc);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"framesyms(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(framesyms,'x','MarkerSize',1)\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"title('Frame Symbols');\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"axis([-1.2 1.2 -1.2 1.2]);\n");

    // write nco_phase
    fprintf(fid,"nco_phase = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    fwindow_read(_fs->debug_nco_phase, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"nco_phase(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_phase)\n");
    fprintf(fid,"ylabel('nco phase [radians]');\n");

    // write nco_freq
    fprintf(fid,"nco_freq = zeros(1,%u);\n", DEBUG_BUFFER_LEN);
    fwindow_read(_fs->debug_nco_freq, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"nco_freq(%4u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(nco_freq)\n");
    fprintf(fid,"ylabel('nco freq');\n");


    fprintf(fid,"\n\n");
    fclose(fid);

    printf("framesync64/debug: results written to %s\n", DEBUG_FILENAME);

    // clean up debug windows
    fwindow_destroy(_fs->debug_agc_rssi);
    cfwindow_destroy(_fs->debug_agc_out);
    cfwindow_destroy(_fs->debug_rxy);
    cfwindow_destroy(_fs->debug_x);
    cfwindow_destroy(_fs->debug_nco_rx_out);
    cfwindow_destroy(_fs->debug_framesyms);
#endif
}
