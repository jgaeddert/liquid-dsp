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
// ofdmflexframesync.c
//
// OFDM frame synchronizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_OFDMFLEXFRAMESYNC 0

#if DEBUG_OFDMFLEXFRAMESYNC
void ofdmflexframesync_debug_print(ofdmflexframesync _q);
#endif

struct ofdmflexframesync_s {
    unsigned int M;         // number of subcarriers
    unsigned int cp_len;    // cyclic prefix length
    unsigned int * p;       // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1

    // header (QPSK)
    modem mod_header;                   // header QPSK modulator
    packetizer p_header;                // header packetizer
    unsigned char header[19];           // header data (uncoded)
    unsigned char header_enc[32];       // header data (encoded)
    unsigned char header_mod[256];      // header symbols
    int header_valid;                   // valid header flag

    // header properties
    modulation_scheme ms_payload;       // payload modulation scheme
    unsigned int bps_payload;           // payload modulation depth (bits/symbol)
    unsigned int payload_len;           // payload length (number of bytes)
    crc_scheme check;                   // payload validity check
    fec_scheme fec0;                    // payload FEC (inner)
    fec_scheme fec1;                    // payload FEC (outer)

    // payload
    packetizer p_payload;               // payload packetizer
    modem mod_payload;                  // payload demodulator
    unsigned char * payload_enc;        // payload data (encoded bytes)
    unsigned char * payload_dec;        // payload data (decoded bytes)
    unsigned int payload_enc_len;       // length of encoded payload
    unsigned int payload_mod_len;       // number of payload modem symbols
    int payload_valid;                  // valid payload flag

    // callback
    ofdmflexframesync_callback callback;// user-defined callback function
    void * userdata;                    // user-defined data structure
    framesyncstats_s framestats;        // frame statistic object

    // internal synchronizer objects
    ofdmframesync fs;                   // internal OFDM frame synchronizer

    // counters/states
    unsigned int symbol_counter;        // received symbol number
    enum {
        OFDMFLEXFRAMESYNC_STATE_HEADER, // extract header
        OFDMFLEXFRAMESYNC_STATE_PAYLOAD // extract payload symbols
    } state;
    unsigned int header_symbol_index;   // number of header symbols received
    unsigned int payload_symbol_index;  // number of payload symbols received
    unsigned int payload_buffer_index;  // bit-level index of payload (pack array)
};

// create ofdmflexframesync object
//  _M          :   number of subcarriers
//  _cp_len     :   length of cyclic prefix [samples]
//  _p          :   subcarrier allocation (PILOT/NULL/DATA) [size: _M x 1]
//  _callback   :   user-defined callback function
//  _userdata   :   user-defined data structure passed to callback
ofdmflexframesync ofdmflexframesync_create(unsigned int _M,
                                           unsigned int _cp_len,
                                           unsigned int * _p,
                                           //unsigned int _taper_len,
                                           ofdmflexframesync_callback _callback,
                                           void * _userdata)
{
    ofdmflexframesync q = (ofdmflexframesync) malloc(sizeof(struct ofdmflexframesync_s));

    // validate input
    if (_M < 8) {
        fprintf(stderr,"warning: ofdmflexframesync_create(), less than 8 subcarriers\n");
    } else if (_M % 2) {
        fprintf(stderr,"error: ofdmflexframesync_create(), number of subcarriers must be even\n");
        exit(1);
    } else if (_cp_len > _M) {
        fprintf(stderr,"error: ofdmflexframesync_create(), cyclic prefix length cannot exceed number of subcarriers\n");
        exit(1);
    }

    // set internal properties
    q->M        = _M;
    q->cp_len   = _cp_len;
    q->callback = _callback;
    q->userdata = _userdata;

    // allocate memory for subcarrier allocation IDs
    q->p = (unsigned int*) malloc((q->M)*sizeof(unsigned int));
    if (_p == NULL) {
        // initialize default subcarrier allocation
        ofdmframe_init_default_sctype(q->M, q->p);
    } else {
        // copy user-defined subcarrier allocation
        memmove(q->p, _p, q->M*sizeof(unsigned int));
    }

    // validate and count subcarrier allocation
    ofdmframe_validate_sctype(q->p, q->M, &q->M_null, &q->M_pilot, &q->M_data);

    // create internal framing object
    q->fs = ofdmframesync_create(_M, _cp_len, _p, ofdmflexframesync_internal_callback, (void*)q);

    // create header objects
    q->mod_header = modem_create(LIQUID_MODEM_BPSK, 1);
    q->p_header   = packetizer_create(19, LIQUID_CRC_16, LIQUID_FEC_HAMMING128, LIQUID_FEC_NONE);
    assert(packetizer_get_enc_msg_len(q->p_header)==32);

    // frame properties (default values to be overwritten when frame
    // header is received and properly decoded)
    q->ms_payload   = LIQUID_MODEM_QPSK;
    q->bps_payload  = 2;
    q->payload_len  = 1;
    q->check        = LIQUID_CRC_NONE;
    q->fec0         = LIQUID_FEC_NONE;
    q->fec1         = LIQUID_FEC_NONE;

    // create payload objects (initally QPSK, etc but overridden by received properties)
    q->mod_payload = modem_create(q->ms_payload, q->bps_payload);
    q->p_payload   = packetizer_create(q->payload_len, q->check, q->fec0, q->fec1);
    q->payload_enc_len = packetizer_get_enc_msg_len(q->p_payload);
    q->payload_enc = (unsigned char*) malloc(q->payload_enc_len*sizeof(unsigned char));
    q->payload_dec = (unsigned char*) malloc(q->payload_len*sizeof(unsigned char));
    q->payload_mod_len = 0;

    // reset state
    ofdmflexframesync_reset(q);

    // return object
    return q;
}

void ofdmflexframesync_destroy(ofdmflexframesync _q)
{
    // destroy internal objects
    ofdmframesync_destroy(_q->fs);
    packetizer_destroy(_q->p_header);
    modem_destroy(_q->mod_header);
    packetizer_destroy(_q->p_payload);
    modem_destroy(_q->mod_payload);

    // free internal buffers/arrays
    free(_q->p);
    free(_q->payload_enc);
    free(_q->payload_dec);

    // free main object memory
    free(_q);
}

void ofdmflexframesync_print(ofdmflexframesync _q)
{
    printf("ofdmflexframesync:\n");
}

void ofdmflexframesync_reset(ofdmflexframesync _q)
{
    // reset internal state
    _q->state = OFDMFLEXFRAMESYNC_STATE_HEADER;

    // reset internal counters
    _q->symbol_counter=0;
    _q->header_symbol_index=0;
    _q->payload_symbol_index=0;
    _q->payload_buffer_index=0;

    // reset internal OFDM frame synchronizer object
    ofdmframesync_reset(_q->fs);
}

// execute synchronizer object on buffer of samples
void ofdmflexframesync_execute(ofdmflexframesync _q,
                               float complex * _x,
                               unsigned int _n)
{
    // push samples through ofdmframesync object
    ofdmframesync_execute(_q->fs, _x, _n);
}

//
// internal methods
//

// internal callback
//  _X          :   subcarrier symbols
//  _p          :   subcarrier allocation
//  _M          :   number of subcarriers
//  _userdata   :   user-defined data structure
int ofdmflexframesync_internal_callback(float complex * _X,
                                        unsigned int  * _p,
                                        unsigned int    _M,
                                        void * _userdata)
{
#if DEBUG_OFDMFLEXFRAMESYNC
    printf("******* ofdmflexframesync callback invoked!\n");
#endif
    // type-cast userdata as ofdmflexframesync object
    ofdmflexframesync _q = (ofdmflexframesync) _userdata;

    _q->symbol_counter++;

#if DEBUG_OFDMFLEXFRAMESYNC
    printf("received symbol %u\n", _q->symbol_counter);
#endif

    // extract symbols
    switch (_q->state) {
    case OFDMFLEXFRAMESYNC_STATE_HEADER:
        ofdmflexframesync_rxheader(_q, _X);
        break;
    case OFDMFLEXFRAMESYNC_STATE_PAYLOAD:
        ofdmflexframesync_rxpayload(_q, _X);
        break;
    default:
        fprintf(stderr,"error: ofdmflexframesync_internal_callback(), unknown/unsupported internal state\n");
        exit(1);
    }

    // return
    return 0;
}

// receive header data
void ofdmflexframesync_rxheader(ofdmflexframesync _q,
                                float complex * _X)
{
#if DEBUG_OFDMFLEXFRAMESYNC
    printf("  ofdmflexframesync extracting header...\n");
#endif

    // demodulate header symbols
    unsigned int i;
    int sctype;
    for (i=0; i<_q->M; i++) {
        // subcarrier type (PILOT/NULL/DATA)
        sctype = _q->p[i];

        // ignore pilot and null subcarriers
        if (sctype == OFDMFRAME_SCTYPE_DATA) {
            // unload header symbols
            // demodulate header symbol
            unsigned int sym;
            modem_demodulate(_q->mod_header, _X[i], &sym);
            _q->header_mod[_q->header_symbol_index++] = sym;
            //printf("  extracting symbol %3u / %3u (x = %8.5f + j%8.5f)\n", _q->header_symbol_index, 256, crealf(_X[i]), cimagf(_X[i]));

            // header extracted
            if (_q->header_symbol_index == 256) {
                // decode header
                ofdmflexframesync_decode_header(_q);

                // TODO : invoke callback if header is invalid
                if (_q->header_valid)
                    _q->state = OFDMFLEXFRAMESYNC_STATE_PAYLOAD;
                else {
                    //printf("**** header invalid!\n");
                    // set framestats internals
                    _q->framestats.rssi             = ofdmframesync_get_rssi(_q->fs);
                    _q->framestats.framesyms        = NULL;
                    _q->framestats.num_framesyms    = 0;
                    _q->framestats.mod_scheme       = LIQUID_MODEM_UNKNOWN;
                    _q->framestats.mod_bps          = 0;
                    _q->framestats.check            = LIQUID_CRC_UNKNOWN;
                    _q->framestats.fec0             = LIQUID_FEC_UNKNOWN;
                    _q->framestats.fec1             = LIQUID_FEC_UNKNOWN;

                    // invoke callback method
                    _q->callback(_q->header,
                                 _q->header_valid,
                                 NULL,
                                 0,
                                 0,
                                 _q->framestats,
                                 _q->userdata);

                    ofdmflexframesync_reset(_q);
                }
                break;
            }
        }
    }
}

// decode header
void ofdmflexframesync_decode_header(ofdmflexframesync _q)
{
#if 0
    unsigned int i;

    // pack 96 2-bit header symbols into 24 8-bit bytes
    for (i=0; i<24; i++) {
        _q->header_enc[i] = 0x00;
        _q->header_enc[i] |= (_q->header_mod[4*i+0] << 6) & 0xc0;
        _q->header_enc[i] |= (_q->header_mod[4*i+1] << 4) & 0x30;
        _q->header_enc[i] |= (_q->header_mod[4*i+2] << 2) & 0x0c;
        _q->header_enc[i] |= (_q->header_mod[4*i+3]     ) & 0x03;
    }

    // unscramble header
    unscramble_data(_q->header_enc, 24);
#else
    // pack 256 1-bit header symbols into 32 8-bit bytes
    unsigned int num_written;
    liquid_pack_bytes(_q->header_mod, 256,
                      _q->header_enc, 32,
                      &num_written);
    assert(num_written==32);

    // unscramble header
    unscramble_data(_q->header_enc, 32);
#endif

    // run packet decoder
    _q->header_valid = packetizer_decode(_q->p_header, _q->header_enc, _q->header);

#if 0
    // print header
    printf("header rx (enc) : ");
    for (i=0; i<32; i++)
        printf("%.2X ", _q->header_enc[i]);
    printf("\n");

    // print header
    printf("header rx (dec) : ");
    for (i=0; i<14; i++)
        printf("%.2X ", _q->header[i]);
    printf("\n");
#endif

#if DEBUG_OFDMFLEXFRAMESYNC
    printf("****** header extracted [%s]\n", _q->header_valid ? "valid" : "INVALID!");
#endif
    if (!_q->header_valid)
        return;

    // first byte is for expansion/version validation
    if (_q->header[12] != OFDMFLEXFRAME_VERSION) {
        fprintf(stderr,"warning: ofdmflexframesync_decode_header(), invalid framing version\n");
        _q->header_valid = 0;
    }

    // strip off payload length
    unsigned int payload_len = (_q->header[14] << 8) | (_q->header[15]);

    // strip off modulation scheme/depth
    //  mod. scheme : most-significant five bits
    //  mod. depth  : least-significant three bits (+1)
    unsigned int mod_scheme = ( _q->header[16] >> 3) & 0x1f;
    unsigned int mod_depth  = ((_q->header[16]     ) & 0x07)+1;

    // strip off CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [17]
    //  fec0    : least-significant 5 bits of [17]
    //  fec1    : least-significant 5 bits of [18]
    unsigned int check = (_q->header[17] >> 5 ) & 0x07;
    unsigned int fec0  = (_q->header[17]      ) & 0x1f;
    unsigned int fec1  = (_q->header[18]      ) & 0x1f;

    // validate properties
    if (check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr,"warning: ofdmflexframesync_decode_header(), decoded CRC exceeds available\n");
        check = LIQUID_CRC_UNKNOWN;
        _q->header_valid = 0;
    }
    if (fec0 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,"warning: ofdmflexframesync_decode_header(), decoded FEC (inner) exceeds available\n");
        fec0 = LIQUID_FEC_UNKNOWN;
        _q->header_valid = 0;
    }
    if (fec1 >= LIQUID_FEC_NUM_SCHEMES) {
        fprintf(stderr,"warning: ofdmflexframesync_decode_header(), decoded FEC (outer) exceeds available\n");
        fec1 = LIQUID_FEC_UNKNOWN;
        _q->header_valid = 0;
    }

    // print results
#if DEBUG_OFDMFLEXFRAMESYNC
    printf("    properties:\n");
    printf("      * mod scheme      :   %s (%u b/s)\n", modulation_scheme_str[mod_scheme][1], mod_depth);
    printf("      * fec (inner)     :   %s\n", fec_scheme_str[fec0][1]);
    printf("      * fec (outer)     :   %s\n", fec_scheme_str[fec1][1]);
    printf("      * CRC scheme      :   %s\n", crc_scheme_str[check][1]);
    printf("      * payload length  :   %u bytes\n", payload_len);
#endif

    // configure payload receiver
    if (_q->header_valid) {
        // configure modem
        if (mod_scheme != _q->ms_payload || mod_depth != _q->bps_payload) {
            // set new properties
            _q->ms_payload  = mod_scheme;
            _q->bps_payload = mod_depth;

            // recreate modem (destroy/create)
            modem_destroy(_q->mod_payload);
            _q->mod_payload = modem_create(_q->ms_payload, _q->bps_payload);
        }

        // set new packetizer properties
        _q->payload_len = payload_len;
        _q->check       = check;
        _q->fec0        = fec0;
        _q->fec1        = fec1;
        
        // recreate packetizer object
        _q->p_payload = packetizer_recreate(_q->p_payload,
                                            _q->payload_len,
                                            _q->check,
                                            _q->fec0,
                                            _q->fec1);

        // re-compute payload encoded message length
        _q->payload_enc_len = packetizer_get_enc_msg_len(_q->p_payload);
#if DEBUG_OFDMFLEXFRAMESYNC
        printf("      * payload encoded :   %u bytes\n", _q->payload_enc_len);
#endif

        // re-allocate buffers accordingly
        _q->payload_enc = (unsigned char*) realloc(_q->payload_enc, _q->payload_enc_len*sizeof(unsigned char));
        _q->payload_dec = (unsigned char*) realloc(_q->payload_dec, _q->payload_len*sizeof(unsigned char));

        // re-compute number of modulated payload symbols
        div_t d = div(8*_q->payload_enc_len, _q->bps_payload);
        _q->payload_mod_len = d.quot + (d.rem ? 1 : 0);
#if DEBUG_OFDMFLEXFRAMESYNC
        printf("      * payload mod syms:   %u symbols\n", _q->payload_mod_len);
#endif
    }
}

// receive payload data
void ofdmflexframesync_rxpayload(ofdmflexframesync _q,
                                 float complex * _X)
{
    // demodulate paylod symbols
    unsigned int i;
    int sctype;
    for (i=0; i<_q->M; i++) {
        // subcarrier type (PILOT/NULL/DATA)
        sctype = _q->p[i];

        // ignore pilot and null subcarriers
        if (sctype == OFDMFRAME_SCTYPE_DATA) {
            // unload payload symbols
            unsigned int sym;
            modem_demodulate(_q->mod_payload, _X[i], &sym);

            // pack decoded symbol into array
            liquid_pack_array(_q->payload_enc,
                              _q->payload_enc_len,
                              _q->payload_buffer_index,
                              _q->bps_payload,
                              sym);

            // increment...
            _q->payload_buffer_index += _q->bps_payload;

            // increment symbol counter
            _q->payload_symbol_index++;

            if (_q->payload_symbol_index == _q->payload_mod_len) {
                // payload extracted

                // decode payload
                _q->payload_valid = packetizer_decode(_q->p_payload, _q->payload_enc, _q->payload_dec);
#if DEBUG_OFDMFLEXFRAMESYNC
                printf("****** payload extracted [%s]\n", _q->payload_valid ? "valid" : "INVALID!");
#endif

                // ignore callback if set to NULL
                if (_q->callback == NULL) {
                    ofdmflexframesync_reset(_q);
                    break;
                }

                // set framestats internals
                _q->framestats.rssi             = ofdmframesync_get_rssi(_q->fs);
                _q->framestats.framesyms        = NULL;
                _q->framestats.num_framesyms    = 0;
                _q->framestats.mod_scheme       = _q->ms_payload;
                _q->framestats.mod_bps          = _q->bps_payload;
                _q->framestats.check            = _q->check;
                _q->framestats.fec0             = _q->fec0;
                _q->framestats.fec1             = _q->fec1;

                // invoke callback method
                _q->callback(_q->header,
                             _q->header_valid,
                             _q->payload_dec,
                             _q->payload_len,
                             _q->payload_valid,
                             _q->framestats,
                             _q->userdata);


                // reset object
                ofdmflexframesync_reset(_q);
                break;
            }
        }
    }
}



