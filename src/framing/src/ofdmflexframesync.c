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

#define DEBUG_OFDMFLEXFRAMESYNC             1
#define DEBUG_OFDMFLEXFRAMESYNC_PRINT       0
#define DEBUG_OFDMFLEXFRAMESYNC_FILENAME    "ofdmflexframesync_internal_debug.m"
#define DEBUG_OFDMFLEXFRAMESYNC_BUFFER_LEN  (2048)

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
    unsigned char header[14];           // header data (uncoded)
    unsigned char header_enc[24];       // header data (encoded)
    unsigned char header_mod[96];       // header symbols
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

    // internal...
    ofdmframesync fs;       // internal OFDM frame synchronizer

    // counters/states
    unsigned int symbol_counter;        // received symbol number
    enum {
        OFDMFLEXFRAMESYNC_STATE_HEADER, // extract header
        OFDMFLEXFRAMESYNC_STATE_PAYLOAD // extract payload symbols
    } state;
    unsigned int header_symbol_index;   //
    unsigned int payload_symbol_index;  //
    unsigned int payload_buffer_index;  // bit-level index of payload
};

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
    q->M = _M;
    q->cp_len = _cp_len;

    // TODO : set callback data
    //q->callback = _callback;
    //q->userdata = _userdata;

    // validate and count subcarrier allocation
    ofdmframe_validate_sctype(_p, q->M, &q->M_null, &q->M_pilot, &q->M_data);

    // allocate memory for subcarrier allocation IDs
    q->p = (unsigned int*) malloc((q->M)*sizeof(unsigned int));
    if (_p == NULL) {
        // initialize default subcarrier allocation
        ofdmframe_init_default_sctype(q->M, q->p);
    } else {
        // copy user-defined subcarrier allocation
        memmove(q->p, _p, q->M*sizeof(unsigned int));
    }

    // create internal framing object
    q->fs = ofdmframesync_create(_M, _cp_len, _p, ofdmflexframesync_internal_callback, (void*)q);

    // create header objects
    q->mod_header = modem_create(LIQUID_MODEM_QPSK, 2);
    q->p_header   = packetizer_create(14, LIQUID_CRC_16, LIQUID_FEC_HAMMING128, LIQUID_FEC_NONE);
    assert(packetizer_get_enc_msg_len(q->p_header)==24);

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
    _q->symbol_counter=0;
    _q->state = OFDMFLEXFRAMESYNC_STATE_HEADER;
    _q->header_symbol_index=0;
    _q->payload_symbol_index=0;
    _q->payload_buffer_index=0;

    // reset internal OFDM frame synchronizer object
    ofdmframesync_reset(_q->fs);
}

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
//  _p          :
//  _M          :
//  _userdata   :
int ofdmflexframesync_internal_callback(float complex * _X,
                                        unsigned int  * _p,
                                        unsigned int    _M,
                                        void * _userdata)
{
#if DEBUG_OFDMFLEXFRAMESYNC_PRINT
    printf("******* ofdmflexframesync callback invoked!\n");
#endif
    // type-cast userdata as ofdmflexframesync object
    ofdmflexframesync _q = (ofdmflexframesync) _userdata;

    _q->symbol_counter++;

#if DEBUG_OFDMFLEXFRAMESYNC_PRINT
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
#if DEBUG_OFDMFLEXFRAMESYNC_PRINT
    printf("  ofdmflexframesync extracting header...\n");
#endif

#if 0
    // header (QPSK) REFERENCE
    modem mod_header;                   // header QPSK modulator
    packetizer p_header;                // header packetizer
    unsigned char header[14];           // header data (uncoded)
    unsigned char header_enc[24];       // header data (encoded)
    unsigned char header_mod[96];       // header symbols
#endif

    // demodulate header symbols
    unsigned int i;
    int sctype;
    for (i=0; i<_q->M; i++) {
        //
        sctype = _q->p[i];

        // ignore pilot and null subcarriers
        if (sctype == OFDMFRAME_SCTYPE_DATA) {
            // unload header symbols
            //printf("  extracting symbol %u / %u\n", _q->header_symbol_index, 96);
            // demodulate header symbol
            unsigned int sym;
            modem_demodulate(_q->mod_header, _X[i], &sym);
            _q->header_mod[_q->header_symbol_index++] = sym;

            if (_q->header_symbol_index == 96) {
                //printf("  ***** header extracted!\n");
                ofdmflexframesync_decode_header(_q);
                if (_q->header_valid)
                    _q->state = OFDMFLEXFRAMESYNC_STATE_PAYLOAD;
                else
                    ofdmflexframesync_reset(_q);
                break;
            }
        }
    }
}

// decode header
void ofdmflexframesync_decode_header(ofdmflexframesync _q)
{
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

    // run packet decoder
    _q->header_valid = packetizer_decode(_q->p_header, _q->header_enc, _q->header);

#if 0
    // print header
    printf("header rx (enc) : ");
    for (i=0; i<24; i++)
        printf("%.2X ", _q->header_enc[i]);
    printf("\n");

    // print header
    printf("header rx (dec) : ");
    for (i=0; i<14; i++)
        printf("%.2X ", _q->header[i]);
    printf("\n");
#endif
    printf("****** header extracted [%s]\n", _q->header_valid ? "valid" : "INVALID!");
    if (!_q->header_valid)
        return;

    // TODO : return if header is invalid

    // strip off payload length
    unsigned int payload_len = (_q->header[8] << 8) | (_q->header[9]);
    //_q->payload_len = payload_len;

    // strip off modulation scheme/depth
    //  mod. scheme : most-significant five bits
    //  mod. depth  : least-significant three bits (+1)
    unsigned int mod_scheme = ( _q->header[10] >> 3) & 0x1f;
    unsigned int mod_depth  = ((_q->header[10]     ) & 0x07)+1;

    // strip off CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [11]
    //  fec0    : least-significant 5 bits of [11]
    //  fec1    : least-significant 5 bits of [12]
    unsigned int check = (_q->header[11] >> 5 ) & 0x07;
    unsigned int fec0  = (_q->header[11]      ) & 0x1f;
    unsigned int fec1  = (_q->header[12]      ) & 0x1f;

    // last byte is for expansion/version validation
    if (_q->header[13] != OFDMFLEXFRAME_VERSION) {
        fprintf(stderr,"warning: ofdmflexframesync_decode_header(), invalid framing version\n");
        _q->header_valid = 0;
    }

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
#if DEBUG_OFDMFLEXFRAMESYNC_PRINT
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
#if DEBUG_OFDMFLEXFRAMESYNC_PRINT
        printf("      * payload encoded :   %u bytes\n", _q->payload_enc_len);
#endif

        // re-allocate buffers accordingly
        _q->payload_enc = (unsigned char*) realloc(_q->payload_enc, _q->payload_enc_len*sizeof(unsigned char));
        _q->payload_dec = (unsigned char*) realloc(_q->payload_dec, _q->payload_len*sizeof(unsigned char));

        // re-compute number of modulated payload symbols
        div_t d = div(8*_q->payload_enc_len, _q->bps_payload);
        _q->payload_mod_len = d.quot + (d.rem ? 1 : 0);
#if DEBUG_OFDMFLEXFRAMESYNC_PRINT
        printf("      * payload mod syms:   %u symbols\n", _q->payload_mod_len);
#endif
    }
}

// receive payload data
void ofdmflexframesync_rxpayload(ofdmflexframesync _q,
                                 float complex * _X)
{
#if DEBUG_OFDMFLEXFRAMESYNC_PRINT
    printf("  ofdmflexframesync extracting payload...\n");
#endif

    // demodulate paylod symbols
    unsigned int i;
    int sctype;
    for (i=0; i<_q->M; i++) {
        //
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
                printf("****** payload extracted [%s]\n", _q->payload_valid ? "valid" : "INVALID!");

                // TODO : invoke callback function

                // reset object...
#if DEBUG_OFDMFLEXFRAMESYNC_PRINT
                printf("  ... resetting ofdmflexframesync object...\n");
#endif
                ofdmflexframesync_reset(_q);
                break;
            }
        }
    }
}



