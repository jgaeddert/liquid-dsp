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

#define DEBUG_FLEXFRAMEGEN 1

// reconfigure internal properties
void          flexframegen_reconfigure      (flexframegen _q);
float complex flexframegen_generate_symbol  (flexframegen _q);
float complex flexframegen_generate_preamble(flexframegen _q);
float complex flexframegen_generate_header  (flexframegen _q);
float complex flexframegen_generate_payload (flexframegen _q);
float complex flexframegen_generate_tail    (flexframegen _q);

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
    // interpolator
    unsigned int    k;                  // interp samples/symbol (fixed at 2)
    unsigned int    m;                  // interp filter delay (symbols)
    float           beta;               // excess bandwidth factor
    firinterp_crcf  interp;             // interpolator object
    float complex   buf_interp[2];      // output interpolator buffer [size: k x 1]

    flexframegenprops_s props;          // payload properties

    // preamble
    float complex * preamble_pn;        // p/n sequence

    // header
    unsigned char * header;             // header data
    qpacketmodem    header_encoder;     // header encoder/modulator
    unsigned int    header_mod_len;     // header length (encoded/modulated)
    float complex * header_mod;         // header symbols (encoded/modulated)
    qpilotgen       header_pilotgen;    // header pilot symbol generator
    unsigned int    header_sym_len;     // header length (pilots added)
    float complex * header_sym;         // header symbols (pilots added)

    // payload
    unsigned int    payload_dec_len;    // length of decoded
    qpacketmodem    payload_encoder;    // packet encoder/modulator
    unsigned int    payload_sym_len;    // length of encoded/modulated payload
    float complex * payload_sym;        // encoded payload symbols

    // counters/states
    unsigned int    symbol_counter;     // output symbol number
    unsigned int    sample_counter;     // output sample number
    int             frame_assembled;    // frame assembled flag
    int             frame_complete;     // frame completed flag
    enum {
                    STATE_PREAMBLE=0,   // write preamble p/n sequence
                    STATE_HEADER,       // write header symbols
                    STATE_PAYLOAD,      // write payload symbols
                    STATE_TAIL,         // tail symbols
    }               state;              // write state
};

flexframegen flexframegen_create(flexframegenprops_s * _fgprops)
{
    flexframegen q = (flexframegen) malloc(sizeof(struct flexframegen_s));
    unsigned int i;

    // create pulse-shaping filter
    q->k      = 2;
    q->m      = 7;
    q->beta   = 0.25f;
    q->interp = firinterp_crcf_create_prototype(LIQUID_FIRFILT_ARKAISER,q->k,q->m,q->beta,0);

    // generate pn sequence
    q->preamble_pn = (float complex *) malloc(64*sizeof(float complex));
    msequence ms = msequence_create(7, 0x0089, 1);
    for (i=0; i<64; i++) {
        q->preamble_pn[i] = (msequence_advance(ms) ? M_SQRT1_2 : -M_SQRT1_2) +
                            (msequence_advance(ms) ? M_SQRT1_2 : -M_SQRT1_2) * _Complex_I;
    }
    msequence_destroy(ms);

    // create header encoder/modulator
    q->header     = (unsigned char *) malloc(FLEXFRAME_H_DEC*sizeof(unsigned char));
    q->header_encoder = qpacketmodem_create();
    qpacketmodem_configure(q->header_encoder,
                           FLEXFRAME_H_DEC,
                           FLEXFRAME_H_CRC,
                           FLEXFRAME_H_FEC0,
                           FLEXFRAME_H_FEC1,
                           LIQUID_MODEM_QPSK);
    q->header_mod_len = qpacketmodem_get_frame_len(q->header_encoder);
    q->header_mod     = (float complex *) malloc(q->header_mod_len*sizeof(float complex));

    // create header pilot sequence generator
    q->header_pilotgen = qpilotgen_create(q->header_mod_len, 16);
    q->header_sym_len  = qpilotgen_get_frame_len(q->header_pilotgen);
    q->header_sym      = (float complex *) malloc(q->header_sym_len*sizeof(float complex));
    //printf("header: %u bytes > %u mod > %u sym\n", 64, q->header_mod_len, q->header_sym_len);

    // payload encoder/modulator (initialize with default parameters to be reconfigured later)
    q->payload_encoder = qpacketmodem_create();
    q->payload_dec_len = 64;
    q->payload_sym_len = qpacketmodem_get_frame_len(q->payload_encoder);
    q->payload_sym     = (float complex *) malloc( q->payload_sym_len*sizeof(float complex));

    // reset object
    flexframegen_reset(q);

    // set payload properties
    flexframegen_setprops(q, _fgprops);

    // return pointer to main object
    return q;
}

void flexframegen_destroy(flexframegen _q)
{
    // destroy internal objects
    firinterp_crcf_destroy(_q->interp);
    qpacketmodem_destroy  (_q->header_encoder);
    qpilotgen_destroy     (_q->header_pilotgen);
    qpacketmodem_destroy  (_q->payload_encoder);

    // free buffers/arrays
    free(_q->preamble_pn);  // preamble symbols
    free(_q->header);       // header bytes
    free(_q->header_mod);   // encoded/modulated header symbols 
    free(_q->payload_sym);  // encoded/modulated payload symbols

    // destroy frame generator
    free(_q);
}

// print flexframegen object internals
void flexframegen_print(flexframegen _q)
{
    unsigned int num_frame_symbols =
            64 +                    // preamble p/n sequence length
            _q->header_sym_len +    // header symbols
            _q->payload_sym_len +   // number of modulation symbols
            2*_q->m;                // number of tail symbols
    unsigned int num_frame_bits = 8*_q->payload_dec_len;
    float eta = (float)num_frame_bits / (float)num_frame_symbols;

    printf("flexframegen:\n");
    printf("  head          : %u symbols\n", _q->m);
    printf("  preamble      : %u\n", 64);
    printf("  header        : %u symbols (%u bytes)\n", _q->header_sym_len, FLEXFRAME_H_DEC);
    printf("  payload       : %u symbols (%u bytes)\n", _q->payload_sym_len, _q->payload_dec_len);
    printf("    payload crc : %s\n", crc_scheme_str[_q->props.check][1]);
    printf("    fec (inner) : %s\n", fec_scheme_str[_q->props.fec0][1]);
    printf("    fec (outer) : %s\n", fec_scheme_str[_q->props.fec1][1]);
    printf("    mod scheme  : %s\n", modulation_types[_q->props.mod_scheme].name);
    printf("  tail          : %u symbols\n", _q->m);
    printf("  total         : %u symbols\n", num_frame_symbols);
    printf("  efficiency    : %.2f bits/second/Hz\n", eta);
}

// reset flexframegen object internals
void flexframegen_reset(flexframegen _q)
{
    // reset internal counters and state
    _q->symbol_counter  = 0;
    _q->sample_counter  = 0;
    _q->frame_assembled = 0;
    _q->frame_complete  = 0;
    _q->state           = STATE_PREAMBLE;
}

// is frame assembled?
int flexframegen_is_assembled(flexframegen _q)
{
    return _q->frame_assembled;
}

// get flexframegen properties
//  _q      :   frame generator object
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
int flexframegen_setprops(flexframegen          _q,
                          flexframegenprops_s * _props)
{
    // if frame is already assembled, give warning
    if (_q->frame_assembled) {
        fprintf(stderr, "warning: flexframegen_setprops(), frame is already assembled; must reset() first\n");
        return -1;
    }

    // if properties object is NULL, initialize with defaults
    if (_props == NULL) {
        flexframegen_setprops(_q, &flexframegenprops_default);
        return 0;
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

    return 0;
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
            _q->header_sym_len +    // header symbols
            _q->payload_sym_len +   // number of modulation symbols
            2*_q->m;                // number of tail symbols

    return num_frame_symbols*_q->k; // k samples/symbol
}

// exectue frame generator (create the frame)
//  _q              :   frame generator object
//  _header         :   user-defined header
//  _payload        :   variable payload buffer (configured by setprops method)
//  _payload_dec_len:   length of payload
void flexframegen_assemble(flexframegen          _q,
                           const unsigned char * _header,
                           const unsigned char * _payload,
                           unsigned int          _payload_dec_len)
{
    // reset object
    flexframegen_reset(_q);

    // set decoded payload length
    _q->payload_dec_len = _payload_dec_len;

    // copy user-defined header to internal
    memmove(_q->header, _header, FLEXFRAME_H_USER*sizeof(unsigned char));

    // first several bytes of header are user-defined
    unsigned int n = FLEXFRAME_H_USER;

    // add FLEXFRAME_PROTOCOL
    _q->header[n+0] = FLEXFRAME_PROTOCOL;

    // add payload length
    _q->header[n+1] = (_q->payload_dec_len >> 8) & 0xff;
    _q->header[n+2] = (_q->payload_dec_len     ) & 0xff;

    // add modulation scheme/depth (pack into single byte)
    _q->header[n+3]  = (unsigned int)(_q->props.mod_scheme);

    // add CRC, forward error-correction schemes
    //  CRC     : most-significant  3 bits of [n+4]
    //  fec0    : least-significant 5 bits of [n+4]
    //  fec1    : least-significant 5 bits of [n+5]
    _q->header[n+4]  = (_q->props.check & 0x07) << 5;
    _q->header[n+4] |= (_q->props.fec0) & 0x1f;
    _q->header[n+5]  = (_q->props.fec1) & 0x1f;

    // encode/modulate header
    qpacketmodem_encode(_q->header_encoder, _q->header, _q->header_mod);

    // add pilots
    qpilotgen_execute(_q->header_pilotgen, _q->header_mod, _q->header_sym);

    // reconfigure payload
    flexframegen_reconfigure(_q);

    // encode/modulate payload
    qpacketmodem_encode(_q->payload_encoder, _payload, _q->payload_sym);

    // set assembled flag
    _q->frame_assembled = 1;
}

// write samples of assembled frame, two samples at a time, returning
// '1' when frame is complete, '0' otherwise. Zeros will be written
// to the buffer if the frame is not assembled
//  _q          :   frame generator object
//  _buffer     :   output buffer [size: _buffer_len x 1]
//  _buffer_len :   output buffer length
int flexframegen_write_samples(flexframegen    _q,
                               float complex * _buffer,
                               unsigned int    _buffer_len)
{
    unsigned int i;
    for (i=0; i<_buffer_len; i++) {
        // determine if new sample needs to be written
        if (_q->sample_counter == 0) {
            // generate new symbol
            float complex sym = flexframegen_generate_symbol(_q);

            // interpolate result
            firinterp_crcf_execute(_q->interp, sym, _q->buf_interp);
        }
        
        // write output sample from interpolator buffer
        _buffer[i] = _q->buf_interp[_q->sample_counter];
            
        // adjust sample counter
        _q->sample_counter = (_q->sample_counter + 1) % _q->k;
    }

    return _q->frame_complete;
}

//
// internal
//

// reconfigure internal buffers, objects, etc.
void flexframegen_reconfigure(flexframegen _q)
{
    // configure payload encoder/modulator
    qpacketmodem_configure(_q->payload_encoder,
                           _q->payload_dec_len,
                           _q->props.check,
                           _q->props.fec0,
                           _q->props.fec1,
                           _q->props.mod_scheme);

    // re-allocate memory for encoded message
    _q->payload_sym_len = qpacketmodem_get_frame_len(_q->payload_encoder);
    _q->payload_sym = (float complex*) realloc(_q->payload_sym,
                                               _q->payload_sym_len*sizeof(float complex));

    // ensure payload was reallocated appropriately
    if (_q->payload_sym == NULL) {
        fprintf(stderr,"error: flexframegen_reconfigure(), could not re-allocate payload array\n");
        exit(1);
    }
}

// fill interpolator buffer
float complex flexframegen_generate_symbol(flexframegen _q)
{
    // write zeros to buffer if frame is not assembled
    if (!_q->frame_assembled)
        return 0.0f;

    switch (_q->state) {
    case STATE_PREAMBLE: return flexframegen_generate_preamble(_q); break;
    case STATE_HEADER:   return flexframegen_generate_header  (_q); break;
    case STATE_PAYLOAD:  return flexframegen_generate_payload (_q); break;
    case STATE_TAIL:     return flexframegen_generate_tail    (_q); break;
    default:
        fprintf(stderr,"error: flexframegen_generate_symbol(), unknown/unsupported internal state\n");
        exit(1);
    }

    return 0.0f;
}

// generate preamble
float complex flexframegen_generate_preamble(flexframegen _q)
{
    float complex symbol = _q->preamble_pn[_q->symbol_counter++];

    // check state
    if (_q->symbol_counter == 64) {
        _q->symbol_counter = 0;
        _q->state = STATE_HEADER;
    }
    return symbol;
}

// generate header
float complex flexframegen_generate_header(flexframegen _q)
{
    float complex symbol = _q->header_sym[_q->symbol_counter++];

    // check state
    if (_q->symbol_counter == _q->header_sym_len) {
        _q->symbol_counter = 0;
        _q->state = STATE_PAYLOAD;
    }
    return symbol;
}

// generate payload
float complex flexframegen_generate_payload(flexframegen _q)
{
    float complex symbol = _q->payload_sym[_q->symbol_counter++];

    // check state
    if (_q->symbol_counter == _q->payload_sym_len) {
        _q->symbol_counter = 0;
        _q->state = STATE_TAIL;
    }
    return symbol;
}

// generate tail
float complex flexframegen_generate_tail(flexframegen _q)
{
    // increment symbol counter
    _q->symbol_counter++;

    // check state
    if (_q->symbol_counter == 2*_q->m) {
        _q->symbol_counter  = 0;
        _q->frame_complete  = 1;
        _q->frame_assembled = 0;
    }

    return 0.0f;
}

