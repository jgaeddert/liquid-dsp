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
// ofdmflexframegen.c
//
// OFDM flexible frame generator
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_OFDMFLEXFRAMEGEN            1

// default ofdmflexframegen properties
static ofdmflexframegenprops_s ofdmflexframegenprops_default = {
    3,                  // num_symbols_S0
    0,                  // payload_len
    LIQUID_CRC_16,      // check
    LIQUID_FEC_NONE,    // fec0
    LIQUID_FEC_NONE,    // fec1
    LIQUID_MODEM_QPSK,  // mod_scheme
    2                   // mod_bps
    //64                // block_size
};

void ofdmflexframegenprops_init_default(ofdmflexframegenprops_s * _props)
{
    memmove(_props, &ofdmflexframegenprops_default, sizeof(ofdmflexframegenprops_s));
}

struct ofdmflexframegen_s {
    unsigned int M;         // number of subcarriers
    unsigned int cp_len;    // cyclic prefix length
    unsigned int * p;       // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1

    // buffers
    float complex * X;      // frequency-domain buffer
    float complex * x;      // time-domain buffer

    // internal low-level objects
    ofdmframegen fg;        // frame generator object

    // options/derived lengths
    unsigned int num_symbols_S0;        // 
    unsigned int num_symbols_S1;        // 
    unsigned int num_symbols_header;    // number of header OFDM symbols
    unsigned int num_symbols_payload;   // number of payload OFDM symbols

    // header (QPSK)
    modem mod_header;                   // header QPSK modulator
    packetizer p_header;                // header packetizer
    unsigned char header[14];           // header data (uncoded)
    unsigned char header_enc[24];       // header data (encoded)
    unsigned char header_sym[96];       // header symbols
    float complex header_samples[96];   // header samples

    // payload
    packetizer p_payload;               // payload packetizer
    modem mod_payload;                  // payload modulator
    unsigned char * payload_enc;        // payload data (encoded bytes)
    unsigned int payload_enc_msg_len;   // length of encoded payload
    unsigned int payload_modem_symbols; // number of modulated symbols in payload

    // properties
    ofdmflexframegenprops_s props;
};

// TODO : put these options in 'assemble()' method?
ofdmflexframegen ofdmflexframegen_create(unsigned int _M,
                                         unsigned int _cp_len,
                                         unsigned int * _p,
                                         ofdmflexframegenprops_s * _props)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: ofdmflexframegen_create(), number of subcarriers must be at least 2\n");
        exit(1);
    } else if (_M % 2) {
        fprintf(stderr,"error: ofdmflexframegen_create(), number of subcarriers must be even\n");
        exit(1);
    }

    ofdmflexframegen q = (ofdmflexframegen) malloc(sizeof(struct ofdmflexframegen_s));
    q->M = _M;
    q->cp_len = _cp_len;

    // allocate memory for transform buffers
    q->X = (float complex*) malloc((q->M)*sizeof(float complex));
    q->x = (float complex*) malloc((q->M)*sizeof(float complex));

    // validate and count subcarrier allocation
    ofdmframe_validate_sctype(_p, q->M, &q->M_null, &q->M_pilot, &q->M_data);

    // create internal OFDM frame generator object
    q->fg = ofdmframegen_create(_M, _cp_len, _p);

    // create header objects
    q->mod_header = modem_create(LIQUID_MODEM_QPSK, 2);
    q->p_header   = packetizer_create(14, LIQUID_CRC_16, LIQUID_FEC_HAMMING128, LIQUID_FEC_NONE);
    assert(packetizer_get_enc_msg_len(q->p_header)==24);

    // compute number of header symbols
    div_t d = div(96, q->M_data);
    q->num_symbols_header = d.quot + (d.rem ? 1 : 0);

    // initial memory allocation for payload
    q->p_payload = packetizer_create(0, LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE);
    q->payload_enc = (unsigned char*) malloc(1*sizeof(unsigned char));

    // create payload modem (initially QPSK, overridden by properties)
    q->mod_payload = modem_create(LIQUID_MODEM_QPSK, 1);

    // initialize properties
    if (_props != NULL)
        ofdmflexframegen_setprops(q, _props);
    else
        ofdmflexframegen_setprops(q, &ofdmflexframegenprops_default);

    return q;
}

void ofdmflexframegen_destroy(ofdmflexframegen _q)
{
    // destroy internal objects
    ofdmframegen_destroy(_q->fg);       // OFDM frame generator
    packetizer_destroy(_q->p_header);   // header packetizer
    modem_destroy(_q->mod_header);      // header modulator
    packetizer_destroy(_q->p_payload);  // payload packetizer
    modem_destroy(_q->mod_payload);     // payload modulator

    // free transform array memory
    free(_q->X);                        // frequency-domain buffer
    free(_q->x);                        // time-domain buffer

    // free main object memory
    free(_q);
}

// get ofdmflexframegen properties
//  _q      :   frame generator object
//  _props  :   frame generator properties structure pointer
void ofdmflexframegen_getprops(ofdmflexframegen _q,
                               ofdmflexframegenprops_s * _props)
{
    // copy properties structure to output pointer
    memmove(_props, &_q->props, sizeof(ofdmflexframegenprops_s));
}

void ofdmflexframegen_setprops(ofdmflexframegen _q,
                               ofdmflexframegenprops_s * _props)
{
    // validate input
    if (_props->mod_bps == 0) {
        fprintf(stderr, "error: ofdmflexframegen_setprops(), modulation depth must be greater than 0\n");
        exit(1);
    } else if (_props->check == LIQUID_CRC_UNKNOWN || _props->check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr, "error: ofdmflexframegen_setprops(), invalid/unsupported CRC scheme\n");
        exit(1);
    } else if (_props->fec0 == LIQUID_FEC_UNKNOWN || _props->fec1 == LIQUID_FEC_UNKNOWN) {
        fprintf(stderr, "error: ofdmflexframegen_setprops(), invalid/unsupported FEC scheme\n");
        exit(1);
    } else if (_props->mod_scheme == LIQUID_MODEM_UNKNOWN ) {
        fprintf(stderr, "error: ofdmflexframegen_setprops(), invalid/unsupported modulation scheme\n");
        exit(1);
    }

    // copy properties to internal structure
    memmove(&_q->props, _props, sizeof(ofdmflexframegenprops_s));

    // re-create payload packetizer
    _q->p_payload = packetizer_recreate(_q->p_payload,
                                        _q->props.payload_len,
                                        _q->props.check,
                                        _q->props.fec0,
                                        _q->props.fec1);
    _q->payload_enc_msg_len = packetizer_get_enc_msg_len(_q->p_payload);

    // re-create modem
    modem_destroy(_q->mod_payload);
    _q->mod_payload = modem_create(_q->props.mod_scheme, _q->props.mod_bps);

    // 
    // re-compute payload length
    //

    // compute number of payload modem symbols
    div_t d = div(8*_q->payload_enc_msg_len, _q->props.mod_bps);
    _q->payload_modem_symbols = d.quot + (d.rem ? 1 : 0);

    // compute number of payload OFDM symbols
    d = div(_q->payload_modem_symbols, _q->M_data);
    _q->num_symbols_payload = d.quot + (d.rem ? 1 : 0);

#if 0
    // re-compute payload and frame lengths
    ofdmflexframegen_compute_payload_len(_q);
    ofdmflexframegen_compute_frame_len(_q);

    // reconfigure payload buffers (reallocate as necessary)
    ofdmflexframegen_configure_payload_buffers(_q);
#endif
}

void ofdmflexframegen_print(ofdmflexframegen _q)
{
    printf("ofdmflexframegen:\n");
    printf("    num subcarriers     :   %-u\n", _q->M);
    printf("      * NULL            :   %-u\n", _q->M_null);
    printf("      * pilot           :   %-u\n", _q->M_pilot);
    printf("      * data            :   %-u\n", _q->M_data);
    printf("    cyclic prefix len   :   %-u\n", _q->cp_len);
    printf("    properties:\n");
    printf("      * mod scheme      :   %s (%u b/s)\n", modulation_scheme_str[_q->props.mod_scheme][1], _q->props.mod_bps);
    printf("      * fec (inner)     :   %s\n", fec_scheme_str[_q->props.fec0][1]);
    printf("      * fec (outer)     :   %s\n", fec_scheme_str[_q->props.fec1][1]);
    printf("      * CRC scheme      :   %s\n", crc_scheme_str[_q->props.check][1]);
    printf("    lengths:\n");
    printf("      * payload bytes (dec) :   %-u\n", _q->props.payload_len);
    printf("      * payload bytes (enc) :   %-u\n", _q->payload_enc_msg_len);
    printf("      * payload modem syms  :   %-u\n", _q->payload_modem_symbols);
    printf("    total OFDM symbols  :   %-u\n", ofdmflexframegen_getframelen(_q));
    printf("      * S0 symbols      :   %-u @ %u\n", _q->props.num_symbols_S0, _q->M);
    printf("      * S1 symbols      :   %-u @ %u\n", 1, _q->M+_q->cp_len);
    printf("      * header symbols  :   %-u @ %u\n", _q->num_symbols_header,  _q->M+_q->cp_len);
    printf("      * payload symbols :   %-u @ %u\n", _q->num_symbols_payload, _q->M+_q->cp_len);
}

// get length of frame (symbols)
//  _q              :   OFDM frame generator object
unsigned int ofdmflexframegen_getframelen(ofdmflexframegen _q)
{
    // number of S0 symbols
    // number of S1 symbols (1)
    // number of header symbols
    // number of payload symbols
    return  _q->props.num_symbols_S0 +
            1 +
            _q->num_symbols_header +
            _q->num_symbols_payload;
}

// assemble a frame from an array of data
//  _q              :   OFDM frame generator object
//  _header         :   frame header [8 bytes]
//  _payload        :   payload data
void ofdmflexframegen_assemble(ofdmflexframegen _q,
                               unsigned char * _header,
                               unsigned char * _payload)
{
    // copy header

    // compute number of data symbols...
}

// write symbols of assembled frame
//  _q              :   OFDM frame generator object
//  _buffer         :   output buffer [size: N+cp_len x 1]
//  _num_written    :   number written (either N or N+cp_len)
int ofdmflexframegen_writesymbol(ofdmflexframegen _q,
                                 liquid_float_complex * _buffer,
                                 unsigned int * _num_written)
{
    // write S0 symbols
    // write S1 symbols
    // write header symbols
    // write payload symbols

    return 1;
}


//
// internal
//

// compute payload length (number of modulation symbols)
void ofdmflexframegen_compute_payload_len(ofdmflexframegen _q)
{
#if 0
    // compute integer division, keeping track of remainder
    div_t d = div(8*_q->payload_enc_msg_len, _q->props.mod_bps);

    // extend number of payload symbols if remainder is present
    _q->num_payload_symbols = d.quot + (d.rem ? 1 : 0);
#endif
}

// encode header
void ofdmflexframegen_encode_header(ofdmflexframegen _q)
{
    // first 8 bytes user data

    // add payload length
    unsigned int payload_len = 0;
    _q->header[8] = (payload_len >> 8) & 0xff;
    _q->header[9] = (payload_len     ) & 0xff;

    // add modulation scheme/depth (pack into single byte)
    //  mod. scheme : most-significant five bits
    //  mod. depth  : least-significant three bits (-1)
    modulation_scheme mod_scheme = LIQUID_MODEM_QPSK;
    unsigned int mod_bps = 2;
    _q->header[10]  = ( mod_scheme & 0x1f) << 3;
    _q->header[10] |= ((mod_bps-1) & 0x07);

    // add CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [17]
    //  fec0    : least-significant 5 bits of [17]
    //  fec1    : least-significant 5 bits of [18]
    crc_scheme check = LIQUID_CRC_32;
    fec_scheme fec0  = LIQUID_FEC_NONE;
    fec_scheme fec1  = LIQUID_FEC_NONE;
    _q->header[11]  = (check & 0x07) << 5;
    _q->header[11] |= (fec0) & 0x1f;
    _q->header[12]  = (fec1) & 0x1f;

    // last byte is for expansion/version validation
    _q->header[13] = 0;

    // scramble header
    scramble_data(_q->header, 14);

    // run packet encoder
    packetizer_encode(_q->p_header, _q->header, _q->header_enc);
}

// modulate header
void ofdmflexframegen_modulate_header(ofdmflexframegen _q)
{
    unsigned int i;

    // unpack header symbols
    for (i=0; i<24; i++) {
        _q->header_sym[4*i+0] = (_q->header_enc[i] >> 6) & 0x03;
        _q->header_sym[4*i+1] = (_q->header_enc[i] >> 4) & 0x03;
        _q->header_sym[4*i+2] = (_q->header_enc[i] >> 2) & 0x03;
        _q->header_sym[4*i+3] = (_q->header_enc[i]     ) & 0x03;
    }

    // modulate symbols
    for (i=0; i<96; i++)
        modem_modulate(_q->mod_header, _q->header_sym[i], &_q->header_samples[i]);
}

// modulate payload
void ofdmflexframegen_modulate_payload(ofdmflexframegen _q)
{
}


