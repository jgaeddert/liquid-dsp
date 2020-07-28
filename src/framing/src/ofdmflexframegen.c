/*
 * Copyright (c) 2007 - 2017 Joseph Gaeddert
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

#define DEBUG_OFDMFLEXFRAMEGEN  0

// reconfigure internal buffers, objects, etc.
void ofdmflexframegen_reconfigure(ofdmflexframegen _q);

// encode header
void ofdmflexframegen_encode_header(ofdmflexframegen _q);

// modulate header
void ofdmflexframegen_modulate_header(ofdmflexframegen _q);

// generate samples of assembled frame (internally)
void ofdmflexframegen_gen_symbol (ofdmflexframegen _q); // (generic)
void ofdmflexframegen_gen_S0a    (ofdmflexframegen _q); // generate S0 symbol (first)
void ofdmflexframegen_gen_S0b    (ofdmflexframegen _q); // generate S0 symbol (second)
void ofdmflexframegen_gen_S1     (ofdmflexframegen _q); // generate S1 symbol
void ofdmflexframegen_gen_header (ofdmflexframegen _q); // generate header symbol
void ofdmflexframegen_gen_payload(ofdmflexframegen _q); // generate payload symbol
void ofdmflexframegen_gen_tail   (ofdmflexframegen _q); // generate tail symbol
void ofdmflexframegen_gen_zeros  (ofdmflexframegen _q); // generate zeros

// default ofdmflexframegen properties
static ofdmflexframegenprops_s ofdmflexframegenprops_default = {
    LIQUID_CRC_32,      // check
    LIQUID_FEC_NONE,    // fec0
    LIQUID_FEC_NONE,    // fec1
    LIQUID_MODEM_QPSK,  // mod_scheme
    //64                // block_size
};

static ofdmflexframegenprops_s ofdmflexframegenprops_header_default = {
    OFDMFLEXFRAME_H_CRC,
    OFDMFLEXFRAME_H_FEC0,
    OFDMFLEXFRAME_H_FEC1,
    OFDMFLEXFRAME_H_MOD,
};

void ofdmflexframegenprops_init_default(ofdmflexframegenprops_s * _props)
{
    memmove(_props, &ofdmflexframegenprops_default, sizeof(ofdmflexframegenprops_s));
}

struct ofdmflexframegen_s {
    unsigned int M;         // number of subcarriers
    unsigned int cp_len;    // cyclic prefix length
    unsigned int taper_len; // taper length
    unsigned char * p;      // subcarrier allocation (null, pilot, data)

    // constants
    unsigned int M_null;    // number of null subcarriers
    unsigned int M_pilot;   // number of pilot subcarriers
    unsigned int M_data;    // number of data subcarriers
    unsigned int M_S0;      // number of enabled subcarriers in S0
    unsigned int M_S1;      // number of enabled subcarriers in S1
    unsigned int frame_len; // frame length (M + cp_len)

    // buffers
    float complex * X;          // frequency-domain buffer
    float complex * buf_tx;     // transmit buffer
    unsigned int    buf_index;  // buffer index

    // internal low-level objects
    ofdmframegen fg;        // frame generator object

    // options/derived lengths
    unsigned int num_symbols_header;    // number of header OFDM symbols
    unsigned int num_symbols_payload;   // number of payload OFDM symbols

    // header
    modem mod_header;             // header modulator
    packetizer p_header;          // header packetizer
    unsigned char * header;       // header data (uncoded)
    unsigned char * header_enc;   // header data (encoded)
    unsigned char * header_mod;   // header symbols
    unsigned int header_user_len; // header length (user)
    unsigned int header_dec_len;  // header length (decoded)
    unsigned int header_enc_len;  // header length (encoded)
    unsigned int header_sym_len;  // header length (mod symbols)

    // payload
    packetizer p_payload;               // payload packetizer
    unsigned int payload_dec_len;       // payload length (num un-encoded bytes)
    modem mod_payload;                  // payload modulator
    unsigned char * payload_enc;        // payload data (encoded bytes)
    unsigned int * payload_mod;        // payload data (modulated symbols)
    unsigned int payload_enc_len;       // length of encoded payload
    unsigned int payload_mod_len;       // number of modulated symbols in payload

    // counters/states
    unsigned int symbol_number;         // output symbol number
    enum {
        OFDMFLEXFRAMEGEN_STATE_S0a=0,   // write S0 symbol (first)
        OFDMFLEXFRAMEGEN_STATE_S0b,     // write S0 symbol (second)
        OFDMFLEXFRAMEGEN_STATE_S1,      // write S1 symbol
        OFDMFLEXFRAMEGEN_STATE_HEADER,  // write header symbols
        OFDMFLEXFRAMEGEN_STATE_PAYLOAD, // write payload symbols
        OFDMFLEXFRAMEGEN_STATE_TAIL,    // write tail of last symbol
        OFDMFLEXFRAMEGEN_STATE_ZEROS    // write zeros
    } state;
    int frame_assembled;                // frame assembled flag
    int frame_complete;                 // frame completed flag
    unsigned int header_symbol_index;   //
    unsigned int payload_symbol_index;  //

    // properties
    ofdmflexframegenprops_s props;
    ofdmflexframegenprops_s header_props;
};

// create OFDM flexible framing generator object
//  _M          :   number of subcarriers, >10 typical
//  _cp_len     :   cyclic prefix length
//  _taper_len  :   taper length (OFDM symbol overlap)
//  _p          :   subcarrier allocation (null, pilot, data), [size: _M x 1]
//  _fgprops    :   frame properties (modulation scheme, etc.)
ofdmflexframegen ofdmflexframegen_create(unsigned int              _M,
                                         unsigned int              _cp_len,
                                         unsigned int              _taper_len,
                                         unsigned char *           _p,
                                         ofdmflexframegenprops_s * _fgprops)
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
    q->M         = _M;          // number of subcarriers
    q->cp_len    = _cp_len;     // cyclic prefix length
    q->taper_len = _taper_len;  // taper length

    // allocate memory for transform buffers
    q->frame_len = q->M + q->cp_len;    // frame length
    q->X         = (float complex*) malloc((q->M        )*sizeof(float complex));
    q->buf_tx    = (float complex*) malloc((q->frame_len)*sizeof(float complex));
    q->buf_index = q->frame_len;

    // allocate memory for subcarrier allocation IDs
    q->p = (unsigned char*) malloc((q->M)*sizeof(unsigned char));
    if (_p == NULL) {
        // initialize default subcarrier allocation
        ofdmframe_init_default_sctype(q->M, q->p);
    } else {
        // copy user-defined subcarrier allocation
        memmove(q->p, _p, q->M*sizeof(unsigned char));
    }

    // validate and count subcarrier allocation
    ofdmframe_validate_sctype(q->p, q->M, &q->M_null, &q->M_pilot, &q->M_data);

    // create internal OFDM frame generator object
    q->fg = ofdmframegen_create(q->M, q->cp_len, q->taper_len, q->p);

    // create header objects
    q->header = NULL;
    q->p_header = NULL;
    q->header_enc = NULL;
    q->header_mod = NULL;
    q->mod_header = NULL;
    q->header_user_len = OFDMFLEXFRAME_H_USER_DEFAULT;
    ofdmflexframegen_set_header_props(q, NULL);

    // initial memory allocation for payload
    q->payload_dec_len = 1;
    q->p_payload = packetizer_create(q->payload_dec_len,
                                     LIQUID_CRC_NONE,
                                     LIQUID_FEC_NONE,
                                     LIQUID_FEC_NONE);
    q->payload_enc_len = packetizer_get_enc_msg_len(q->p_payload);
    q->payload_enc = (unsigned char*) malloc(q->payload_enc_len*sizeof(unsigned char));

    q->payload_mod_len = 1;
    q->payload_mod = (unsigned int*) malloc(q->payload_mod_len*sizeof(unsigned int));

    // create payload modem (initially QPSK, overridden by properties)
    q->mod_payload = modem_create(LIQUID_MODEM_QPSK);

    // initialize properties
    ofdmflexframegen_setprops(q, _fgprops);

    // reset
    ofdmflexframegen_reset(q);

    // return pointer to main object
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

    // free buffers/arrays
    free(_q->payload_enc);              // encoded payload bytes
    free(_q->payload_mod);              // modulated payload symbols
    free(_q->X);                        // frequency-domain buffer
    free(_q->buf_tx);                   // transmit buffer
    free(_q->p);                        // subcarrier allocation
    free(_q->header);                   // decoded header
    free(_q->header_enc);               // encoded header
    free(_q->header_mod);               // modulated header

    // free main object memory
    free(_q);
}

void ofdmflexframegen_reset(ofdmflexframegen _q)
{
    // reset symbol counter and state
    _q->symbol_number        = 0;
    _q->state = OFDMFLEXFRAMEGEN_STATE_S0a;
    _q->frame_assembled      = 0;
    _q->frame_complete       = 0;
    _q->header_symbol_index  = 0;
    _q->payload_symbol_index = 0;
    _q->buf_index            = _q->frame_len;

    // reset internal OFDM frame generator object
    // NOTE: this is important for appropriately setting the pilot phases
    ofdmframegen_reset(_q->fg);
}

// is frame assembled?
int ofdmflexframegen_is_assembled(ofdmflexframegen _q)
{
    return _q->frame_assembled;
}

void ofdmflexframegen_print(ofdmflexframegen _q)
{
    printf("ofdmflexframegen:\n");
    printf("    num subcarriers     :   %-u\n", _q->M);
    printf("      * NULL            :   %-u\n", _q->M_null);
    printf("      * pilot           :   %-u\n", _q->M_pilot);
    printf("      * data            :   %-u\n", _q->M_data);
    printf("    cyclic prefix len   :   %-u\n", _q->cp_len);
    printf("    taper len           :   %-u\n", _q->taper_len);
    printf("    properties:\n");
    printf("      * mod scheme      :   %s\n", modulation_types[_q->props.mod_scheme].fullname);
    printf("      * fec (inner)     :   %s\n", fec_scheme_str[_q->props.fec0][1]);
    printf("      * fec (outer)     :   %s\n", fec_scheme_str[_q->props.fec1][1]);
    printf("      * CRC scheme      :   %s\n", crc_scheme_str[_q->props.check][1]);
    printf("    frame assembled     :   %s\n", _q->frame_assembled ? "yes" : "no");
    if (_q->frame_assembled) {
        printf("    payload:\n");
        printf("      * decoded bytes   :   %-u\n", _q->payload_dec_len);
        printf("      * encoded bytes   :   %-u\n", _q->payload_enc_len);
        printf("      * modulated syms  :   %-u\n", _q->payload_mod_len);
        printf("    total OFDM symbols  :   %-u\n", ofdmflexframegen_getframelen(_q));
        printf("      * S0 symbols      :   %-u @ %u\n", 2, _q->M+_q->cp_len);
        printf("      * S1 symbols      :   %-u @ %u\n", 1, _q->M+_q->cp_len);
        printf("      * header symbols  :   %-u @ %u\n", _q->num_symbols_header,  _q->M+_q->cp_len);
        printf("      * payload symbols :   %-u @ %u\n", _q->num_symbols_payload, _q->M+_q->cp_len);

        // compute asymptotic spectral efficiency
        unsigned int num_bits = 8*_q->payload_dec_len;
        unsigned int num_samples = (_q->M+_q->cp_len)*(3 + _q->num_symbols_header + _q->num_symbols_payload);
        printf("    spectral efficiency :   %-6.4f b/s/Hz\n", (float)num_bits / (float)num_samples);
    }
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
    // if properties object is NULL, initialize with defaults
    if (_props == NULL) {
        ofdmflexframegen_setprops(_q, &ofdmflexframegenprops_default);
        return;
    }

    // validate input
    if (_props->check == LIQUID_CRC_UNKNOWN || _props->check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr, "error: ofdmflexframegen_setprops(), invalid/unsupported CRC scheme\n");
        exit(1);
    } else if (_props->fec0 == LIQUID_FEC_UNKNOWN || _props->fec1 == LIQUID_FEC_UNKNOWN) {
        fprintf(stderr, "error: ofdmflexframegen_setprops(), invalid/unsupported FEC scheme\n");
        exit(1);
    } else if (_props->mod_scheme == LIQUID_MODEM_UNKNOWN ) {
        fprintf(stderr, "error: ofdmflexframegen_setprops(), invalid/unsupported modulation scheme\n");
        exit(1);
    }

    // TODO : determine if re-configuration is necessary

    // copy properties to internal structure
    memmove(&_q->props, _props, sizeof(ofdmflexframegenprops_s));

    // reconfigure internal buffers, objects, etc.
    ofdmflexframegen_reconfigure(_q);
}

void ofdmflexframegen_set_header_len(ofdmflexframegen _q,
                                     unsigned int     _len)
{
    _q->header_user_len = _len;
    _q->header_dec_len = OFDMFLEXFRAME_H_DEC + _q->header_user_len;
    _q->header = realloc(_q->header, _q->header_dec_len*sizeof(unsigned char));

    if (_q->p_header) {
        packetizer_destroy(_q->p_header);
    }
    _q->p_header = packetizer_create(_q->header_dec_len,
                                     _q->header_props.check,
                                     _q->header_props.fec0,
                                     _q->header_props.fec1);
    _q->header_enc_len = packetizer_get_enc_msg_len(_q->p_header);
    _q->header_enc = realloc(_q->header_enc, _q->header_enc_len*sizeof(unsigned char));

    unsigned int bps = modulation_types[_q->header_props.mod_scheme].bps;
    div_t bps_d = div(_q->header_enc_len*8, bps);
    _q->header_sym_len = bps_d.quot + (bps_d.rem ? 1 : 0);
    _q->header_mod = realloc(_q->header_mod, _q->header_sym_len*sizeof(unsigned char));
    // create header objects
    if (_q->mod_header) {
        modem_destroy(_q->mod_header);
    }
    _q->mod_header = modem_create(_q->header_props.mod_scheme);

    // compute number of header symbols
    div_t d = div(_q->header_sym_len, _q->M_data);
    _q->num_symbols_header = d.quot + (d.rem ? 1 : 0);
}

void ofdmflexframegen_set_header_props(ofdmflexframegen _q,
                                       ofdmflexframegenprops_s * _props)
{
    // if properties object is NULL, initialize with defaults
    if (_props == NULL) {
        _props = &ofdmflexframegenprops_header_default;
    }

    // validate input
    if (_props->check == LIQUID_CRC_UNKNOWN || _props->check >= LIQUID_CRC_NUM_SCHEMES) {
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
    memmove(&_q->header_props, _props, sizeof(ofdmflexframegenprops_s));

    // reconfigure internal buffers, objects, etc.
    ofdmflexframegen_set_header_len(_q, _q->header_user_len);
}

// get length of frame (symbols)
//  _q              :   OFDM frame generator object
unsigned int ofdmflexframegen_getframelen(ofdmflexframegen _q)
{
    // number of S0 symbols (2)
    // number of S1 symbols (1)
    // number of header symbols
    // number of payload symbols
    return  2 + // S0 symbols
            1 + // S1 symbol
            _q->num_symbols_header +
            _q->num_symbols_payload;
}

// assemble a frame from an array of data (NULL pointers will use random data)
//  _q              :   OFDM frame generator object
//  _header         :   frame header
//  _payload        :   payload data [size: _payload_len x 1]
//  _payload_len    :   payload data length
void ofdmflexframegen_assemble(ofdmflexframegen      _q,
                               const unsigned char * _header,
                               const unsigned char * _payload,
                               unsigned int          _payload_len)
{
    // reset state
    ofdmflexframegen_reset(_q);

    // check payload length and reconfigure if necessary
    if (_payload_len != _q->payload_dec_len) {
        _q->payload_dec_len = _payload_len;
        ofdmflexframegen_reconfigure(_q);
    }

    // set assembled flag
    _q->frame_assembled = 1;

    // copy user-defined header data
    if (_header == NULL)
        memset(_q->header, 0x00, _q->header_user_len*sizeof(unsigned char));
    else
        memmove(_q->header, _header, _q->header_user_len*sizeof(unsigned char));

    // encode full header
    ofdmflexframegen_encode_header(_q);

    // modulate header
    ofdmflexframegen_modulate_header(_q);

    // encode payload
    packetizer_encode(_q->p_payload, _payload, _q->payload_enc);

    // 
    // pack modem symbols
    //

    // clear payload
    memset(_q->payload_mod, 0x00, _q->payload_mod_len * sizeof(unsigned int));

    // repack 8-bit payload bytes into 'bps'-bit payload symbols
    unsigned int bps = modulation_types[_q->props.mod_scheme].bps;
    liquid_unpack_array_block(_q->payload_enc, _q->payload_enc_len,
                              bps, _q->payload_mod_len, _q->payload_mod);
}

// write samples of assembled frame
//  _q              :   OFDM frame generator object
//  _buf            :   output buffer [size: _buf_len x 1]
//  _buf_len        :   output buffer length
int ofdmflexframegen_write(ofdmflexframegen _q,
                           float complex *  _buf,
                           unsigned int     _buf_len)
{
    unsigned int i;
    for (i=0; i<_buf_len; i++) {
        if (_q->buf_index >= _q->frame_len) {
            ofdmflexframegen_gen_symbol(_q);
            _q->buf_index = 0;
        }

        // TODO: write samples appropriately
        _buf[i] = _q->buf_tx[_q->buf_index++];
    }
    return _q->frame_complete;
}


//
// internal
//

// reconfigure internal buffers, objects, etc.
void ofdmflexframegen_reconfigure(ofdmflexframegen _q)
{
    // re-create payload packetizer
    _q->p_payload = packetizer_recreate(_q->p_payload,
                                        _q->payload_dec_len,
                                        _q->props.check,
                                        _q->props.fec0,
                                        _q->props.fec1);

    // re-allocate memory for encoded message
    _q->payload_enc_len = packetizer_get_enc_msg_len(_q->p_payload);
    _q->payload_enc = (unsigned char*) realloc(_q->payload_enc,
                                               _q->payload_enc_len*sizeof(unsigned char));
#if DEBUG_OFDMFLEXFRAMEGEN
    //printf(">>>> payload : %u (%u encoded)\n", _q->props.payload_len, _q->payload_enc_len);
#endif

    // re-create modem
    // TODO : only do this if necessary
    _q->mod_payload = modem_recreate(_q->mod_payload, _q->props.mod_scheme);

    // re-allocate memory for payload modem symbols
    unsigned int bps = modulation_types[_q->props.mod_scheme].bps;
    div_t d = div(8*_q->payload_enc_len, bps);
    _q->payload_mod_len = d.quot + (d.rem ? 1 : 0);
    _q->payload_mod = (unsigned int*)realloc(_q->payload_mod,
                                              _q->payload_mod_len*sizeof(unsigned int));

    // re-compute number of payload OFDM symbols
    d = div(_q->payload_mod_len, _q->M_data);
    _q->num_symbols_payload = d.quot + (d.rem ? 1 : 0);
}

// encode header
void ofdmflexframegen_encode_header(ofdmflexframegen _q)
{
    // first 'n' bytes user data
    unsigned int n = _q->header_user_len;

    // first byte is for expansion/version validation
    _q->header[n+0] = OFDMFLEXFRAME_PROTOCOL;

    // add payload length
    _q->header[n+1] = (_q->payload_dec_len >> 8) & 0xff;
    _q->header[n+2] = (_q->payload_dec_len     ) & 0xff;

    // add modulation scheme/depth (pack into single byte)
    _q->header[n+3]  = _q->props.mod_scheme;

    // add CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [n+4]
    //  fec0    : least-significant 5 bits of [n+4]
    //  fec1    : least-significant 5 bits of [n+5]
    _q->header[n+4]  = (_q->props.check & 0x07) << 5;
    _q->header[n+4] |= (_q->props.fec0) & 0x1f;
    _q->header[n+5]  = (_q->props.fec1) & 0x1f;

    // run packet encoder
    packetizer_encode(_q->p_header, _q->header, _q->header_enc);

    // scramble header
    scramble_data(_q->header_enc, _q->header_enc_len);

#if 0
    // print header (decoded)
    unsigned int i;
    printf("header tx (dec) : ");
    for (i=0; i<OFDMFLEXFRAME_H_DEC; i++)
        printf("%.2X ", _q->header[i]);
    printf("\n");

    // print header (encoded)
    printf("header tx (enc) : ");
    for (i=0; i<OFDMFLEXFRAME_H_ENC; i++)
        printf("%.2X ", _q->header_enc[i]);
    printf("\n");
#endif
}

// modulate header
void ofdmflexframegen_modulate_header(ofdmflexframegen _q)
{
    // repack 8-bit header bytes into 'bps'-bit payload symbols
    unsigned int bps = modulation_types[_q->header_props.mod_scheme].bps;
    unsigned int num_written;
    liquid_repack_bytes(_q->header_enc, 8,   _q->header_enc_len,
                        _q->header_mod, bps, _q->header_sym_len,
                        &num_written);
}

// generate transmit samples (fill internal buffer)
void ofdmflexframegen_gen_symbol(ofdmflexframegen _q)
{
    // increment symbol counter
    _q->symbol_number++;

    switch (_q->state) {
    case OFDMFLEXFRAMEGEN_STATE_S0a:     ofdmflexframegen_gen_S0a    (_q); break;
    case OFDMFLEXFRAMEGEN_STATE_S0b:     ofdmflexframegen_gen_S0b    (_q); break;
    case OFDMFLEXFRAMEGEN_STATE_S1:      ofdmflexframegen_gen_S1     (_q); break;
    case OFDMFLEXFRAMEGEN_STATE_HEADER:  ofdmflexframegen_gen_header (_q); break;
    case OFDMFLEXFRAMEGEN_STATE_PAYLOAD: ofdmflexframegen_gen_payload(_q); break;
    case OFDMFLEXFRAMEGEN_STATE_TAIL:    ofdmflexframegen_gen_tail   (_q); break;
    case OFDMFLEXFRAMEGEN_STATE_ZEROS:   ofdmflexframegen_gen_zeros  (_q); break;
    default:
        fprintf(stderr,"error: ofdmflexframegen_writesymbol(), unknown/unsupported internal state\n");
        exit(1);
    }
}

// write first S0 symbol
void ofdmflexframegen_gen_S0a(ofdmflexframegen _q)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing S0[a] symbol\n");
#endif

    // write S0 symbol into front of buffer
    ofdmframegen_write_S0a(_q->fg, _q->buf_tx);

    // update state
    _q->state = OFDMFLEXFRAMEGEN_STATE_S0b;
}

// write second S0 symbol
void ofdmflexframegen_gen_S0b(ofdmflexframegen _q)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing S0[b] symbol\n");
#endif

    // write S0 symbol into front of buffer
    ofdmframegen_write_S0b(_q->fg, _q->buf_tx);

    // update state
    _q->state = OFDMFLEXFRAMEGEN_STATE_S1;
}

// write S1 symbol
void ofdmflexframegen_gen_S1(ofdmflexframegen _q)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing S1 symbol\n");
#endif

    // write S1 symbol into end of buffer
    ofdmframegen_write_S1(_q->fg, _q->buf_tx);

    // update state
    _q->symbol_number = 0;
    _q->state = OFDMFLEXFRAMEGEN_STATE_HEADER;
}

// write header symbol
void ofdmflexframegen_gen_header(ofdmflexframegen _q)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing header symbol\n");
#endif

    // load data onto data subcarriers
    unsigned int i;
    int sctype;
    for (i=0; i<_q->M; i++) {
        //
        sctype = _q->p[i];

        // 
        if (sctype == OFDMFRAME_SCTYPE_DATA) {
            // load...
            if (_q->header_symbol_index < _q->header_sym_len) {
                // modulate header symbol onto data subcarrier
                modem_modulate(_q->mod_header, _q->header_mod[_q->header_symbol_index++], &_q->X[i]);
                //printf("  writing symbol %3u / %3u (x = %8.5f + j%8.5f)\n", _q->header_symbol_index, OFDMFLEXFRAME_H_SYM, crealf(_q->X[i]), cimagf(_q->X[i]));
            } else {
                //printf("  random header symbol\n");
                // load random symbol
                unsigned int sym = modem_gen_rand_sym(_q->mod_header);
                modem_modulate(_q->mod_header, sym, &_q->X[i]);
            }
        } else {
            // ignore subcarrier (ofdmframegen handles nulls and pilots)
            _q->X[i] = 0.0f;
        }
    }

    // write symbol
    ofdmframegen_writesymbol(_q->fg, _q->X, _q->buf_tx);

    // check state
    if (_q->symbol_number == _q->num_symbols_header) {
        _q->symbol_number = 0;
        _q->state = OFDMFLEXFRAMEGEN_STATE_PAYLOAD;
    }
}

// write payload symbol
void ofdmflexframegen_gen_payload(ofdmflexframegen _q)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing payload symbol\n");
#endif

    // load data onto data subcarriers
    unsigned int i;
    int sctype;
    for (i=0; i<_q->M; i++) {
        //
        sctype = _q->p[i];

        // 
        if (sctype == OFDMFRAME_SCTYPE_DATA) {
            // load...
            if (_q->payload_symbol_index < _q->payload_mod_len) {
                // modulate payload symbol onto data subcarrier
                modem_modulate(_q->mod_payload, _q->payload_mod[_q->payload_symbol_index++], &_q->X[i]);
            } else {
                //printf("  random payload symbol\n");
                // load random symbol
                unsigned int sym = modem_gen_rand_sym(_q->mod_payload);
                modem_modulate(_q->mod_payload, sym, &_q->X[i]);
            }
        } else {
            // ignore subcarrier (ofdmframegen handles nulls and pilots)
            _q->X[i] = 0.0f;
        }
    }

    // write symbol
    ofdmframegen_writesymbol(_q->fg, _q->X, _q->buf_tx);

    // check to see if this is the last symbol in the payload
    if (_q->symbol_number == _q->num_symbols_payload)
        _q->state = OFDMFLEXFRAMEGEN_STATE_TAIL;
}

// generate buffer of zeros
void ofdmflexframegen_gen_tail(ofdmflexframegen _q)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing tail\n");
#endif
    // initialize buffer with zeros
    unsigned int i;
    for (i=0; i<_q->frame_len; i++)
        _q->buf_tx[i] = 0.0f;

    // write taper_len samples to buffer
    ofdmframegen_writetail(_q->fg, _q->buf_tx);

    // mark frame as complete
    _q->frame_complete = 1;
    _q->frame_assembled = 0;
    _q->state = OFDMFLEXFRAMEGEN_STATE_ZEROS;
}

// generate buffer of zeros
void ofdmflexframegen_gen_zeros(ofdmflexframegen _q)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing zeros\n");
#endif
    unsigned int i;
    for (i=0; i<_q->frame_len; i++)
        _q->buf_tx[i] = 0.0f;
}

