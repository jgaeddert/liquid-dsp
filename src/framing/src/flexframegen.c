/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// flexframegen.c
//
// flexible frame generator
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include "liquid.internal.h"

#define DEBUG_FLEXFRAMEGEN          0

// reconfigure internal properties
void flexframegen_reconfigure(flexframegen _q);

// encode header
void flexframegen_encode_header(flexframegen _q);

void flexframegen_modulate_header(flexframegen _q);

void flexframegen_modulate_payload(flexframegen _q);

//
void flexframegen_write_preamble(flexframegen _q, float complex * _buffer);
void flexframegen_write_header(  flexframegen _q, float complex * _buffer);
void flexframegen_write_payload( flexframegen _q, float complex * _buffer);
void flexframegen_write_tail(    flexframegen _q, float complex * _buffer);


// default flexframegen properties
static flexframegenprops_s flexframegenprops_default = {
    LIQUID_CRC_16,      // check
    LIQUID_FEC_NONE,    // fec0
    LIQUID_FEC_NONE,    // fec1
    LIQUID_MODEM_BPSK,  // mod_scheme
};

void flexframegenprops_init_default(flexframegenprops_s * _props)
{
    memmove(_props, &flexframegenprops_default, sizeof(flexframegenprops_s));
}

struct flexframegen_s {
    // BPSK preamble
    float preamble_pn[64];              // p/n sequence
    // post-p/n sequence symbols?

    // header (BPSK)
    modem mod_header;                   // header BPSK modulator
    packetizer p_header;                // header packetizer
    unsigned char header[FLEXFRAME_H_DEC];      // header data (uncoded)
    unsigned char header_enc[FLEXFRAME_H_ENC];  // header data (encoded)
    unsigned char header_sym[FLEXFRAME_H_SYM];  // header symbols

    // payload
    packetizer p_payload;               // payload packetizer
    unsigned int payload_dec_len;       // payload length (num un-encoded bytes)
    modem mod_payload;                  // payload modulator
    unsigned char * payload_enc;        // payload data (encoded bytes)
    unsigned char * payload_mod;        // payload symbols (modem input)
    unsigned int payload_enc_len;       // length of encoded payload
    unsigned int payload_mod_len;       // length of encoded payload

    // interpolator
    unsigned int k;                     // interp samples/symbol (fixed at 2)
    unsigned int m;                     // interp filter delay (symbols)
    float        beta;                  // excess bandwidth factor
    firinterp_crcf interp;              // interpolator object

    // counters/states
    unsigned int symbol_counter;         // output symbol number
    enum {
        STATE_PREAMBLE=0,               // write preamble p/n sequence
        STATE_HEADER,                   // write header symbols
        STATE_PAYLOAD,                  // write payload symbols
        STATE_TAIL,                     // tail symbols
    } state;
    int frame_assembled;                // frame assembled flag
    int frame_complete;                 // frame completed flag

    // properties
    flexframegenprops_s props;
};

flexframegen flexframegen_create(flexframegenprops_s * _fgprops)
{
    flexframegen q = (flexframegen) malloc(sizeof(struct flexframegen_s));

    unsigned int i;

    // generate pn sequence
    msequence ms = msequence_create(6, 0x005b, 1);
    for (i=0; i<64; i++)
        q->preamble_pn[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    // create header objects
    q->mod_header = modem_create(LIQUID_MODEM_BPSK);
    q->p_header   = packetizer_create(FLEXFRAME_H_DEC,
                                      FLEXFRAME_H_CRC,
                                      FLEXFRAME_H_FEC0,
                                      FLEXFRAME_H_FEC1);
    assert(packetizer_get_enc_msg_len(q->p_header)==FLEXFRAME_H_ENC);

    // initial memory allocation for payload
    q->payload_dec_len = 1;
    q->p_payload = packetizer_create(q->payload_dec_len,
                                     LIQUID_CRC_NONE,
                                     LIQUID_FEC_NONE,
                                     LIQUID_FEC_NONE);
    q->payload_enc_len = packetizer_get_enc_msg_len(q->p_payload);
    q->payload_enc = (unsigned char*) malloc(q->payload_enc_len*sizeof(unsigned char));

    q->payload_mod_len = 1;
    q->payload_mod = (unsigned char*) malloc(1*sizeof(unsigned char));

    // create payload modem (initially QPSK, overridden by properties)
    q->mod_payload = modem_create(LIQUID_MODEM_QPSK);
    
    // create pulse-shaping filter
    q->k    = 2;
    q->m    = 7;
    q->beta = 0.25f;
    q->interp = firinterp_crcf_create_rnyquist(LIQUID_FIRFILT_ARKAISER,q->k,q->m,q->beta,0);

    // ensure frame is not assembled and initialize properties
    q->frame_assembled = 0;
    flexframegen_setprops(q, _fgprops);

    // reset
    flexframegen_reset(q);

    // return pointer to main object
    return q;
}

void flexframegen_destroy(flexframegen _q)
{
    // destroy internal objects
    packetizer_destroy(_q->p_header);   // header packetizer
    modem_destroy(_q->mod_header);      // header modulator
    packetizer_destroy(_q->p_payload);  // payload packetizer
    modem_destroy(_q->mod_payload);     // payload modulator
    firinterp_crcf_destroy(_q->interp); // pulse-shaping filter

    // free buffers/arrays
    free(_q->payload_enc);              // encoded payload bytes
    free(_q->payload_mod);              // modulated payload symbols

    // destroy frame generator
    free(_q);
}

// print flexframegen object internals
void flexframegen_print(flexframegen _q)
{
    printf("flexframegen:\n");
    printf("    p/n sequence len    :   %u\n",       64);
    printf("    header len          :   %u\n",      FLEXFRAME_H_SYM);
    printf("    payload len, uncoded:   %u bytes\n", _q->payload_dec_len);
    printf("    payload crc         :   %s\n", crc_scheme_str[_q->props.check][1]);
    printf("    fec (inner)         :   %s\n", fec_scheme_str[_q->props.fec0][1]);
    printf("    fec (outer)         :   %s\n", fec_scheme_str[_q->props.fec1][1]);
    printf("    payload len, coded  :   %u bytes\n", _q->payload_enc_len);
    printf("    modulation scheme   :   %s\n", modulation_types[_q->props.mod_scheme].name);
    printf("    num payload symbols :   %u\n", _q->payload_mod_len);
}

// reset flexframegen object internals
void flexframegen_reset(flexframegen _q)
{
    // reset internal counters
    _q->symbol_counter  = 0;
    _q->frame_assembled = 0;
    _q->frame_complete  = 0;

    // reset state
    _q->state = STATE_PREAMBLE;
}

// is frame assembled?
int flexframegen_is_assembled(flexframegen _q)
{
    return _q->frame_assembled;
}

// get flexframegen properties
//  _q     :   frame generator object
//  _props  :   frame generator properties structure pointer
void flexframegen_getprops(flexframegen          _q,
                           flexframegenprops_s * _props)
{
    // copy properties structure to output pointer
    memmove(_props, &_q->props, sizeof(flexframegenprops_s));
}

// set flexframegen properties
//  _q      :   frame generator object
//  _props  :   frame generator properties structure pointer
void flexframegen_setprops(flexframegen          _q,
                           flexframegenprops_s * _props)
{
    // if frame is already assembled, give warning
    if (_q->frame_assembled) {
        fprintf(stderr, "warning: flexframegen_setprops(), frame is already assembled; must reset() first\n");
        return;
    }

    // if properties object is NULL, initialize with defaults
    if (_props == NULL) {
        flexframegen_setprops(_q, &flexframegenprops_default);
        return;
    }

    // validate input
    if (_props->check == LIQUID_CRC_UNKNOWN || _props->check >= LIQUID_CRC_NUM_SCHEMES) {
        fprintf(stderr, "error: flexframegen_setprops(), invalid/unsupported CRC scheme\n");
        exit(1);
    } else if (_props->fec0 == LIQUID_FEC_UNKNOWN || _props->fec1 == LIQUID_FEC_UNKNOWN) {
        fprintf(stderr, "error: flexframegen_setprops(), invalid/unsupported FEC scheme\n");
        exit(1);
    } else if (_props->mod_scheme == LIQUID_MODEM_UNKNOWN ) {
        fprintf(stderr, "error: flexframegen_setprops(), invalid/unsupported modulation scheme\n");
        exit(1);
    }

    // TODO : determine if re-configuration is necessary

    // copy properties to internal structure
    memmove(&_q->props, _props, sizeof(flexframegenprops_s));

    // reconfigure payload buffers (reallocate as necessary)
    flexframegen_reconfigure(_q);
}

// get frame length (number of samples)
unsigned int flexframegen_getframelen(flexframegen _q)
{
    if (!_q->frame_assembled) {
        fprintf(stderr,"warning: flexframegen_getframelen(), frame not assembled!\n");
        return 0;
    }
    unsigned int num_frame_symbols =
            64 +                    // preamble p/n sequence length
            FLEXFRAME_H_SYM +       // header symbols
            _q->payload_mod_len +   // number of modulation symbols
            2*_q->m;                // number of tail symbols

    return num_frame_symbols*_q->k; // k samples/symbol
}

// exectue frame generator (create the frame)
//  _q              :   frame generator object
//  _header         :   user-defined header
//  _payload        :   variable payload buffer (configured by setprops method)
//  _payload_len    :   length of payload
void flexframegen_assemble(flexframegen    _q,
                           unsigned char * _header,
                           unsigned char * _payload,
                           unsigned int    _payload_len)
{
    // check payload length and reconfigure if necessary
    if (_payload_len != _q->payload_dec_len) {
        _q->payload_dec_len = _payload_len;
        flexframegen_reconfigure(_q);
    }

    // set assembled flag
    _q->frame_assembled = 1;

    // copy user-defined header data
    memmove(_q->header, _header, FLEXFRAME_H_USER*sizeof(unsigned char));

    // encode full header
    flexframegen_encode_header(_q);

    // modulate header
    flexframegen_modulate_header(_q);

    // encode payload
    packetizer_encode(_q->p_payload, _payload, _q->payload_enc);
    scramble_data(_q->payload_enc, _q->payload_enc_len);

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

#if DEBUG_FLEXFRAMEGEN
    printf("wrote %u symbols (expected %u)\n", num_written, _q->payload_mod_len);
    flexframegen_print(_q);
#endif
}

// write symbols of assembled frame
//  _q              :   frame generator object
//  _buffer         :   output buffer [size: N+cp_len x 1]
int flexframegen_write_samples(flexframegen    _q,
                               float complex * _buffer)
{
    // check if frame is actually assembled
    if ( !_q->frame_assembled ) {
        fprintf(stderr,"warning: flexframegen_writesymbol(), frame not assembled\n");
        return 1;
    }

    switch (_q->state) {
    case STATE_PREAMBLE:
        // write preamble
        flexframegen_write_preamble(_q, _buffer);
        break;
    case STATE_HEADER:
        // write header symbols
        flexframegen_write_header(_q, _buffer);
        break;
    case STATE_PAYLOAD:
        // write payload symbols
        flexframegen_write_payload(_q, _buffer);
        break;
    case STATE_TAIL:
        // write tail symbols
        flexframegen_write_tail(_q, _buffer);
        break;
    default:
        fprintf(stderr,"error: flexframegen_writesymbol(), unknown/unsupported internal state\n");
        exit(1);
    }

    if (_q->frame_complete) {
        // reset framing object
        flexframegen_reset(_q);
        return 1;
    }

    return 0;
}

//
// internal
//

// reconfigure internal buffers, objects, etc.
void flexframegen_reconfigure(flexframegen _q)
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
#if DEBUG_FLEXFRAMEGEN
    printf(">>>> payload : %u (%u encoded)\n", _q->payload_dec_len, _q->payload_enc_len);
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
#if DEBUG_FLEXFRAMEGEN
    printf(">>>> payload mod length : %u\n", _q->payload_mod_len);
#endif
}

// encode header of flexframe
void flexframegen_encode_header(flexframegen _q)
{
    // first several bytes of header are user-defined
    unsigned int n = FLEXFRAME_H_USER;

    // add FLEXFRAME_VERSION
    _q->header[n+0] = FLEXFRAME_VERSION;

    // add payload length
    _q->header[n+1] = (_q->payload_dec_len >> 8) & 0xff;
    _q->header[n+2] = (_q->payload_dec_len     ) & 0xff;

    // add modulation scheme/depth (pack into single byte)
    _q->header[n+3]  = (unsigned int)(_q->props.mod_scheme);

    // add CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [n+4]
    //  fec0    : least-significant 5 bits of [n+4]
    //  fec1    : least-significant 5 bits of [n+5]
    _q->header[n+4]  = (_q->props.check & 0x07) << 5;
    _q->header[n+4] |= (_q->props.fec0) & 0x1f;
    _q->header[n+5]  = (_q->props.fec1) & 0x1f;

    // run packet encoder
    packetizer_encode(_q->p_header, _q->header, _q->header_enc);

    // scramble encoded header
    scramble_data(_q->header_enc, FLEXFRAME_H_ENC);

#if DEBUG_FLEXFRAMEGEN
    // print header (decoded)
    unsigned int i;
    printf("header tx (dec) : ");
    for (i=0; i<19; i++)
        printf("%.2X ", _q->header[i]);
    printf("\n");

    // print header (encoded)
    printf("header tx (enc) : ");
    for (i=0; i<32; i++)
        printf("%.2X ", _q->header_enc[i]);
    printf("\n");
#endif
}

// modulate header into BPSK symbols
void flexframegen_modulate_header(flexframegen _q)
{
    unsigned int i;

    // unpack header symbols
    for (i=0; i<FLEXFRAME_H_ENC; i++) {
        _q->header_sym[8*i+0] = (_q->header_enc[i] >> 7) & 0x01;
        _q->header_sym[8*i+1] = (_q->header_enc[i] >> 6) & 0x01;
        _q->header_sym[8*i+2] = (_q->header_enc[i] >> 5) & 0x01;
        _q->header_sym[8*i+3] = (_q->header_enc[i] >> 4) & 0x01;
        _q->header_sym[8*i+4] = (_q->header_enc[i] >> 3) & 0x01;
        _q->header_sym[8*i+5] = (_q->header_enc[i] >> 2) & 0x01;
        _q->header_sym[8*i+6] = (_q->header_enc[i] >> 1) & 0x01;
        _q->header_sym[8*i+7] = (_q->header_enc[i]     ) & 0x01;
    }
}

// modulate payload
void flexframegen_modulate_payload(flexframegen _q)
{
}

// write preamble
void flexframegen_write_preamble(flexframegen    _q,
                                 float complex * _buffer)
{
#if DEBUG_FLEXFRAMEGEN
    //printf("writing preamble symbol %u\n", _q->symbol_counter);
#endif

    // interpolate symbol
    float complex s = _q->preamble_pn[_q->symbol_counter];
    firinterp_crcf_execute(_q->interp, s, _buffer);

    // increment symbol counter
    _q->symbol_counter++;

    // check state
    if (_q->symbol_counter == 64) {
        _q->symbol_counter = 0;
        _q->state = STATE_HEADER;
    }
}

// write header
void flexframegen_write_header(flexframegen    _q,
                               float complex * _buffer)
{
#if DEBUG_FLEXFRAMEGEN
    //printf("writing header symbol %u\n", _q->symbol_counter);
#endif

    float complex s;
    modem_modulate(_q->mod_header, _q->header_sym[_q->symbol_counter], &s);

    // interpolate symbol
    firinterp_crcf_execute(_q->interp, s, _buffer);

    // increment symbol counter
    _q->symbol_counter++;

    // check state
    if (_q->symbol_counter == FLEXFRAME_H_SYM) {
        _q->symbol_counter = 0;
        _q->state = STATE_PAYLOAD;
    }
}

// write payload
void flexframegen_write_payload(flexframegen    _q,
                                float complex * _buffer)
{
#if DEBUG_FLEXFRAMEGEN
    //printf("writing payload symbol %u\n", _q->symbol_counter);
#endif

    float complex s;
    modem_modulate(_q->mod_payload, _q->payload_mod[_q->symbol_counter], &s);

    // interpolate symbol
    firinterp_crcf_execute(_q->interp, s, _buffer);

    // increment symbol counter
    _q->symbol_counter++;

    // check state
    if (_q->symbol_counter == _q->payload_mod_len) {
        _q->symbol_counter = 0;
        _q->state = STATE_TAIL;
    }
}

// write tail
void flexframegen_write_tail(flexframegen    _q,
                             float complex * _buffer)
{
#if DEBUG_FLEXFRAMEGEN
    //printf("writing tail symbol %u\n", _q->symbol_counter);
#endif

    // interpolate symbol
    firinterp_crcf_execute(_q->interp, 0.0f, _buffer);

    // increment symbol counter
    _q->symbol_counter++;

    // check state
    if (_q->symbol_counter == 2*_q->m)
        _q->frame_complete = 1;
}

