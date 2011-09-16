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

    // buffering
    unsigned int num_samples;   // total number of samples in the frame
    unsigned int num_remaining; // number of remaining samples to write
    unsigned int byte_index;    // 
    unsigned int bit_index;     // 
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

    // create modulator
    q->mod = gmskmod_create(q->k, q->m, q->BT);

    // create packet generator
    q->dec_msg_len = 0;
    q->check = LIQUID_CRC_32;
    q->fec0  = LIQUID_FEC_HAMMING128;
    q->fec1  = LIQUID_FEC_NONE;
    q->pgen  = bpacketgen_create(0, q->dec_msg_len, q->check, q->fec0, q->fec1);

    q->enc_msg_len = bpacketgen_get_packet_len(q->pgen);

    // allocate memory for encoded packet
    q->packet = (unsigned char*) malloc(q->enc_msg_len*sizeof(unsigned char));

    return q;
}

// destroy gmskframegen object
void gmskframegen_destroy(gmskframegen _q)
{
    // destroy gmsk modulator
    gmskmod_destroy(_q->mod);

    // destroy packet generator
    bpacketgen_destroy(_q->pgen);

    // free encoded packet array
    free(_q->packet);

    // free main object memory
    free(_q);
}

// reset frame generator object
void gmskframegen_reset(gmskframegen _q)
{
    // reset GMSK modulator
    gmskmod_reset(_q->mod);
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

        // re-allocate memory
        _q->packet = (unsigned char*) realloc(_q->packet, _q->enc_msg_len*sizeof(unsigned char));
    }

    // generate packet
    bpacketgen_encode(_q->pgen, _payload, _q->packet);

    // reset counters
    _q->num_samples   = (8*_q->enc_msg_len + 2*_q->m) * _q->k;
    _q->num_remaining = _q->num_samples;
    _q->byte_index    = 0;
    _q->bit_index     = 0;
}

// get length of frame (samples)
unsigned int gmskframegen_get_frame_len(gmskframegen _q)
{
    return _q->num_samples;
}

// write sample to output buffer
int gmskframegen_write_samples(gmskframegen _q,
                               liquid_float_complex * _y,
                               unsigned int   _num_available,
                               unsigned int * _num_written)
{
    // TODO : write output in blocks of size 'k'

    // validate input
    if (_num_available < _q->num_samples) {
        fprintf(stderr,"error: gmskframegen_write_samples(), too few samples available\n");
        exit(1);
    }

    // modulate entire frame and write output
    unsigned int i;
    unsigned int j;
    unsigned int n=0;

    // 
    for (i=0; i<_q->m; i++) {
        unsigned char bit = rand() % 2;
        gmskmod_modulate(_q->mod, bit, &_y[n]);
        n += _q->k;
    }

    for (i=0; i<_q->enc_msg_len; i++) {
        unsigned char byte = _q->packet[i];
        for (j=0; j<8; j++) {
            unsigned char bit = (byte >> (8-j-1)) & 0x01;
            gmskmod_modulate(_q->mod, bit, &_y[n]);
            n += _q->k;
        }
    }

    // 
    for (i=0; i<_q->m; i++) {
        unsigned char bit = rand() % 2;
        gmskmod_modulate(_q->mod, bit, &_y[n]);
        n += _q->k;
    }

    return 1;
}


