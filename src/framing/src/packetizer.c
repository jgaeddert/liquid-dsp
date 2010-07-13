/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// Packetizer
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// packetizer_get_packet_length()
//
// returns the length of encoded bytes after packetizing
//
//  _n      :   number of uncoded input bytes
//  _fec0   :   inner forward error-correction code
//  _fec1   :   outer forward error-correction code
unsigned int packetizer_get_packet_length(unsigned int _n,
                                          int _fec0,
                                          int _fec1)
{
    return fec_get_enc_msg_length(_fec1,
                fec_get_enc_msg_length(_fec0, _n+4) );
}

// packetizer_create()
//
// create packetizer object
//
//  _n      :   number of uncoded intput bytes
//  _fec0   :   inner forward error-correction code
//  _fec1   :   outer forward error-correction code
packetizer packetizer_create(unsigned int _n,
                             int _fec0,
                             int _fec1)
{
    packetizer p = (packetizer) malloc(sizeof(struct packetizer_s));

    p->msg_len = _n;
    p->packet_len = packetizer_get_packet_length(_n, _fec0, _fec1);

    // allocate memory for buffers
    p->buffer_len = p->packet_len;
    p->buffer_0 = (unsigned char*) malloc(p->buffer_len);
    p->buffer_1 = (unsigned char*) malloc(p->buffer_len);

    // create plan
    p->plan_len = 2;
    p->plan = (struct fecintlv_plan*) malloc((p->plan_len)*sizeof(struct fecintlv_plan));

    // set schemes
    unsigned int i, n0=_n+4;
    for (i=0; i<p->plan_len; i++) {
        // set schemes
        p->plan[i].fs = (i==0) ? _fec0 : _fec1;
        p->plan[i].intlv_scheme = LIQUID_INTERLEAVER_BLOCK;

        // compute lengths
        p->plan[i].dec_msg_len = n0;
        p->plan[i].enc_msg_len = fec_get_enc_msg_length(p->plan[i].fs,
                                                        p->plan[i].dec_msg_len);

        // create objects
        p->plan[i].f = fec_create(p->plan[i].fs, NULL);
        p->plan[i].q = interleaver_create(p->plan[i].enc_msg_len,
                                          p->plan[i].intlv_scheme);

        // update length
        n0 = p->plan[i].enc_msg_len;
    }

    return p;
}

// packetizer_recreate()
//
// re-create packetizer object
//
//  _p      :   initialz packetizer object
//  _n      :   number of uncoded intput bytes
//  _fec0   :   inner forward error-correction code
//  _fec1   :   outer forward error-correction code
packetizer packetizer_recreate(packetizer _p,
                               unsigned int _n,
                               int _fec0,
                               int _fec1)
{
    if (_p == NULL) {
        // packetizer was never created
        return packetizer_create(_n, _fec0, _fec1);
    }

    // check values
    if (_p->msg_len     ==  _n      &&
        _p->plan[0].fs  ==  _fec0   &&
        _p->plan[1].fs  ==  _fec1 )
    {
        // no change; return input pointer
        return _p;
    } else {
        // something has changed; destroy old object and create new one
        // TODO : rather than completely destroying object, only change values that are necessary
        packetizer_destroy(_p);
        return packetizer_create(_n,_fec0,_fec1);
    }
}

// destroy packetizer object
void packetizer_destroy(packetizer _p)
{
    // free fec, interleaver objects
    unsigned int i;
    for (i=0; i<_p->plan_len; i++) {
        fec_destroy(_p->plan[i].f);
        interleaver_destroy(_p->plan[i].q);
    };

    // free plan
    free(_p->plan);

    // free buffers
    free(_p->buffer_0);
    free(_p->buffer_1);

    // free packetizer object
    free(_p);
}

// print packetizer object internals
void packetizer_print(packetizer _p)
{
    printf("packetizer [dec: %u, enc: %u]\n", _p->msg_len, _p->packet_len);
    printf("     : crc32    %-10u %-10u %-16s\n",_p->msg_len,_p->msg_len+4,"crc32"); // crc-key
    unsigned int i;
    for (i=0; i<_p->plan_len; i++) {
        printf("%4u : fec      %-10u %-10u %-16s\n",
            i,
            _p->plan[i].dec_msg_len,
            _p->plan[i].enc_msg_len,
            fec_scheme_str[_p->plan[i].fs]);
    }
}

// get decoded message length
unsigned int packetizer_get_dec_msg_len(packetizer _p)
{
    return _p->msg_len;
}

// get encoded message length
unsigned int packetizer_get_enc_msg_len(packetizer _p)
{
    return _p->packet_len;
}

// packetizer_encode()
//
// Execute the packetizer on an input message
//
//  _p      :   packetizer object
//  _msg    :   input message (uncoded bytes)
//  _pkt    :   encoded output message
void packetizer_encode(packetizer _p,
                       unsigned char * _msg,
                       unsigned char * _pkt)
{
    // copy input message to internal buffer[0]
    memmove(_p->buffer_0, _msg, _p->msg_len);

    // compute crc32, append to buffer
    _p->crc32_key = crc32_generate_key(_p->buffer_0, _p->msg_len);
    unsigned int crc32_key = _p->crc32_key;
    _p->buffer_0[_p->msg_len+0] = (crc32_key & 0x000000ff) >> 0;
    _p->buffer_0[_p->msg_len+1] = (crc32_key & 0x0000ff00) >> 8;
    _p->buffer_0[_p->msg_len+2] = (crc32_key & 0x00ff0000) >> 16;
    _p->buffer_0[_p->msg_len+3] = (crc32_key & 0xff000000) >> 24;

    // execute fec/interleaver plans
    unsigned int i;
    for (i=0; i<_p->plan_len; i++) {
        // run the encoder: buffer[0] > buffer[1]
        fec_encode(_p->plan[i].f,
                   _p->plan[i].dec_msg_len,
                   _p->buffer_0,
                   _p->buffer_1);

        // run the interleaver: buffer[1] > buffer[0]
        interleaver_encode(_p->plan[i].q,
                           _p->buffer_1,
                           _p->buffer_0);
    }

    // copy result to output
    memmove(_pkt, _p->buffer_0, _p->packet_len);
}

// packetizer_decode()
//
// Execute the packetizer to decode an input message, return validity
// check of resulting data
//
//  _p      :   packetizer object
//  _pkt    :   input message (coded bytes)
//  _msg    :   decoded output message
int packetizer_decode(packetizer _p,
                      unsigned char * _pkt,
                      unsigned char * _msg)
{
    // copy coded message to internal buffer[0]
    memmove(_p->buffer_0, _pkt, _p->packet_len);

    // execute fec/interleaver plans
    unsigned int i;
    for (i=_p->plan_len; i>0; i--) {
        // run the de-interleaver: buffer[0] > buffer[1]
        interleaver_decode(_p->plan[i-1].q,
                           _p->buffer_0,
                           _p->buffer_1);

        // run the decoder: buffer[1] > buffer[0]
        fec_decode(_p->plan[i-1].f,
                   _p->plan[i-1].dec_msg_len,
                   _p->buffer_1,
                   _p->buffer_0);
    }

    // strip crc32, validate message
    unsigned int crc32_key = 0;
    crc32_key |= _p->buffer_0[_p->msg_len+0] << 0;
    crc32_key |= _p->buffer_0[_p->msg_len+1] << 8;
    crc32_key |= _p->buffer_0[_p->msg_len+2] << 16;
    crc32_key |= _p->buffer_0[_p->msg_len+3] << 24;

    // copy result to output
    memmove(_msg, _p->buffer_0, _p->msg_len);

    // return crc validity
    return crc32_validate_message(_p->buffer_0,
                                  _p->msg_len,
                                  crc32_key);
}

void packetizer_set_scheme(packetizer _p, int _fec0, int _fec1)
{
    //
}

// 
// internal methods
//

void packetizer_realloc_buffers(packetizer _p, unsigned int _len)
{
    _p->buffer_len = _len;
    _p->buffer_0 = (unsigned char*) realloc(_p->buffer_0, _p->buffer_len);
    _p->buffer_1 = (unsigned char*) realloc(_p->buffer_1, _p->buffer_len);
}

