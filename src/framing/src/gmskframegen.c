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
// gmskframegen.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include "liquid.internal.h"

#define DEBUG_GMSKFRAMEGEN    0

// gmskframe object structure
struct gmskframegen_s {
    gmskmod mod;                // GMSK modulator
    unsigned int k;             // filter samples/symbol
    unsigned int m;             // filter semi-length (symbols)
    float BT;                   // filter bandwidth-time product

    // framing lengths (symbols)
    unsigned int rampup_len;    //
    unsigned int phasing_len;   //
    unsigned int preamble_len;  //
    unsigned int header_len;    // length of header (encoded)
    unsigned int payload_len;   //
    unsigned int rampdn_len;    //
    unsigned int frame_len;     // total number of symbols in the frame

    // preamble
    //unsigned int genpoly_header;// generator polynomial
    msequence ms_header;        // header sequence

    // header
    unsigned char * header_dec; // uncoded header [GMSKFRAME_H_DEC]
    unsigned char * header_enc; // encoded header [GMSKFRAME_H_ENC]
    packetizer p_header;        // header packetizer

    // payload
    packetizer p_payload;       // payload packetizer
    crc_scheme check;           // CRC
    fec_scheme fec0;            // inner forward error correction
    fec_scheme fec1;            // outer forward error correction
    unsigned int dec_msg_len;   // 
    unsigned int enc_msg_len;   // 
    unsigned char * payload_enc;// encoded payload

    // framing state
    enum {
        GMSKFRAMEGEN_STATE_RAMPUP,      // ramp up
        GMSKFRAMEGEN_STATE_PHASING,     // phasing
        GMSKFRAMEGEN_STATE_PREAMBLE,    // preamble
        GMSKFRAMEGEN_STATE_HEADER,      // header
        GMSKFRAMEGEN_STATE_PAYLOAD,     // payload (frame)
        GMSKFRAMEGEN_STATE_RAMPDN,      // ramp down
    } state;
    int frame_complete;         //
    unsigned int symbol_counter;//
};

// create gmskframegen object
gmskframegen gmskframegen_create(unsigned int _k,
                                 unsigned int _m,
                                 float _BT)
{
    // TODO : validate input
    gmskframegen q = (gmskframegen) malloc(sizeof(struct gmskframegen_s));
    q->k  = _k;
    q->m  = _m;
    q->BT = _BT;

    // internal/derived values
    q->rampup_len   =  8;   // number of ramp/up symbols
    q->phasing_len  = 40;   // number of phasing symbols
    q->preamble_len = 64;   // number of preamble symbols
    q->payload_len  =  0;   // number of payload symbols
    q->rampdn_len   =  8;   // number of ramp\dn symbols

    // create modulator
    q->mod = gmskmod_create(q->k, q->m, q->BT);

    // preamble objects/arrays
    //q->genpoly_header = 0x00;
    q->ms_header = msequence_create_default(6);

    // header objects/arrays
    q->header_dec = (unsigned char*)malloc(GMSKFRAME_H_DEC*sizeof(unsigned char));
    q->header_enc = (unsigned char*)malloc(GMSKFRAME_H_ENC*sizeof(unsigned char));
    q->header_len = GMSKFRAME_H_ENC * 8;
    q->p_header   = packetizer_create(GMSKFRAME_H_DEC,
                                      GMSKFRAME_H_CRC,
                                      GMSKFRAME_H_FEC,
                                      LIQUID_FEC_NONE);

    // payload objects/arrays
    q->dec_msg_len = 0;
    q->check = LIQUID_CRC_32;
    q->fec0  = LIQUID_FEC_NONE;
    q->fec1  = LIQUID_FEC_NONE;

    q->p_payload = packetizer_create(q->dec_msg_len,
                                     q->check,
                                     q->fec0,
                                     q->fec1);
    q->enc_msg_len = packetizer_get_enc_msg_len(q->p_payload);
    q->payload_len = 8*q->enc_msg_len;

    // allocate memory for encoded packet
    q->payload_enc = (unsigned char*) malloc(q->enc_msg_len*sizeof(unsigned char));

    // compute frame length (symbols)
    q->frame_len = q->rampup_len +
                   q->phasing_len +
                   q->preamble_len +
                   q->header_len +
                   q->payload_len +
                   q->rampdn_len;

    // reset framing object
    gmskframegen_reset(q);

    // return object
    return q;
}

// destroy gmskframegen object
void gmskframegen_destroy(gmskframegen _q)
{
    // destroy gmsk modulator
    gmskmod_destroy(_q->mod);

    // destroy/free preamble objects/arrays
    msequence_destroy(_q->ms_header);

    // destroy/free header objects/arrays
    free(_q->header_dec);
    free(_q->header_enc);
    packetizer_destroy(_q->p_header);

    // destroy/free payload objects/arrays
    free(_q->payload_enc);
    packetizer_destroy(_q->p_payload);

    // free main object memory
    free(_q);
}

// reset frame generator object
void gmskframegen_reset(gmskframegen _q)
{
    // reset GMSK modulator
    gmskmod_reset(_q->mod);

    // reset states
    _q->state = GMSKFRAMEGEN_STATE_RAMPUP;
    msequence_reset(_q->ms_header);
    _q->frame_complete = 0;
    _q->symbol_counter = 0;
}

// print gmskframegen object internals
void gmskframegen_print(gmskframegen _q)
{
    // compute spectral efficiency
    float eta = (float)(8*_q->dec_msg_len) / (float)(_q->frame_len);

    // plot
    printf("gmskframegen:\n");
    printf("  physical properties\n");
    printf("    samples/symbol  :   %u\n", _q->k);
    printf("    filter delay    :   %u symbols\n", _q->m);
    printf("    bandwidth-time  :   %-8.3f\n", _q->BT);
    printf("  framing properties\n");
    printf("    ramp/up         :   %-4u symbols\n", _q->rampup_len);
    printf("    phasing         :   %-4u symbols\n", _q->phasing_len);
    printf("    preamble        :   %-4u symbols\n", _q->preamble_len);
    printf("    payload         :   %-4u symbols\n", _q->payload_len);
    printf("    ramp\\dn         :   %-4u symbols\n", _q->rampdn_len);
    printf("    total           :   %-4u symbols\n", _q->frame_len);
    printf("  packet properties\n");
    printf("    crc             :   %s\n", crc_scheme_str[_q->check][1]);
    printf("    fec (inner)     :   %s\n", fec_scheme_str[_q->fec0][1]);
    printf("    fec (outer)     :   %s\n", fec_scheme_str[_q->fec1][1]);
    printf("  efficiency        :   %-8.3f b/s/Hz\n", eta);
}

// assemble frame
//  _q              :   frame generator object
//  _header         :   raw header
//  _payload        :   raw payload [size: _payload_len x 1]
//  _payload_len    :   raw payload length (bytes)
//  _check          :   data validity check
//  _fec0           :   inner forward error correction
//  _fec1           :   outer forward error correction
void gmskframegen_assemble(gmskframegen    _q,
                           unsigned char * _header,
                           unsigned char * _payload,
                           unsigned int    _payload_len,
                           crc_scheme      _check,
                           fec_scheme      _fec0,
                           fec_scheme      _fec1)
{
    // re-create frame generator if properties don't match
    if (_q->dec_msg_len != _payload_len ||
        _q->check       != _check       ||
        _q->fec0        != _fec0        ||
        _q->fec1        != _fec1)
    {
        // set properties
        _q->dec_msg_len = _payload_len;
        _q->check       = _check;
        _q->fec0        = _fec0;
        _q->fec1        = _fec1;

        // re-create payload packetizer
        _q->p_payload = packetizer_recreate(_q->p_payload, _q->dec_msg_len, _q->check, _q->fec0, _q->fec1);
        
        // get packet length
        _q->enc_msg_len = packetizer_get_enc_msg_len(_q->p_payload);
        _q->payload_len = 8*_q->enc_msg_len;

        // compute frame length (symbols)
        _q->frame_len = _q->rampup_len +
                        _q->phasing_len +
                        _q->preamble_len +
                        _q->header_len +
                        _q->payload_len +
                        _q->rampdn_len;

        // re-allocate memory
        _q->payload_enc = (unsigned char*) realloc(_q->payload_enc, _q->enc_msg_len*sizeof(unsigned char));
    }

    // encode header
    gmskframegen_encode_header(_q, _header);

    // encode payload
    packetizer_encode(_q->p_payload, _payload, _q->payload_enc);
}

// get length of frame (symbols)
unsigned int gmskframegen_get_frame_len(gmskframegen _q)
{
    return _q->frame_len;
}

// write sample to output buffer
int gmskframegen_write_samples(gmskframegen _q,
                               float complex * _y)
{
    switch (_q->state) {
    case GMSKFRAMEGEN_STATE_RAMPUP:
        // write ramp-up symbols
        gmskframegen_write_rampup(_q, _y);
        break;

    case GMSKFRAMEGEN_STATE_PHASING:
        // write phasing pattern
        gmskframegen_write_phasing(_q, _y);
        break;

    case GMSKFRAMEGEN_STATE_PREAMBLE:
        // write preamble
        gmskframegen_write_preamble(_q, _y);
        break;

    case GMSKFRAMEGEN_STATE_HEADER:
        // write header
        gmskframegen_write_header(_q, _y);
        break;

    case GMSKFRAMEGEN_STATE_PAYLOAD:
        // write ramp-up symbols
        gmskframegen_write_payload(_q, _y);
        break;

    case GMSKFRAMEGEN_STATE_RAMPDN:
        // write ramp-down symbols
        gmskframegen_write_rampdn(_q, _y);
        break;

    default:
        fprintf(stderr,"error: gmskframegen_writesymbol(), unknown/unsupported internal state\n");
        exit(1);
    }

    if (_q->frame_complete) {
        // reset framing object
#if DEBUG_GMSKFRAMEGEN
        printf(" ...resetting...\n");
#endif
        gmskframegen_reset(_q);
        return 1;
    }

    return 0;
}


// 
// internal methods
//

void gmskframegen_encode_header(gmskframegen _q,
                                unsigned char * _header)
{
    // first 'n' bytes user data
    memmove(_q->header_dec, _header, GMSKFRAME_H_USER);
    unsigned int n = GMSKFRAME_H_USER;

    // first byte is for expansion/version validation
    _q->header_dec[n+0] = GMSKFRAME_VERSION;

    // add payload length
    _q->header_dec[n+1] = (_q->dec_msg_len >> 8) & 0xff;
    _q->header_dec[n+2] = (_q->dec_msg_len     ) & 0xff;

    // add CRC, forward error-correction schemes
    //  CRC     : most-significant 3 bits of [n+4]
    //  fec0    : least-significant 5 bits of [n+4]
    //  fec1    : least-significant 5 bits of [n+5]
    _q->header_dec[n+3]  = (_q->check & 0x07) << 5;
    _q->header_dec[n+3] |= (_q->fec0) & 0x1f;
    _q->header_dec[n+4]  = (_q->fec1) & 0x1f;

    // run packet encoder
    packetizer_encode(_q->p_header, _q->header_dec, _q->header_enc);

    // scramble header
    scramble_data(_q->header_enc, GMSKFRAME_H_ENC);
#if 0
    printf("    header_enc      :");
    unsigned int i;
    for (i=0; i<GMSKFRAME_H_ENC; i++)
        printf(" %.2X", _q->header_enc[i]);
    printf("\n");
#endif
}

void gmskframegen_write_rampup(gmskframegen _q,
                               float complex * _y)
{
    unsigned char bit = rand() % 2;
    gmskmod_modulate(_q->mod, bit, _y);

    // TODO : check window...
    unsigned int i;
    for (i=0; i<_q->k; i++) {
        _y[i] *= hamming(_q->symbol_counter*_q->k + i, 2*_q->rampup_len*_q->k);
    }

    _q->symbol_counter++;

    if (_q->symbol_counter == _q->rampup_len) {
        _q->symbol_counter = 0;
        _q->state = GMSKFRAMEGEN_STATE_PHASING;
    }
}

void gmskframegen_write_phasing(gmskframegen _q,
                                float complex * _y)
{
    unsigned char bit = rand() % 2;
    gmskmod_modulate(_q->mod, bit, _y);

    _q->symbol_counter++;

    if (_q->symbol_counter == _q->phasing_len) {
        _q->symbol_counter = 0;
        _q->state = GMSKFRAMEGEN_STATE_PREAMBLE;
    }
}

void gmskframegen_write_preamble(gmskframegen _q,
                                 float complex * _y)
{
    unsigned char bit = msequence_advance(_q->ms_header);
    gmskmod_modulate(_q->mod, bit, _y);

    _q->symbol_counter++;

    if (_q->symbol_counter == _q->preamble_len) {
        msequence_reset(_q->ms_header);
        _q->symbol_counter = 0;
        _q->state = GMSKFRAMEGEN_STATE_HEADER;
    }
}

void gmskframegen_write_header(gmskframegen _q,
                               float complex * _y)
{
    div_t d = div(_q->symbol_counter, 8);
    unsigned int byte_index = d.quot;
    unsigned int bit_index  = d.rem;
    unsigned char byte = _q->header_enc[byte_index];
    unsigned char bit  = (byte >> (8-bit_index-1)) & 0x01;

    gmskmod_modulate(_q->mod, bit, _y);

    _q->symbol_counter++;
    
    if (_q->symbol_counter == _q->header_len) {
        _q->symbol_counter = 0;
        _q->state = GMSKFRAMEGEN_STATE_PAYLOAD;
    }
}

void gmskframegen_write_payload(gmskframegen _q,
                                float complex * _y)
{
    div_t d = div(_q->symbol_counter, 8);
    unsigned int byte_index = d.quot;
    unsigned int bit_index  = d.rem;
    unsigned char byte = _q->payload_enc[byte_index];
    unsigned char bit  = (byte >> (8-bit_index-1)) & 0x01;

    gmskmod_modulate(_q->mod, bit, _y);

    _q->symbol_counter++;
    
    if (_q->symbol_counter == _q->payload_len) {
        _q->symbol_counter = 0;
        _q->state = GMSKFRAMEGEN_STATE_RAMPDN;
    }
}

void gmskframegen_write_rampdn(gmskframegen _q,
                               float complex * _y)
{
    unsigned char bit = rand() % 2;
    gmskmod_modulate(_q->mod, bit, _y);

    // TODO : check window...
    unsigned int i;
    for (i=0; i<_q->k; i++) {
        _y[i] *= hamming(_q->rampdn_len*_q->k + _q->symbol_counter*_q->k + i, 2*_q->rampdn_len*_q->k);
    }

    _q->symbol_counter++;

    if (_q->symbol_counter == _q->rampdn_len) {
        _q->symbol_counter = 0;
        _q->frame_complete = 1;
    }
}

