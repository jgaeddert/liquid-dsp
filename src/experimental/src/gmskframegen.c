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

#include "liquid.experimental.h"

#define DEBUG_GMSKFRAMEGEN    0

// gmskframe object structure
struct gmskframegen_s {
    gmskmod mod;                // GMSK modulator
    unsigned int k;             // filter samples/symbol
    unsigned int m;             // filter semi-length (symbols)
    float BT;                   // filter bandwidth-time product

    // framing generator
    bpacketgen pgen;            // packet generator
    crc_scheme check;           // CRC
    fec_scheme fec0;            // inner forward error correction
    fec_scheme fec1;            // outer forward error correction
    unsigned int dec_msg_len;   // 
    unsigned int enc_msg_len;   // 
    unsigned char * packet;     // encoded packet

    //
    unsigned int rampup_len;    //
    unsigned int preamble_len;  //
    unsigned int payload_len;   //
    unsigned int rampdn_len;    //
    enum {
        GMSKFRAMEGEN_STATE_RAMPUP,      // ramp up
        GMSKFRAMEGEN_STATE_PREAMBLE,    // preamble
        GMSKFRAMEGEN_STATE_PAYLOAD,     // payload (frame)
        GMSKFRAMEGEN_STATE_RAMPDN,      // ramp down
    } state;
    int frame_complete;         //
    unsigned int symbol_counter;//

    // buffering
    unsigned int num_samples;   // total number of samples in the frame
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
    q->rampup_len = 4;      // number of ramp/up symbols
    q->preamble_len = 16;   // number of preamble symbols
    q->payload_len = 0;     // number of payload symbols
    q->rampdn_len = 4;      // number of ramp\dn symbols

    // create modulator
    q->mod = gmskmod_create(q->k, q->m, q->BT);

    // create packet generator
    q->dec_msg_len = 0;
    q->check = LIQUID_CRC_32;
    q->fec0  = LIQUID_FEC_HAMMING128;
    q->fec1  = LIQUID_FEC_NONE;
    q->pgen  = bpacketgen_create(0, q->dec_msg_len, q->check, q->fec0, q->fec1);

    q->enc_msg_len = bpacketgen_get_packet_len(q->pgen);
    q->payload_len = 8*q->enc_msg_len;

    // allocate memory for encoded packet
    q->packet = (unsigned char*) malloc(q->enc_msg_len*sizeof(unsigned char));

    // reset framing object
    gmskframegen_reset(q);

    return q;
}

// destroy gmskframegen object
void gmskframegen_destroy(gmskframegen _q)
{
    // destroy gmsk modulator
    gmskmod_destroy(_q->mod);

    // destroy packet generator
    bpacketgen_destroy(_q->pgen);

    // free arrays
    free(_q->packet);   // encoded packet array

    // free main object memory
    free(_q);
}

// reset frame generator object
void gmskframegen_reset(gmskframegen _q)
{
    // reset GMSK modulator
    gmskmod_reset(_q->mod);

    // reset state
    _q->state = GMSKFRAMEGEN_STATE_RAMPUP;
    _q->frame_complete = 0;
    _q->symbol_counter = 0;
}

// print gmskframegen object internals
void gmskframegen_print(gmskframegen _q)
{
    printf("gmskframegen:\n");
    //printf("    samples/symbol  :   %u\n", _q->k);
}

// assemble frame
//  _q              :   frame generator object
//  _payload        :   raw payload [size: _payload_len x 1]
//  _payload_len    :   raw payload length (bytes)
void gmskframegen_assemble(gmskframegen _q,
                           unsigned char * _payload,
                           unsigned int    _payload_len)
{
    // re-create frame generator if lengths don't match
    // TODO : also check FEC, CRC, etc.
    if (_q->dec_msg_len != _payload_len) {
        _q->dec_msg_len = _payload_len;
        _q->pgen = bpacketgen_recreate(_q->pgen, 0, _q->dec_msg_len, _q->check, _q->fec0, _q->fec1);
        
        // get packet length
        _q->enc_msg_len = bpacketgen_get_packet_len(_q->pgen);
        _q->payload_len = 8*_q->enc_msg_len;

        // re-allocate memory
        _q->packet = (unsigned char*) realloc(_q->packet, _q->enc_msg_len*sizeof(unsigned char));
    }

    // generate packet
    bpacketgen_encode(_q->pgen, _payload, _q->packet);

    // reset counters
    //_q->num_samples   = _q->rampup_len + _q->preamble_len + _q->payload_len + _q->rampdn_len;
    _q->num_samples   = (_q->payload_len + 2*_q->m)*_q->k;
}

// get length of frame (samples)
unsigned int gmskframegen_get_frame_len(gmskframegen _q)
{
    return _q->num_samples;
}

// write sample to output buffer
int gmskframegen_write_samples(gmskframegen _q,
                               float complex * _y)
{
    switch (_q->state) {
    case GMSKFRAMEGEN_STATE_RAMPUP:
        printf("ramp up\n");
        // write ramp-up symbols
        gmskframegen_write_rampup(_q, _y);
        break;

    case GMSKFRAMEGEN_STATE_PREAMBLE:
        printf("preamble\n");
        // write preamble
        gmskframegen_write_preamble(_q, _y);
        break;

    case GMSKFRAMEGEN_STATE_PAYLOAD:
        printf("payload\n");
        // write ramp-up symbols
        gmskframegen_write_payload(_q, _y);
        break;

    case GMSKFRAMEGEN_STATE_RAMPDN:
        printf("ramp down\n");
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
        _q->state = GMSKFRAMEGEN_STATE_PREAMBLE;
    }
}

void gmskframegen_write_preamble(gmskframegen _q,
                                 float complex * _y)
{
    unsigned char bit = rand() % 2;
    gmskmod_modulate(_q->mod, bit, _y);

    _q->symbol_counter++;

    if (_q->symbol_counter == _q->preamble_len) {
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
    unsigned char byte = _q->packet[byte_index];
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

    if (_q->symbol_counter == _q->rampup_len) {
        _q->symbol_counter = 0;
        _q->frame_complete = 1;
    }
}

