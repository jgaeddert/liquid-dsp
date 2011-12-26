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
// flexframesync.c
//
// flexible frame synchronizer object
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

#include "liquid.internal.h"

#define FLEXFRAMESYNC_SQUELCH_TIMEOUT   (32)

#define FLEXFRAMESYNC_EQRLS_LAMBDA      (0.999f)

#define DEBUG_FLEXFRAMESYNC             0
#define DEBUG_FLEXFRAMESYNC_PRINT       0
#define DEBUG_FLEXFRAMESYNC_FILENAME    "flexframesync_internal_debug.m"
#define DEBUG_FLEXFRAMESYNC_BUFFER_LEN  (4096)

#if DEBUG_FLEXFRAMESYNC
void flexframesync_output_debug_file(flexframesync _fs);
#endif

struct flexframesync_s {

    // synchronizer objects
    agc_crcf agc_rx;                    // automatic gain control
    symsync_crcf mfdecim;               // symbol synchronizer (timing recovery)
    nco_crcf nco_rx;                    // oscillator and phase-locked loop
    bsync_rrrf fsync;                   // p/n sequence correlator

    // equalizer objects
    windowcf weq;                       // equalizer window (size: p/n length)
    eqrls_cccf eq;                      // RLS equalizer
    firfilt_cccf fireq;                 // FIR filter (equalizer)
    unsigned int eq_len;                // equalizer filter length
    float complex * heq;                // equalizer filter coefficients array
    float complex * pnsequence;         // transmitted p/n sequence (float)

    // squelch
    int squelch_status;                 // status of AGC squelch

    // status variables
    enum {
        FLEXFRAMESYNC_STATE_SEEKPN=0,   // seek p/n sequence
        FLEXFRAMESYNC_STATE_RXHEADER,   // receive header data
        FLEXFRAMESYNC_STATE_RXPAYLOAD,  // receive payload data
        FLEXFRAMESYNC_STATE_RESET       // reset synchronizer
    } state;
    unsigned int num_symbols_collected; // symbols collected counter
    unsigned int header_key;            // header cyclic redundancy check
    int header_valid;                   // header valid?

    // preamble
    modem mod_preamble;                 // preamble demodulator (BPSK)
    unsigned int pnsequence_len;        // p/n sequence length

    // header
    modem mod_header;                   // header demodulator (BPSK)
    packetizer p_header;                // header packetizer decoder
    unsigned char header_sym[256];      // header symbols (modem output)
    unsigned char header_enc[32];       // header data (encoded)
    unsigned char header[19];           // header data (decoded)
    int payload_valid;                  // header valid?

    // SINDR estimate (signal to interference, noise,
    // and distortion ratio)
    float evm_hat;      // averaged EVM

    // header properties
    modulation_scheme ms_payload;       // payload modulation scheme
    unsigned int bps_payload;           // payload modulation depth (bits/symbol)
    unsigned int payload_len;           // payload length (number of bytes)
    crc_scheme check;                   // payload validity check
    fec_scheme fec0;                    // payload fec (inner)
    fec_scheme fec1;                    // payload fec (outer)

    // payload
    packetizer p_payload;               // payload packetizer
    unsigned int payload_enc_msg_len;   // payload encoded message length
    unsigned int num_payload_symbols;   // payload length (number of symbols)
    modem mod_payload;                  // payload demodulator (variable)
    float complex * payload_samples;    // payload samples (modem input)
    unsigned char * payload_sym;        // payload symbols (modem output)
    unsigned char * payload_enc;        // paylod data (encoded)
    unsigned char * payload_dec;        // paylod data (decoded)
    unsigned int payload_samples_numalloc;
    unsigned int payload_sym_numalloc;
    unsigned int payload_enc_numalloc;
    unsigned int payload_dec_numalloc;

    // generic user-configurable properties
    framesyncprops_s props;

    // callback
    flexframesync_callback callback;    // user-defined callback function
    void * userdata;                    // user-defined data structure
    framesyncstats_s framestats;        // frame statistic object

    // medium access control
    unsigned int csma_enabled;
    framesync_csma_callback csma_lock;
    framesync_csma_callback csma_unlock;
    void * csma_userdata;

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
    windowcf debug_heq;
    unsigned int debug_heq_len;
#endif
};

// create flexframesync object
//  _props          :   properties structure pointer (default if NULL)
//  _callback       :   callback function invoked when frame is received
//  _userdata       :   user-defined data object passed to callback
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
    fs->mod_header = modem_create(LIQUID_MODEM_BPSK);
    fs->p_header = packetizer_create(19, LIQUID_CRC_16, LIQUID_FEC_HAMMING128, LIQUID_FEC_NONE);
    assert(packetizer_get_enc_msg_len(fs->p_header)==32);

    // agc, rssi, squelch
    fs->agc_rx = agc_crcf_create();
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
    fs->pnsequence_len = 64;
    // TODO : adjust msequence based on p/n sequence length
    msequence ms = msequence_create(6, 0x005b, 1);
    float pn_sequence[fs->pnsequence_len];
    for (i=0; i<fs->pnsequence_len; i++)
        pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    fs->fsync = bsync_rrrf_create(fs->pnsequence_len, pn_sequence);
    msequence_destroy(ms);

    // design symsync (k=2)
    unsigned int npfb = 32;
    unsigned int m=3;
    float beta=0.7f;
    fs->mfdecim = symsync_crcf_create_rnyquist(LIQUID_RNYQUIST_RRC, 2, m, beta, npfb);

    // 
    fs->mod_preamble = modem_create(LIQUID_MODEM_BPSK);

    // equalizer
    fs->eq_len = 1 + fs->props.eq_len;
    fs->heq = (float complex*) malloc(fs->eq_len*sizeof(float complex));
    for (i=0; i<fs->eq_len; i++)
        fs->heq[i] = (i==0) ? 1.0f : 0.0f;
    fs->weq = windowcf_create(fs->pnsequence_len); // equalizer window (size: p/n length)
    fs->eq = eqrls_cccf_create(NULL, fs->eq_len);         // RLS equalizer
    fs->fireq = firfilt_cccf_create(fs->heq, fs->eq_len);    // FIR filter (equalizer)
    fs->pnsequence = (float complex*) malloc(fs->pnsequence_len*sizeof(complex float));
    for (i=0; i<fs->pnsequence_len; i++)
        fs->pnsequence[i] = pn_sequence[i];

    // flexible frame properties (default values to be over-written
    // when frame header is received and decoded)
    fs->ms_payload  = LIQUID_MODEM_BPSK;
    fs->payload_len = 0;
    fs->check       = LIQUID_CRC_NONE;
    fs->fec0        = LIQUID_FEC_NONE;
    fs->fec1        = LIQUID_FEC_NONE;
    fs->payload_enc_msg_len = 0;
    fs->num_payload_symbols = 0;

    // set status flags
    fs->state = FLEXFRAMESYNC_STATE_SEEKPN;
    fs->num_symbols_collected = 0;

    // payload buffers, objects
    fs->mod_payload = modem_create(fs->ms_payload);
    fs->p_payload = packetizer_create(0, LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE);
    fs->payload_samples = NULL;
    fs->payload_sym = NULL;
    fs->payload_enc = NULL;
    fs->payload_dec = NULL;
    fs->payload_samples_numalloc = 0;
    fs->payload_sym_numalloc = 0;
    fs->payload_enc_numalloc = 0;
    fs->payload_dec_numalloc = 0;

    // reset, open bandwidth
    flexframesync_reset(fs);
    flexframesync_open_bandwidth(fs);

    // advanced mode : csma
    fs->csma_enabled = 0;
    fs->csma_lock = NULL;
    fs->csma_unlock = NULL;
    fs->csma_userdata = NULL;

#ifdef DEBUG_FLEXFRAMESYNC
    fs->debug_agc_rssi  =  windowf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_agc_out   = windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_x         = windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_rxy       = windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_rx_out= windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_framesyms = windowcf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_phase =  windowf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_nco_freq  =  windowf_create(DEBUG_FLEXFRAMESYNC_BUFFER_LEN);
    fs->debug_heq       = windowcf_create(64);
    fs->debug_heq_len   = 0;
#endif

    return fs;
}

void flexframesync_destroy(flexframesync _fs)
{
#if DEBUG_FLEXFRAMESYNC
    flexframesync_output_debug_file(_fs);

    // clean up debug windows
    windowf_destroy( _fs->debug_agc_rssi);
    windowcf_destroy(_fs->debug_agc_out);
    windowcf_destroy(_fs->debug_x);
    windowcf_destroy(_fs->debug_rxy);
    windowcf_destroy(_fs->debug_nco_rx_out);
    windowcf_destroy(_fs->debug_framesyms);
    windowf_destroy( _fs->debug_nco_phase);
    windowf_destroy( _fs->debug_nco_freq);
    windowcf_destroy(_fs->debug_heq);
#endif

    // destroy synchronizer objects
    agc_crcf_destroy(_fs->agc_rx);      // automatic gain control
    symsync_crcf_destroy(_fs->mfdecim); // symbol synchronizer
    nco_crcf_destroy(_fs->nco_rx);      // nco/pll for carrier recovery
    bsync_rrrf_destroy(_fs->fsync);     // p/n sequence correlator

    // destroy preamble objects
    modem_destroy(_fs->mod_preamble);   // preamble modem

    // destroy header objects
    modem_destroy(_fs->mod_header);     // header modem
    packetizer_destroy(_fs->p_header);  // header packetizer

    // free payload objects
    modem_destroy(_fs->mod_payload);    // payload modem
    packetizer_destroy(_fs->p_payload); // payload packetizer
    free(_fs->payload_samples);         // payload samples buffer
    free(_fs->payload_sym);             // payload symbols buffer
    free(_fs->payload_enc);             // payload data buffer (encoded)
    free(_fs->payload_dec);             // payload data buffer (decoded)

    // destroy equalizer objects
    windowcf_destroy(_fs->weq);
    eqrls_cccf_destroy(_fs->eq);
    firfilt_cccf_destroy(_fs->fireq);
    free(_fs->heq);
    free(_fs->pnsequence);

    // free main object memory
    free(_fs);
}

// get flexframesync properties
//  _fs     :   frame synchronizer object
//  _props  :   frame synchronizer properties structure pointer
void flexframesync_getprops(flexframesync _fs,
                            framesyncprops_s * _props)
{
    memmove(_props, &_fs->props, sizeof(framesyncprops_s));
}

// set flexframesync properties
//  _fs     :   frame synchronizer object
//  _props  :   frame synchronizer properties structure pointer
void flexframesync_setprops(flexframesync _fs,
                            framesyncprops_s * _props)
{
    // TODO : flexframesync_setprops() validate input
    if (_props->eq_len > 32) {
        fprintf(stderr,"error: flexframesync_setprops(), equalizer must be less than 32 taps\n");
        exit(1);
    }

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

// print flexframesync object internals
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
    printf("    payload len, uncoded:   %u bytes\n", _fs->payload_len);
    printf("    payload crc         :   %s\n", crc_scheme_str[_fs->check][1]);
    printf("    fec (inner)         :   %s\n", fec_scheme_str[_fs->fec0][1]);
    printf("    fec (outer)         :   %s\n", fec_scheme_str[_fs->fec1][1]);
    printf("    payload len, coded  :   %u bytes\n", _fs->payload_enc_msg_len);
    printf("    modulation scheme   :   %u-%s\n",
        1<<(_fs->bps_payload),
        modulation_types[_fs->ms_payload].name);
    printf("    num payload symbols :   %u\n", _fs->num_payload_symbols);
}

// reset frame synchronizer object
void flexframesync_reset(flexframesync _fs)
{
    agc_crcf_unlock(_fs->agc_rx);       // automatic gain control (unlock)
    symsync_crcf_clear(_fs->mfdecim);   // symbol synchronizer (clear state)
    symsync_crcf_unlock(_fs->mfdecim);  // symbol synchronizer (unlock)
    nco_crcf_reset(_fs->nco_rx);        // nco/pll (reset phase)

    // reset equalizer
    unsigned int i;
    for (i=0; i<_fs->eq_len; i++)
        _fs->heq[i] = (i==0) ? 1.0f : 0.0f;
    _fs->fireq = firfilt_cccf_recreate(_fs->fireq, _fs->heq, _fs->eq_len);

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

    // clear stats object
    framesyncstats_init_default(&_fs->framestats);

    // TODO open bandwidth?
}

// execute frame synchronizer
//  _fs     :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
void flexframesync_execute(flexframesync _fs, float complex *_x, unsigned int _n)
{
    unsigned int i, j, nw;
    float complex agc_rx_out;
    float complex mfdecim_out[4];
    float complex nco_rx_out;
    float phase_error;
    unsigned int demod_sym;

    for (i=0; i<_n; i++) {
        // agc
        agc_crcf_execute(_fs->agc_rx, _x[i], &agc_rx_out);

#ifdef DEBUG_FLEXFRAMESYNC
        windowcf_push(_fs->debug_x, _x[i]);
        windowf_push(_fs->debug_agc_rssi, agc_crcf_get_rssi(_fs->agc_rx));
        windowcf_push(_fs->debug_agc_out, agc_rx_out);
#endif

        // squelch: block agc output from synchronizer only if
        // 1. received signal strength indicator has not exceeded squelch
        //    threshold at any time within the past <squelch_timeout> samples
        // 2. mode is FLEXFRAMESYNC_STATE_SEEKPN (seek p/n sequence)
        _fs->squelch_status = agc_crcf_squelch_get_status(_fs->agc_rx);

        // if squelch is enabled, skip remaining of synchronizer
        // NOTE : if squelch is deactivated, the default status
        //        value is LIQUID_AGC_SQUELCH_SIGNALHI
        if (_fs->squelch_status == LIQUID_AGC_SQUELCH_RISE) {
            // invoke csma lock function, if enabled
            flexframesync_csma_lock(_fs);

        } else if (_fs->squelch_status == LIQUID_AGC_SQUELCH_TIMEOUT) {
            // invoke csma unlock function, if enabled
            flexframesync_csma_unlock(_fs);

            // reset on timeout (very important!)
            flexframesync_reset(_fs);

        } else if (_fs->squelch_status == LIQUID_AGC_SQUELCH_ENABLED) {
            continue;
        }

        // symbol synchronizer
        symsync_crcf_execute(_fs->mfdecim, &agc_rx_out, 1, mfdecim_out, &nw);

        for (j=0; j<nw; j++) {
            // push through equalizer, ignoring if equalizer length is 1
            if (_fs->eq_len > 1) {
                firfilt_cccf_push(_fs->fireq, mfdecim_out[j]);
                firfilt_cccf_execute(_fs->fireq, &mfdecim_out[j]);
            }

            // mix down, demodulate, run PLL
            nco_crcf_mix_down(_fs->nco_rx, mfdecim_out[j], &nco_rx_out);
            if (_fs->state == FLEXFRAMESYNC_STATE_SEEKPN) {
            //if (0) {
                modem_demodulate(_fs->mod_preamble, nco_rx_out, &demod_sym);
                phase_error = modem_get_demodulator_phase_error(_fs->mod_preamble);
            } else if (_fs->state == FLEXFRAMESYNC_STATE_RXHEADER) {
                modem_demodulate(_fs->mod_header, nco_rx_out, &demod_sym);
                phase_error = modem_get_demodulator_phase_error(_fs->mod_header);
            } else {
                modem_demodulate(_fs->mod_payload, nco_rx_out, &demod_sym);
                phase_error = modem_get_demodulator_phase_error(_fs->mod_payload);
                phase_error *= cabsf(nco_rx_out);
            }

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

            //
            switch (_fs->state) {
            case FLEXFRAMESYNC_STATE_SEEKPN:
                flexframesync_execute_seekpn(_fs, nco_rx_out, demod_sym);
                break;
            case FLEXFRAMESYNC_STATE_RXHEADER:
                flexframesync_execute_rxheader(_fs, nco_rx_out, demod_sym);
                break;
            case FLEXFRAMESYNC_STATE_RXPAYLOAD:
                flexframesync_execute_rxpayload(_fs, nco_rx_out, demod_sym);
                break;
            case FLEXFRAMESYNC_STATE_RESET:
                flexframesync_execute_reset(_fs, nco_rx_out, demod_sym);
                break;
            default:;
            }
        }
    }
}

// 
// internal
//

// open bandwidth of synchronizer objects (acquisition mode)
void flexframesync_open_bandwidth(flexframesync _fs)
{
    // open bandwidth of automatic gain control
    agc_crcf_set_bandwidth(_fs->agc_rx, _fs->props.agc_bw0);

    // open bandwidth of symbol synchronizer
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->props.sym_bw0);

    // open bandwidth of nco/pll
    nco_crcf_pll_set_bandwidth(_fs->nco_rx, _fs->props.pll_bw0);
}

// close bandwidth of synchronizer objects (tracking mode)
void flexframesync_close_bandwidth(flexframesync _fs)
{
    // close bandwidth of automatic gain control
    agc_crcf_set_bandwidth(_fs->agc_rx, _fs->props.agc_bw1);

    // close bandwidth of symbol synchronizer
    symsync_crcf_set_lf_bw(_fs->mfdecim, _fs->props.sym_bw1);

    // close bandwidth of nco/pll
    nco_crcf_pll_set_bandwidth(_fs->nco_rx, _fs->props.pll_bw1);
}

// train equalizer
void flexframesync_train_eq(flexframesync _fs)
{
    // check to see if length has changed
    if (_fs->eq_len != _fs->props.eq_len + 1) {
        _fs->eq_len = _fs->props.eq_len + 1;
        //printf("new equalizer : %u taps\n", _fs->eq_len);

        // re-create the equalizer
        _fs->eq = eqrls_cccf_recreate(_fs->eq, NULL, _fs->eq_len);

        // re-allocate coefficients array
        _fs->heq = (float complex*) realloc(_fs->heq, _fs->eq_len*sizeof(float complex));
    }

    // ignore training equalizer if length is 1
    if (_fs->eq_len == 1)
        return;

    // read received P/N sequence from buffer
    float complex * y;
    windowcf_read(_fs->weq, &y);

    // set the equalizer training factor and train
    eqrls_cccf_set_bw(_fs->eq, _fs->props.eqrls_lambda);
    eqrls_cccf_train(_fs->eq, _fs->heq, y, _fs->pnsequence, _fs->pnsequence_len);

    // ensure phase of first tap of equalizer is zero, de-rotate
    // all taps by same complex phase gain
    // TODO : look at tap with highest magnitude (not necessarily
    //        the first one?)
    float complex g = cexpf(-_Complex_I*cargf(_fs->heq[0]));
    unsigned int i;
    for (i=0; i<_fs->eq_len; i++)
        _fs->heq[i] *= g;

    // normalize equalizer gain
    float e2 = 0.0f;
    for (i=0; i<_fs->eq_len; i++)
        e2 += crealf(_fs->heq[i] * conjf(_fs->heq[i]));
    for (i=0; i<_fs->eq_len; i++)
        _fs->heq[i] *= sqrtf(1.0f / e2);

    // re-create equalizer filter
    _fs->fireq = firfilt_cccf_recreate(_fs->fireq, _fs->heq, _fs->eq_len);

#  if 0
    // print received P/N sequence
    for (i=0; i<_fs->pnsequence_len; i++)
        printf("pn(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_fs->pnsequence[i]), cimagf(_fs->pnsequence[i]));
    for (i=0; i<_fs->pnsequence_len; i++)
        printf("rpn(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    for (i=0; i<_fs->eq_len; i++)
        printf("heq(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(_fs->heq[i]), cimagf(_fs->heq[i]));
#  endif

#ifdef DEBUG_FLEXFRAMESYNC
    for (i=0; i<_fs->eq_len; i++)
        windowcf_push(_fs->debug_heq, _fs->heq[i]);
    _fs->debug_heq_len = _fs->eq_len;
#endif
}

// 
// state-specific execute methods
//

// execute synchronizer, seeking p/n sequence
//  _fs     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void flexframesync_execute_seekpn(flexframesync _fs,
                                  float complex _x,
                                  unsigned int _sym)
{
    // run cross-correlator to find p/n sequence
    float rxy;
    bsync_rrrf_correlate(_fs->fsync, _x, &rxy);

    // save symbols to equalizer window
    windowcf_push(_fs->weq, _x);

#ifdef DEBUG_FLEXFRAMESYNC
    windowcf_push(_fs->debug_rxy, rxy);
#endif

    // check if p/n sequence is found (correlation value
    // exceeds threshold)
    if (fabsf(rxy) > 0.7f) {
        // close bandwidth
        flexframesync_close_bandwidth(_fs);

        // adjust phase of receiver NCO based on p/n correlation phase
        nco_crcf_adjust_phase(_fs->nco_rx, cargf(rxy));

        // lock symbol synchronizer timing phase
        //symsync_crcf_lock(_fs->mfdecim);

        // deactivate squelch as not to suppress signal in the
        // middle of the frame
        agc_crcf_squelch_deactivate(_fs->agc_rx);

        // update synchronizer state
        _fs->state = FLEXFRAMESYNC_STATE_RXHEADER;

        // train equalizer
        flexframesync_train_eq(_fs);

    }

}

// execute synchronizer, receiving header
//  _fs     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void flexframesync_execute_rxheader(flexframesync _fs,
                                    float complex _x,
                                    unsigned int _sym)
{
    // append symbol to buffer
    _fs->header_sym[_fs->num_symbols_collected] = _sym;
    _fs->num_symbols_collected++;

    // SINDR estimation
    float evm;
    evm = modem_get_demodulator_evm(_fs->mod_header);
    _fs->evm_hat += evm;

    if (_fs->num_symbols_collected==256) {

        // estimate signal-to-noise ratio, rssi
        _fs->framestats.evm  =  10*log10f(_fs->evm_hat / 256.0f);
        _fs->framestats.rssi =  10*log10(agc_crcf_get_signal_level(_fs->agc_rx));
        _fs->evm_hat = 0.0f;
#if DEBUG_FLEXFRAMESYNC_PRINT
        printf("  evm   :   %12.8f dB\n", _fs->framestats.evm);
#endif

        // reset symbol counter
        _fs->num_symbols_collected = 0;

        // decode frame header
        flexframesync_decode_header(_fs);

        // check to see if header is valid
        //  yes :   continue on to receive payload
        //  no  :   invoke callback, flagging frame as invalid
        if (_fs->header_valid && _fs->num_payload_symbols > 0) {
            // fully lock automatic gain control
            agc_crcf_lock(_fs->agc_rx);

            // update synchronizer state
            _fs->state = FLEXFRAMESYNC_STATE_RXPAYLOAD;
        } else {
            if (_fs->callback != NULL) {
                // invoke callback method
                // cannot decode frame: invoke callback anyway, but ignore rest of payload
                // payload length is 0 : ignore payload
                _fs->framestats.cfo             =  nco_crcf_get_frequency(_fs->nco_rx);
                _fs->framestats.framesyms       = NULL;
                _fs->framestats.num_framesyms   = 0;
                _fs->framestats.mod_scheme      = LIQUID_MODEM_UNKNOWN;
                _fs->framestats.mod_bps         = 0;
                _fs->framestats.check           = LIQUID_CRC_UNKNOWN;
                _fs->framestats.fec0            = LIQUID_FEC_UNKNOWN;
                _fs->framestats.fec1            = LIQUID_FEC_UNKNOWN;

                _fs->callback(_fs->header,  _fs->header_valid,
                              NULL,         0, 0,
                              _fs->framestats,
                              _fs->userdata);
            }

            // update synchronizer state
            _fs->state = FLEXFRAMESYNC_STATE_RESET;
        }
    }


}

// execute synchronizer, receiving payload
//  _fs     :   frame synchronizer object
//  _x      :   input sample
//  _sym    :   demodulated symbol
void flexframesync_execute_rxpayload(flexframesync _fs,
                                     float complex _x,
                                     unsigned int _sym)
{
#ifdef DEBUG_FLEXFRAMESYNC
    windowcf_push(_fs->debug_framesyms, _x);
#endif

    assert(_fs->num_payload_symbols > 0);

    // append symbol to buffer
    _fs->payload_samples[_fs->num_symbols_collected] = _x;
    _fs->payload_sym[_fs->num_symbols_collected] = (unsigned char) _sym;
    _fs->num_symbols_collected++;

    // check to see if full payload has been received
    if (_fs->num_symbols_collected==_fs->num_payload_symbols) {
        // reset symbol counter
        _fs->num_symbols_collected = 0;

        // assemble raw payload by packing frame data symbols into
        // output buffer
        flexframesync_assemble_payload(_fs);

        // decode payload using internal packet decoder
        _fs->payload_valid =
        packetizer_decode(_fs->p_payload,
                          _fs->payload_enc,
                          _fs->payload_dec);
        //printf("payload crc : %s\n", _fs->payload_valid ? "pass" : "FAIL");

        if (_fs->callback != NULL) {
            // set framestats internals
            _fs->framestats.cfo             =  nco_crcf_get_frequency(_fs->nco_rx);
            _fs->framestats.framesyms       = _fs->payload_samples;
            _fs->framestats.num_framesyms   = _fs->num_payload_symbols;
            _fs->framestats.mod_scheme      = _fs->ms_payload;
            _fs->framestats.mod_bps         = _fs->bps_payload;
            _fs->framestats.check           = _fs->check;
            _fs->framestats.fec0            = _fs->fec0;
            _fs->framestats.fec1            = _fs->fec1;

            // invoke callback method
            _fs->callback(_fs->header,
                          _fs->header_valid,
                          _fs->payload_dec,
                          _fs->payload_len,
                          _fs->payload_valid,
                          _fs->framestats,
                          _fs->userdata);
        }

        // update synchronizer state
        _fs->state = FLEXFRAMESYNC_STATE_RESET;
        //_fs->state = FLEXFRAMESYNC_STATE_SEEKPN;
    }
}

// execute synchronizer, resetting object
//  _fs     :   frame synchronizer object
//  _x      :   input sample
void flexframesync_execute_reset(flexframesync _fs,
                                 float complex _x,
                                 unsigned int _sym)
{
    // open bandwidth
    flexframesync_open_bandwidth(_fs);

    // unlock automatic gain control
    agc_crcf_unlock(_fs->agc_rx);

    // re-activate squelch
    if (_fs->props.squelch_enabled)
        agc_crcf_squelch_activate(_fs->agc_rx);

    // unlock symbol synchronizer
    symsync_crcf_unlock(_fs->mfdecim);

    // reset oscillator
    nco_crcf_reset(_fs->nco_rx);

    // reset equalizer
    unsigned int i;
    for (i=0; i<_fs->eq_len; i++)
        _fs->heq[i] = (i==0) ? 1.0f : 0.0f;
    _fs->fireq = firfilt_cccf_recreate(_fs->fireq, _fs->heq, _fs->eq_len);

    // update synchronizer state
    _fs->state = FLEXFRAMESYNC_STATE_SEEKPN;
}

// 
// advanced modes
//

// enable csma and set external callback functions
//  _fs             :   frame synchronizer object
//  _csma_lock      :   callback to be invoked when signal is high
//  _csma_unlock    :   callback to be invoked when signal is again low
//  _csma_userdata  :   structure passed to callback functions
void flexframesync_set_csma_callbacks(flexframesync _fs,
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
void flexframesync_csma_lock(flexframesync _fs)
{
    if (_fs->csma_enabled && _fs->csma_lock != NULL)
        _fs->csma_lock( _fs->csma_userdata );
}

// if enabled, invoke external csma unlock callback
void flexframesync_csma_unlock(flexframesync _fs)
{
    if (_fs->csma_enabled && _fs->csma_unlock != NULL)
        _fs->csma_unlock( _fs->csma_userdata );
}


// 
// decoding methods
//

// configure payload buffers, reallocating memory as necessary
void flexframesync_configure_payload_buffers(flexframesync _fs)
{
    //printf("flexframesync : payload symbols : %u\n", _fs->num_payload_symbols);

    div_t d;

    // compute payload length (symbols) from number of payload bytes
    //  # payload symbols = ceil( 8 [bits/byte] * payload_enc_msg_len [bytes] / modem depth [bits/symbol] )
    d = div(8*_fs->payload_enc_msg_len, _fs->bps_payload);
    _fs->num_payload_symbols = d.quot + (d.rem ? 1 : 0);

    // required payload allocation size, considering the total number of
    // bits might not divide evenly by the modulation depth
    //  
    d = div(_fs->num_payload_symbols*_fs->bps_payload, 8);
    unsigned int payload_enc_numalloc_req = d.quot + (d.rem ? 1 : 0);

    // allocate memory for payload symbols (decoded)
    _fs->payload_dec = (unsigned char*) realloc(_fs->payload_dec, _fs->payload_len);

    // allocate memory for payload symbols (encoded)
    if (_fs->payload_enc_numalloc < payload_enc_numalloc_req) {
        _fs->payload_enc_numalloc = payload_enc_numalloc_req;
        _fs->payload_enc = (unsigned char*) realloc(_fs->payload_enc, _fs->payload_enc_numalloc);
    }

    // allocate memory for payload data (bytes)
    if (_fs->payload_sym_numalloc < _fs->num_payload_symbols) {
        _fs->payload_sym_numalloc = _fs->num_payload_symbols;
        _fs->payload_sym = (unsigned char*) realloc(_fs->payload_sym, _fs->payload_sym_numalloc);
        //printf("reallocating payload_sym (payload symbols) : %u\n", _fs->payload_sym_numalloc);
    }

    // allocate memory for payload samples
    if (_fs->payload_samples_numalloc < _fs->num_payload_symbols) {
        _fs->payload_samples_numalloc = _fs->num_payload_symbols;
        _fs->payload_samples = (float complex*) realloc(_fs->payload_samples, _fs->payload_samples_numalloc*sizeof(float complex));
        //printf("reallocating payload_samples (modulated payload symbols) : %u\n",
        //        _fs->payload_samples_numalloc);
    }
}

// decode frame header
void flexframesync_decode_header(flexframesync _fs)
{
    // pack 256 1-bit header symbols into 32 8-bit bytes
    unsigned int num_written;
    liquid_pack_bytes(_fs->header_sym, 256,
                      _fs->header_enc, 32,
                      &num_written);
    assert(num_written==32);

    // run packet decoder
    _fs->header_valid =
    packetizer_decode(_fs->p_header, _fs->header_enc, _fs->header);

    // return if header is invalid
    if (!_fs->header_valid)
        return;

    // unscramble header
    unscramble_data(_fs->header, 19);

    // strip off CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [17]
    //  fec0    : least-significant 5 bits of [17]
    //  fec1    : least-significant 5 bits of [18]
    unsigned int check = (_fs->header[17] >> 5 ) & 0x07;
    unsigned int fec0  = (_fs->header[17]      ) & 0x1f;
    unsigned int fec1  = (_fs->header[18]      ) & 0x1f;

    // validate properties
    if (check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr,"warning: flexframesync_decode_header(), decoded CRC exceeds available\n");
        check = LIQUID_CRC_UNKNOWN;
    }
    if (fec0 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,"warning: flexframesync_decode_header(), decoded FEC (inner) exceeds available\n");
        fec0 = LIQUID_FEC_UNKNOWN;
    }
    if (fec1 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,"warning: flexframesync_decode_header(), decoded FEC (outer) exceeds available\n");
        fec1 = LIQUID_FEC_UNKNOWN;
    }

    // strip off modulation scheme/depth
    unsigned int mod_scheme = _fs->header[16];
    if (mod_scheme == 0 || mod_scheme >= LIQUID_MODEM_NUM_SCHEMES) {
        fprintf(stderr,"warning: flexframesync_decode_header(), invalid modulation scheme\n");
        _fs->header_valid = 0;
        return;
    }

    // strip off payload length
    unsigned int payload_len = (_fs->header[14] << 8) | (_fs->header[15]);
    _fs->payload_len = payload_len;

    // configure payload receiver
    if (_fs->header_valid) {
        // configure modem
        if (mod_scheme != _fs->ms_payload) {
#if DEBUG_FLEXFRAMESYNC_PRINT
            printf("flexframesync : configuring payload modem : %u-%s\n",
                    1<<mod_depth,
                    modulation_types[mod_scheme].name);
#endif
            // set new modem properties
            _fs->ms_payload = mod_scheme;
            _fs->bps_payload = modulation_types[mod_scheme].bps;

            // recreate modem
            _fs->mod_payload = modem_recreate(_fs->mod_payload, _fs->ms_payload);
        }

#if DEBUG_FLEXFRAMESYNC_PRINT
        printf("flexframesync : configuring payload packetizer : %s/%s/%s\n",
                crc_scheme_str[check][0],
                fec_scheme_str[fec0][0],
                fec_scheme_str[fec1][0]);
#endif
        // set new packetizer properties
        _fs->check  = check;
        _fs->fec0   = fec0;
        _fs->fec1   = fec1;

        // recreate packetizer object
        _fs->p_payload = packetizer_recreate(_fs->p_payload,
                                             _fs->payload_len,
                                             _fs->check,
                                             _fs->fec0,
                                             _fs->fec1);

        // re-compute payload encoded message length
        _fs->payload_enc_msg_len = packetizer_get_enc_msg_len(_fs->p_payload);

        // configure buffers
        flexframesync_configure_payload_buffers(_fs);
    }
    
#if DEBUG_FLEXFRAMESYNC_PRINT
    // print results
    printf("flexframesync_decode_header():\n");
    printf("    header crc  : %s\n", _fs->header_valid ? "pass" : "FAIL");
    printf("    check       : %s\n", crc_scheme_str[check][1]);
    printf("    fec (inner) : %s\n", fec_scheme_str[fec0][1]);
    printf("    fec (outer) : %s\n", fec_scheme_str[fec1][1]);
    printf("    mod scheme  : %u-%s\n", 1<<mod_depth, modulation_types[mod_scheme].name);
    printf("    payload len : %u\n", payload_len);
    printf("    num symbols : %u\n", _fs->num_payload_symbols);

    printf("    user data   :");
    unsigned int i;
    for (i=0; i<14; i++)
        printf(" %.2x", _fs->header[i]);
    printf("\n");
#endif
}

// assemble raw payload by packing frame data symbols into output buffer
void flexframesync_assemble_payload(flexframesync _fs)
{
    // pack (8-bit) bytes from (bps_payload-bit) symbols
    unsigned int num_written;
    liquid_repack_bytes(_fs->payload_sym, _fs->bps_payload, _fs->num_payload_symbols,
                        _fs->payload_enc, 8,                _fs->payload_enc_numalloc,
                        &num_written);
}


#ifdef DEBUG_FLEXFRAMESYNC
// huge method to write debugging data to file
void flexframesync_output_debug_file(flexframesync _fs)
{
    unsigned int i;
    float * r;
    float complex * rc;
    FILE* fid = fopen(DEBUG_FLEXFRAMESYNC_FILENAME,"w");
    if (!fid) {
        fprintf(stderr, "error: flexframesync_output_debug_file(), could not open file for writing\n");
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
    fprintf(fid,"plot(agc_rssi)\n");
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
    fprintf(fid,"plot(framesyms,'x','MarkerSize',4)\n");
    fprintf(fid,"xlabel('I');\n");
    fprintf(fid,"ylabel('Q');\n");
    fprintf(fid,"axis([-1.5 1.5 -1.5 1.5]);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on\n");

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

    // write equalizer taps
    if (_fs->debug_heq_len > 0) {
        windowcf_read(_fs->debug_heq, &rc);
        rc = &rc[64-_fs->debug_heq_len-1];  // offset by window length
        fprintf(fid,"heq = zeros(1,%u);\n", _fs->debug_heq_len);
        for (i=0; i<_fs->debug_heq_len; i++)
            fprintf(fid,"heq(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    } else {
        fprintf(fid,"heq = 1;\n");
    }
    fprintf(fid,"figure;\n");
    fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Heq = -20*log10(abs(fftshift(fft(heq,nfft))));\n");
    fprintf(fid,"plot(f,Heq);\n");
    fprintf(fid,"axis([-0.5 0.5 min(Heq)-10 20]);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('inverted equalizer response');\n");
    fprintf(fid,"grid on;\n");

    fprintf(fid,"\n\n");
    fclose(fid);

    printf("flexframesync/debug: results written to %s\n", DEBUG_FLEXFRAMESYNC_FILENAME);

}
#endif
