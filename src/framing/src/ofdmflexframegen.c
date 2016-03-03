/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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

#define DEBUG_OFDMFLEXFRAMEGEN            0

// reconfigure internal buffers, objects, etc.
void ofdmflexframegen_reconfigure(ofdmflexframegen _q);

// encode header
void ofdmflexframegen_encode_header(ofdmflexframegen _q);

// modulate header
void ofdmflexframegen_modulate_header(ofdmflexframegen _q);

// write first S0 symbol
void ofdmflexframegen_write_S0a(ofdmflexframegen _q,
                                float complex * _buffer);

// write second S0 symbol
void ofdmflexframegen_write_S0b(ofdmflexframegen _q,
                                float complex * _buffer);

// write S1 symbol
void ofdmflexframegen_write_S1(ofdmflexframegen _q,
                               float complex * _buffer);

// write header symbol
void ofdmflexframegen_write_header(ofdmflexframegen _q,
                                   float complex * _buffer);

// write payload symbol
void ofdmflexframegen_write_payload(ofdmflexframegen _q,
                                    float complex * _buffer);

// default ofdmflexframegen properties
static ofdmflexframegenprops_s ofdmflexframegenprops_default = {
    LIQUID_CRC_32,      // check
    LIQUID_FEC_NONE,    // fec0
    LIQUID_FEC_NONE,    // fec1
    LIQUID_MODEM_QPSK,  // mod_scheme
    //64                // block_size
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

    // buffers
    float complex * X;      // frequency-domain buffer

    // internal low-level objects
    ofdmframegen fg;        // frame generator object

    // options/derived lengths
    unsigned int num_symbols_header;    // number of header OFDM symbols
    unsigned int num_symbols_payload;   // number of payload OFDM symbols

    // header
    modem mod_header;                   // header modulator
    packetizer p_header;                // header packetizer
    unsigned char header[OFDMFLEXFRAME_H_DEC];      // header data (uncoded)
    unsigned char header_enc[OFDMFLEXFRAME_H_ENC];  // header data (encoded)
    unsigned char header_mod[OFDMFLEXFRAME_H_SYM];  // header symbols

    // payload
    packetizer p_payload;               // payload packetizer
    unsigned int payload_dec_len;       // payload length (num un-encoded bytes)
    modem mod_payload;                  // payload modulator
    unsigned char * payload_enc;        // payload data (encoded bytes)
    unsigned char * payload_mod;        // payload data (modulated symbols)
    unsigned int payload_enc_len;       // length of encoded payload
    unsigned int payload_mod_len;       // number of modulated symbols in payload

    // counters/states
    unsigned int symbol_number;         // output symbol number
    enum {
        OFDMFLEXFRAMEGEN_STATE_S0a=0,   // write S0 symbol (first)
        OFDMFLEXFRAMEGEN_STATE_S0b,     // write S0 symbol (second)
        OFDMFLEXFRAMEGEN_STATE_S1,      // write S1 symbol
        OFDMFLEXFRAMEGEN_STATE_HEADER,  // write header symbols
        OFDMFLEXFRAMEGEN_STATE_PAYLOAD  // write payload symbols
    } state;
    int frame_assembled;                // frame assembled flag
    int frame_complete;                 // frame completed flag
    unsigned int header_symbol_index;   //
    unsigned int payload_symbol_index;  //

    // properties
    ofdmflexframegenprops_s props;
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
    q->X = (float complex*) malloc((q->M)*sizeof(float complex));

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
    q->mod_header = modem_create(OFDMFLEXFRAME_H_MOD);
    q->p_header   = packetizer_create(OFDMFLEXFRAME_H_DEC,
                                      OFDMFLEXFRAME_H_CRC,
                                      OFDMFLEXFRAME_H_FEC,
                                      LIQUID_FEC_NONE);
    assert(packetizer_get_enc_msg_len(q->p_header)==OFDMFLEXFRAME_H_ENC);

    // compute number of header symbols
    div_t d = div(OFDMFLEXFRAME_H_SYM, q->M_data);
    q->num_symbols_header = d.quot + (d.rem ? 1 : 0);

    // initial memory allocation for payload
    q->payload_dec_len = 1;
    q->p_payload = packetizer_create(q->payload_dec_len,
                                     LIQUID_CRC_NONE,
                                     LIQUID_FEC_NONE,
                                     LIQUID_FEC_NONE);
    q->payload_enc_len = packetizer_get_enc_msg_len(q->p_payload);
    q->payload_enc = (unsigned char*) malloc(q->payload_enc_len*sizeof(unsigned char));

    q->payload_mod_len = 1;
    q->payload_mod = (unsigned char*) malloc(q->payload_mod_len*sizeof(unsigned char));

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
    free(_q->p);                        // subcarrier allocation

    // free main object memory
    free(_q);
}

void ofdmflexframegen_reset(ofdmflexframegen _q)
{
    // reset symbol counter and state
    _q->symbol_number = 0;
    _q->state = OFDMFLEXFRAMEGEN_STATE_S0a;
    _q->frame_assembled = 0;
    _q->frame_complete = 0;
    _q->header_symbol_index = 0;
    _q->payload_symbol_index = 0;

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

// assemble a frame from an array of data
//  _q              :   OFDM frame generator object
//  _header         :   frame header
//  _payload        :   payload data [size: _payload_len x 1]
//  _payload_len    :   payload data length
void ofdmflexframegen_assemble(ofdmflexframegen _q,
                               const unsigned char *  _header,
                               const unsigned char *  _payload,
                               unsigned int     _payload_len)
{
    // check payload length and reconfigure if necessary
    if (_payload_len != _q->payload_dec_len) {
        _q->payload_dec_len = _payload_len;
        ofdmflexframegen_reconfigure(_q);
    }

    // set assembled flag
    _q->frame_assembled = 1;

    // copy user-defined header data
    memmove(_q->header, _header, OFDMFLEXFRAME_H_USER*sizeof(unsigned char));

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
    memset(_q->payload_mod, 0x00, _q->payload_mod_len);

    // repack 8-bit payload bytes into 'bps'-bit payload symbols
    unsigned int bps = modulation_types[_q->props.mod_scheme].bps;
    unsigned int num_written;
    liquid_repack_bytes(_q->payload_enc,  8,  _q->payload_enc_len,
                        _q->payload_mod, bps, _q->payload_mod_len,
                        &num_written);
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("wrote %u symbols (expected %u)\n", num_written, _q->payload_mod_len);
#endif
}

// write symbols of assembled frame
//  _q              :   OFDM frame generator object
//  _buffer         :   output buffer [size: N+cp_len x 1]
int ofdmflexframegen_writesymbol(ofdmflexframegen       _q,
                                 liquid_float_complex * _buffer)
{
    // check if frame is actually assembled
    if ( !_q->frame_assembled ) {
        fprintf(stderr,"warning: ofdmflexframegen_writesymbol(), frame not assembled\n");
        return 1;
    }

    // increment symbol counter
    _q->symbol_number++;
    //printf("writesymbol(): %u\n", _q->symbol_number);

    switch (_q->state) {
    case OFDMFLEXFRAMEGEN_STATE_S0a:
        // write S0 symbol (first)
        ofdmflexframegen_write_S0a(_q, _buffer);
        break;

    case OFDMFLEXFRAMEGEN_STATE_S0b:
        // write S0 symbol (second)
        ofdmflexframegen_write_S0b(_q, _buffer);
        break;

    case OFDMFLEXFRAMEGEN_STATE_S1:
        // write S1 symbols
        ofdmflexframegen_write_S1(_q, _buffer);
        break;

    case OFDMFLEXFRAMEGEN_STATE_HEADER:
        // write header symbols
        ofdmflexframegen_write_header(_q, _buffer);
        break;

    case OFDMFLEXFRAMEGEN_STATE_PAYLOAD:
        // write payload symbols
        ofdmflexframegen_write_payload(_q, _buffer);
        break;

    default:
        fprintf(stderr,"error: ofdmflexframegen_writesymbol(), unknown/unsupported internal state\n");
        exit(1);
    }

    if (_q->frame_complete) {
        // reset framing object
#if DEBUG_OFDMFLEXFRAMEGEN
        printf(" ...resetting...\n");
#endif
        ofdmflexframegen_reset(_q);
        return 1;
    }

    return 0;
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
    printf(">>>> payload : %u (%u encoded)\n", _q->props.payload_len, _q->payload_enc_len);
#endif

    // re-create modem
    // TODO : only do this if necessary
    _q->mod_payload = modem_recreate(_q->mod_payload, _q->props.mod_scheme);

    // re-allocate memory for payload modem symbols
    unsigned int bps = modulation_types[_q->props.mod_scheme].bps;
    div_t d = div(8*_q->payload_enc_len, bps);
    _q->payload_mod_len = d.quot + (d.rem ? 1 : 0);
    _q->payload_mod = (unsigned char*)realloc(_q->payload_mod,
                                              _q->payload_mod_len*sizeof(unsigned char));

    // re-compute number of payload OFDM symbols
    d = div(_q->payload_mod_len, _q->M_data);
    _q->num_symbols_payload = d.quot + (d.rem ? 1 : 0);
}

// encode header
void ofdmflexframegen_encode_header(ofdmflexframegen _q)
{
    // first 'n' bytes user data
    unsigned int n = OFDMFLEXFRAME_H_USER;

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
    scramble_data(_q->header_enc, OFDMFLEXFRAME_H_ENC);

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
    unsigned int bps = modulation_types[OFDMFLEXFRAME_H_MOD].bps;
    unsigned int num_written;
    liquid_repack_bytes(_q->header_enc, 8,   OFDMFLEXFRAME_H_ENC,
                        _q->header_mod, bps, OFDMFLEXFRAME_H_SYM,
                        &num_written);
}

// write first S0 symbol
void ofdmflexframegen_write_S0a(ofdmflexframegen _q,
                                float complex * _buffer)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing S0[a] symbol\n");
#endif

    // write S0 symbol into front of buffer
    ofdmframegen_write_S0a(_q->fg, _buffer);

    // update state
    _q->state = OFDMFLEXFRAMEGEN_STATE_S0b;
}

// write second S0 symbol
void ofdmflexframegen_write_S0b(ofdmflexframegen _q,
                                float complex * _buffer)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing S0[b] symbol\n");
#endif

    // write S0 symbol into front of buffer
    ofdmframegen_write_S0b(_q->fg, _buffer);

    // update state
    _q->state = OFDMFLEXFRAMEGEN_STATE_S1;
}

// write S1 symbol
void ofdmflexframegen_write_S1(ofdmflexframegen _q,
                               float complex * _buffer)
{
#if DEBUG_OFDMFLEXFRAMEGEN
    printf("writing S1 symbol\n");
#endif

    // write S1 symbol into end of buffer
    ofdmframegen_write_S1(_q->fg, _buffer);

    // update state
    _q->symbol_number = 0;
    _q->state = OFDMFLEXFRAMEGEN_STATE_HEADER;
}

// write header symbol
void ofdmflexframegen_write_header(ofdmflexframegen _q,
                                   float complex * _buffer)
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
            if (_q->header_symbol_index < OFDMFLEXFRAME_H_SYM) {
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
    ofdmframegen_writesymbol(_q->fg, _q->X, _buffer);

    // check state
    if (_q->symbol_number == _q->num_symbols_header) {
        _q->symbol_number = 0;
        _q->state = OFDMFLEXFRAMEGEN_STATE_PAYLOAD;
    }
}

// write payload symbol
void ofdmflexframegen_write_payload(ofdmflexframegen _q,
                                    float complex * _buffer)
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
    ofdmframegen_writesymbol(_q->fg, _q->X, _buffer);

    // check to see if this is the last symbol in the payload
    if (_q->symbol_number == _q->num_symbols_payload)
        _q->frame_complete = 1;
}

