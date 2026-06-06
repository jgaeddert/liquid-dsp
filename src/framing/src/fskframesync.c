/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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
int fskframesync_execute_detectframe(fskframesync _q, float complex _x);
int fskframesync_execute_rxheader(   fskframesync _q, float complex _x);
int fskframesync_execute_rxpayload(  fskframesync _q, float complex _x);

// decode header
int fskframesync_decode_header(fskframesync _q);

// fskframesync object structure
struct fskframesync_s {
    unsigned int    m;                  // demodulator bits/symbol
    unsigned int    k;                  // demodulator samples/symbol
    float           bandwidth;          // demodulator bandwidth
    unsigned int    M;                  // demodulator constellation size, M=2^m
    fskdem          dem_header;         // demodulator object for the header (BFSK)
    fskdem          dem;                // demodulator object (M-FSK)
    float complex * buf;                // demodulator transmit buffer [size: k x 1]

    framesync_callback  callback;       // user-defined callback function
    void *              userdata;       // user-defined data structure
    framesyncstats_s    framestats;     // frame statistic object

    // synchronizer objects, states
    firpfb_crcf     pfb;                // timing recovery
    unsigned int    npfb;               // timing recovery (number of filters in bank)
    unsigned int    pfb_index;          // timing recovery (filter bank index)
    nco_crcf        nco;                // coarse carrier frequency recovery
    firfilt_rrrf    detector;           // frame correlator detector
    windowcf        buf_rx;             // pre-demod buffered samples, size: k
    windowf         buf_LLR2;           // detector signal level
    float           rxy[3];             // detector output for timing recovery

    // header
    unsigned int    header_dec_len;     // header length (decoded bytes)
    unsigned int    header_sym_len;     // header length (number of modulated symbols)
    unsigned char * header_dec;         // header uncoded [size: header_dec_len x 1]
    unsigned char * header_sym;         // header: unmodulated symbols
    qpacketmodem    header_decoder;     //
    int             header_valid;       // header: flag indicating validity of header data

    // payload
    unsigned int    payload_dec_len;    // payload decoded message length
    crc_scheme      payload_crc;        // payload validity check
    fec_scheme      payload_fec0;       // payload inner code
    fec_scheme      payload_fec1;       // payload outer code
    unsigned int    payload_sym_len;    // payload symbols length
    unsigned char * payload_sym;        //
    unsigned char * payload_dec;        // payload decoded [size: payload_dec_len x 1]
    qpacketmodem    payload_decoder;    //

    // framing state
    enum {
                    STATE_DETECTFRAME=0,// preamble
                    STATE_RXHEADER,     // header
                    STATE_RXPAYLOAD,    // payload (frame)
    }               state;
    int             frame_assembled;    // frame assembled flag
    int             frame_detected;     // frame detected flag
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
    q->bandwidth = 0.25f;

    // create demodulators
    q->dem_header = fskdem_create(   1, q->k, q->bandwidth);
    q->dem        = fskdem_create(q->m, q->k, q->bandwidth);
    q->buf        = (float complex*) malloc( q->k * sizeof(float complex) );

    // create polyphase filterbank for timing recovery
    q->npfb = 64;
    q->pfb  = firpfb_crcf_create_kaiser(q->npfb, 5, 0.45f, 40.0f);

    // create oscillator for frequency recovery
    q->nco = nco_crcf_create(LIQUID_VCO);

    // create buffer for demodulator input
    q->buf_rx = windowcf_create(q->k);

    // create preamble frame detector from preamble symbols (over-sampled by 2)
    msequence preamble_ms = msequence_create(6, 0x6d, 1);
    unsigned int preamble_sym_len = 63; // 64;
    float * preamble = (float*) malloc( 2*preamble_sym_len*sizeof(float) );
    unsigned int i;
    for (i=0; i<preamble_sym_len; i++) {
        float v = msequence_advance(preamble_ms) ? 1.0f : -1.0f;

        // reverse direction for filter
        preamble[2*preamble_sym_len - (2*i+0) - 1] = v;
        preamble[2*preamble_sym_len - (2*i+1) - 1] = v;
    }
    q->detector = firfilt_rrrf_create(preamble, 2*preamble_sym_len);
    free(preamble);
    msequence_destroy(preamble_ms);

    // create buffer for detection
    q->buf_LLR2 = windowf_create(2*preamble_sym_len);

    // header objects/arrays
    q->header_dec_len   = 12;
    q->header_dec       = (unsigned char*)malloc(q->header_dec_len*sizeof(unsigned char));
    q->header_decoder   = qpacketmodem_create();
    qpacketmodem_configure(q->header_decoder,
                           q->header_dec_len,
                           LIQUID_CRC_32,
                           LIQUID_FEC_NONE,
                           LIQUID_FEC_GOLAY2412,
                           LIQUID_MODEM_BPSK);
    q->header_sym_len   = qpacketmodem_get_frame_len(q->header_decoder);
    q->header_sym       = (unsigned char*)malloc(q->header_sym_len*sizeof(unsigned char));

    // payload objects/arrays
    q->payload_dec_len  = 200;
    q->payload_crc      = LIQUID_CRC_32;
    q->payload_fec0     = LIQUID_FEC_NONE;
    q->payload_fec1     = LIQUID_FEC_HAMMING128;
    q->payload_decoder  = qpacketmodem_create();
    qpacketmodem_configure(q->payload_decoder,
                           q->payload_dec_len,
                           q->payload_crc,
                           q->payload_fec0,
                           q->payload_fec1,
                           LIQUID_MODEM_QAM16);  // TODO: set bits/sym appropriately
    q->payload_sym_len  = qpacketmodem_get_frame_len(q->payload_decoder);
    q->payload_sym      = (unsigned char*)malloc(q->payload_sym_len*sizeof(unsigned char));
    q->payload_dec      = (unsigned char*)malloc(q->payload_dec_len*sizeof(unsigned char));

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
int fskframesync_destroy(fskframesync _q)
{
#if DEBUG_FSKFRAMESYNC
    // destroy debugging objects
    if (_q->debug_objects_created) {
        windowcf_destroy(_q->debug_x);
    }
#endif

    // destroy modulators
    fskdem_destroy(_q->dem_header);
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
    windowf_destroy(_q->buf_LLR2);

    // destroy/free header objects/arrays
    free(_q->header_dec);
    free(_q->header_sym);
    qpacketmodem_destroy(_q->header_decoder);

    // destroy/free payload objects/arrays
    free(_q->payload_sym);
    free(_q->payload_dec);
    qpacketmodem_destroy(_q->payload_decoder);

    // free main object memory
    free(_q);
    return LIQUID_OK;
}

// print frame synchronizer object internals
int fskframesync_print(fskframesync _q)
{
#if 0
    printf("fskframesync:\n");
    printf("  physical properties\n");
    printf("    bits/symbol     :   %u\n", _q->m);
    printf("    samples/symbol  :   %u\n", _q->k);
    printf("    bandwidth       :   %-8.3f\n", _q->bandwidth);
    printf("  framing properties\n");
    printf("    preamble        :   %-4u symbols\n", 0); //_q->preamble_sym_len);
    printf("    header          :   %-4u symbols, %-4u bytes\n", _q->header_sym_len, _q->header_dec_len);
    printf("    payload         :   %-4u symbols, %-4u bytes\n", _q->payload_sym_len, _q->payload_dec_len);
    printf("  packet properties\n");
    printf("    crc             :   %s\n", crc_scheme_str[_q->payload_crc ][1]);
    printf("    fec (inner)     :   %s\n", fec_scheme_str[_q->payload_fec0][1]);
    printf("    fec (outer)     :   %s\n", fec_scheme_str[_q->payload_fec1][1]);
    printf("  total samples     :   %-4u samples\n", 0);
#else
    printf("<liquid.fskframesync>\n");
#endif
    return LIQUID_OK;
}

// reset frame synchronizer object
int fskframesync_reset(fskframesync _q)
{
    // reset symbol timing recovery state
    firpfb_crcf_reset(_q->pfb);

    // reset carrier recovery objects
    nco_crcf_reset(_q->nco);

    // clear pre-demod buffer
    windowcf_reset(_q->buf_rx);

    // reset internal objects
    firfilt_rrrf_reset(_q->detector);

    // reset state and counters
    _q->state            = STATE_DETECTFRAME;
    _q->frame_detected   = 0;
    _q->sample_counter   = 0;
    _q->symbol_counter   = 0;
    _q->timer            = _q->k - 1;
    _q->pfb_index        = 0;
    return LIQUID_OK;
}

// execute frame synchronizer
//  _q      :   frame synchronizer object
//  _x      :   input sample
int fskframesync_execute(fskframesync  _q,
                         float complex _x)
{
    // push through synchronizer
#if DEBUG_FSKFRAMESYNC
    if (_q->debug_enabled)
        windowcf_push(_q->debug_x, _x);
#endif

    switch (_q->state) {
    case STATE_DETECTFRAME: return fskframesync_execute_detectframe(_q, _x);
    case STATE_RXHEADER:    return fskframesync_execute_rxheader   (_q, _x);
    case STATE_RXPAYLOAD:   return fskframesync_execute_rxpayload  (_q, _x);
    default:;
    }
    return liquid_error(LIQUID_EINT,"fskframesync_execute(), invalid internal mode");
}

// execute frame synchronizer on a block of samples
//  _q      :   frame synchronizer object
//  _x      :   input sample array [size: _n x 1]
//  _n      :   number of input samples
int fskframesync_execute_block(fskframesync    _q,
                                float complex * _x,
                                unsigned int    _n)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        if (fskframesync_execute(_q, _x[i]))
            return liquid_error(LIQUID_EINT,"fskframesync_execute_block(), invalid internal mode");
    }
    return LIQUID_OK;
}

// 
// internal methods
//

int fskframesync_execute_detectframe(fskframesync  _q,
                                     float complex _x)
{
#if 0
    // push sample through timing recovery and compute output
    float complex y;
    firpfb_crcf_push(_q->pfb, _x);
    firpfb_crcf_execute(_q->pfb, 0, &y);

    // push sample into pre-demod p/n sequence buffer
    windowcf_push(_q->buf_rx, y);
#else
    windowcf_push(_q->buf_rx, _x);
#endif

    // decrement timer and determine if symbol output is ready
    _q->timer--;
    if (_q->timer)
        return LIQUID_OK;

    // reset timer
    _q->timer = _q->k;

    // run demodulator and retrieve FFT result, computing LLR sample output
    float complex * r;
    windowcf_read(_q->buf_rx, &r);
    fskdem_demodulate(_q->dem_header, r);
    int fft_bin_range = 2;
    float v0 = fskdem_get_symbol_energy(_q->dem_header, 0, fft_bin_range); // energy for '0' symbol
    float v1 = fskdem_get_symbol_energy(_q->dem_header, 1, fft_bin_range); // energy for '1' symbol

    // compute LLR value
    float LLR = logf( (v1+1e-9f)/(v0+1e-9f) );

    // push result into detector
    float v;
    firfilt_rrrf_push(   _q->detector, LLR);
    firfilt_rrrf_execute(_q->detector, &v);

    // scale by signal level
    windowf_push(_q->buf_LLR2, LLR*LLR);
    float * rf;
    windowf_read(_q->buf_LLR2, &rf);
    float g = 0.0f;
    unsigned int i;
    unsigned int n = 126;
    // sum squares
    for (i=0; i<n; i++)
        g += rf[i];
    float rxy = v / ((float)n * (1e-6f + sqrtf(g/(float)n)));
    
    //printf("LLR(end+1) = %12.4e; v(end+1) = %12.4e; g(end+1) = %12.4e;\n", LLR, rxy, g);

    // shift correlator values
    _q->rxy[0] = _q->rxy[1];
    _q->rxy[1] = _q->rxy[2];
    _q->rxy[2] = rxy;

    // check state; we are waiting for correlator to peak here
    if (!_q->frame_detected) {
        // frame not yet detected; check cross-correlator output
        // NOTE: because this is a ratio of energies in frequency, we don't need
        //       to take the absolute value here; only positive values should work
        if (rxy > 0.5f) {
            //printf("### fskframe detected! ###\n");
            _q->frame_detected = 1;
        }
    } else {
        // frame has already been detected; wait for signal to peak
        if (_q->rxy[1] > _q->rxy[2]) {
            liquid_log_trace("fskframesync, signal peaked! %12.8f %12.8f %12.8f", _q->rxy[0], _q->rxy[1], _q->rxy[2]);

#if 1
            // compute estimate, apply bias compensation
            float gamma = (_q->rxy[2] - _q->rxy[0]) / _q->rxy[1];
            float p2 = 9.54907046918287e-01f;
            float p1 = 8.87465224972323e-02;
            float xf      = fabsf(gamma);
            float tau_hat = copysignf(p2*xf*xf + p1*xf, gamma);
            int   num_samples = round(tau_hat * _q->k);
            liquid_log_trace("fskframesync, timing offset estimate  : %12.8f -> %12.8f (%d samples)", gamma, tau_hat, num_samples);
#endif

            // TODO: set timer and filterbank index accordingly
            _q->timer = 2*_q->k;

            // update state...
            _q->state = STATE_RXHEADER;
        } else {
            //printf("signal not yet peaked...\n");
        }
    }
    return LIQUID_OK;
}

int fskframesync_execute_rxheader(fskframesync  _q,
                                  float complex _x)
{
#if 0
    // push sample through timing recovery and compute output
    float complex y;
    firpfb_crcf_push(_q->pfb, _x);
    firpfb_crcf_execute(_q->pfb, 0, &y);

    // push sample into pre-demod p/n sequence buffer
    windowcf_push(_q->buf_rx, y);
#else
    windowcf_push(_q->buf_rx, _x);
#endif

    // decrement timer and determine if symbol output is ready
    _q->timer--;
    if (_q->timer)
        return LIQUID_OK;

    // reset timer
    _q->timer = _q->k;

    // run demodulator
    float complex * r;
    windowcf_read(_q->buf_rx, &r);
    unsigned char sym = fskdem_demodulate(_q->dem_header, r);

    // add symbol to header buffer
    _q->header_sym[_q->symbol_counter++] = sym;

    // decode header if appropriate
    if (_q->symbol_counter == _q->header_sym_len)
    {
        // decode header
        int rc = fskframesync_decode_header(_q);
        if (rc) return rc;

        // update statistics
        //_q->framedatastats.num_frames_detected++;

        if (_q->header_valid) {
            // continue on to decoding payload
            _q->symbol_counter = 0;
            _q->state = STATE_RXPAYLOAD;
            return LIQUID_OK;
        }

        // header invalid: invoke callback if provided
        if (_q->callback != NULL) {
            // set framestats internals
            _q->framestats.evm           = 0.0f; //20*log10f(sqrtf(_q->framestats.evm / 600));
            _q->framestats.rssi          = 0.0f; //20*log10f(_q->gamma_hat);
            _q->framestats.cfo           = 0.0f; //nco_crcf_get_frequency(_q->mixer);
            _q->framestats.framesyms     = NULL;
            _q->framestats.num_framesyms = 0;
            _q->framestats.mod_scheme    = LIQUID_MODEM_UNKNOWN;
            _q->framestats.mod_bps       = 0;
            _q->framestats.check         = LIQUID_CRC_UNKNOWN;
            _q->framestats.fec0          = LIQUID_FEC_UNKNOWN;
            _q->framestats.fec1          = LIQUID_FEC_UNKNOWN;

            // invoke callback method
            _q->callback(_q->header_dec,
                         0,     // header valid
                         NULL,  // payload
                         0,     // payload length
                         0,     // payload valid,
                         _q->framestats,
                         _q->userdata);
        }

        // reset frame synchronizer
        fskframesync_reset(_q);
    }
    return LIQUID_OK;
}

int fskframesync_execute_rxpayload(fskframesync  _q,
                                   float complex _x)
{
#if 0
    // push sample through timing recovery and compute output
    float complex y;
    firpfb_crcf_push(_q->pfb, _x);
    firpfb_crcf_execute(_q->pfb, 0, &y);

    // push sample into pre-demod p/n sequence buffer
    windowcf_push(_q->buf_rx, y);
#else
    windowcf_push(_q->buf_rx, _x);
#endif

    // decrement timer and determine if symbol output is ready
    _q->timer--;
    if (_q->timer)
        return LIQUID_OK;

    // reset timer
    _q->timer = _q->k;

    // run demodulator
    float complex * r;
    windowcf_read(_q->buf_rx, &r);
    unsigned char sym = fskdem_demodulate(_q->dem, r);

    // add symbol to payload buffer
    _q->payload_sym[_q->symbol_counter++] = sym;

    // decode payload if appropriate
    if (_q->symbol_counter == _q->payload_sym_len) {
#if 0
        printf("rx payload symbols (%u)\n", _q->payload_sym_len);
        unsigned int i;
        for (i=0; i<_q->payload_sym_len; i++)
            printf("%1x%s", _q->payload_sym[i], ((i+1)%64)==0 ? "\n" : "");
        printf("\n");
#endif
        // decode payload
        int payload_valid = qpacketmodem_decode_syms(_q->payload_decoder,
                                                     _q->payload_sym,
                                                     _q->payload_dec);
        //printf("payload: %s\n", payload_valid ? "valid" : "INVALID");
        
        // invoke callback
        if (_q->callback != NULL) {
            // set framestats internals
            _q->framestats.evm           = 0.0f; //20*log10f(sqrtf(_q->framestats.evm / 600));
            _q->framestats.rssi          = 0.0f; //20*log10f(_q->gamma_hat);
            _q->framestats.cfo           = 0.0f; //nco_crcf_get_frequency(_q->mixer);
            _q->framestats.framesyms     = NULL;
            _q->framestats.num_framesyms = 0;
            _q->framestats.mod_scheme    = LIQUID_MODEM_UNKNOWN;
            _q->framestats.mod_bps       = 0;
            _q->framestats.check         = _q->payload_crc;
            _q->framestats.fec0          = _q->payload_fec0;
            _q->framestats.fec1          = _q->payload_fec1;

            // invoke callback method
            _q->callback(_q->header_dec,        // decoded header
                         1,                     // header valid
                         _q->payload_dec,       // payload
                         _q->payload_dec_len,   // payload length
                         payload_valid,         // payload valid,
                         _q->framestats,
                         _q->userdata);
        }

        // reset frame synchronizer
        return fskframesync_reset(_q);
    }
    return LIQUID_OK;
}

// decode header and re-configure payload decoder
int fskframesync_decode_header(fskframesync _q)
{
    // decode header
    _q->header_valid = qpacketmodem_decode_syms(_q->header_decoder,
                                                _q->header_sym,
                                                _q->header_dec);

    liquid_log_debug("fskframesync header: %s",_q->header_valid ? "valid" : "INVALID");
    if (!_q->header_valid)
        return LIQUID_OK;

    // unpack header
    unsigned int n = 8; // first 8 bytes user data
    n++;                // version
    _q->payload_dec_len = _q->header_dec[n++];
    _q->payload_dec_len <<= 8;
    _q->payload_dec_len |= _q->header_dec[n++];
    liquid_log_trace("fskframesync header: [%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x][%.2x:%.2x %.2x]",
        _q->header_dec[0],_q->header_dec[1],_q->header_dec[2],_q->header_dec[3],_q->header_dec[4],_q->header_dec[5],_q->header_dec[6],_q->header_dec[7],
        _q->header_dec[8],
        _q->header_dec[9],_q->header_dec[10]);
    liquid_log_debug("fskframesync header: payload_dec_len=%u", _q->payload_dec_len);
    // expansion
    if (_q->payload_dec_len == 0 || _q->payload_dec_len > LIQUID_MAX_PAYLOAD_LEN)
        return liquid_error(LIQUID_EICONFIG,"fskframesync_decode_header(), payload length (%u) exceeds maximum (%u)", _q->payload_dec_len, LIQUID_MAX_PAYLOAD_LEN);

    // configure decoder appropriately
    int rc = qpacketmodem_configure(_q->payload_decoder,
                                    _q->payload_dec_len,
                                    _q->payload_crc,
                                    _q->payload_fec0,
                                    _q->payload_fec1,
                                    LIQUID_MODEM_QAM16);  // TODO: set bits/sym appropriately
    //printf("decoder: "); qpacketmodem_print(_q->payload_decoder);
    if (rc != LIQUID_OK)
        return liquid_error(LIQUID_EICONFIG,"fskframesync_decode_header(), could not configure qpacketmodem");

    // get packet length and re-allocate memory
    _q->payload_sym_len = qpacketmodem_get_frame_len(_q->payload_decoder);
    _q->payload_sym = (unsigned char*) realloc(_q->payload_sym, _q->payload_sym_len*sizeof(unsigned char));
    if (_q->payload_sym == NULL)
        return liquid_error(LIQUID_EINT,"fskframesync_assemble(), could not allocate memory for symbols");

    _q->header_valid = 1;
    return LIQUID_OK;
}

int fskframesync_debug_enable(fskframesync _q)
{
    // create debugging objects if necessary
#if DEBUG_FSKFRAMESYNC
    if (!_q->debug_objects_created) {
        _q->debug_x  = windowcf_create(DEBUG_FSKFRAMESYNC_BUFFER_LEN);
    }
    
    // set debugging flags
    _q->debug_enabled = 1;
    _q->debug_objects_created = 1;
    return LIQUID_OK;
#else
    return liquid_error(LIQUID_EICONFIG,"fskframesync_debug_enable(), compile-time debugging disabled\n");
#endif
}

int fskframesync_debug_disable(fskframesync _q)
{
#if DEBUG_FSKFRAMESYNC
    _q->debug_enabled = 0;
    return LIQUID_OK;
#else
    return liquid_error(LIQUID_EICONFIG,"fskframesync_debug_disable(), compile-time debugging disabled\n");
#endif
}

int fskframesync_debug_export(fskframesync _q,
                               const char * _filename)
{
#if DEBUG_FSKFRAMESYNC
    if (!_q->debug_objects_created)
        return liquid_error(LIQUID_EICONFIG,"fskframe_debug_print(), debugging objects don't exist; enable debugging first");

    FILE* fid = fopen(_filename,"w");
    if (fid == NULL)
        return liquid_error(LIQUID_EIO,"fskframesync_debug_print(), could not open '%s' for writing", _filename);

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
    return LIQUID_OK;
#else
    return liquid_error(LIQUID_EICONFIG,"fskframesync_debug_print(), compile-time debugging disabled\n");
#endif
}

