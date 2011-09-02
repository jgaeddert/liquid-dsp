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
// Packetizer (experimental improvements)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"
#include "liquid.experimental.h"

// Execute the packetizer to decode an input message, return validity
// check of resulting data; persistent attempts to find error(s)
//
//  _p      :   packetizer object
//  _pkt    :   input message (coded bytes)
//  _msg    :   decoded output message
int packetizer_decode_persistent(packetizer _p,
                                 unsigned char * _pkt,
                                 unsigned char * _msg)
{
    // try regular decoding
    int crc_pass = packetizer_decode(_p, _pkt, _msg);

    // return if decoding was successful
    if (crc_pass)
        return crc_pass;

    unsigned int i;
    unsigned int key=0;

    // result is stored in _p->buffer_0; flip bits to try to get
    // CRC to pass
    for (i=0; i<_p->msg_len + _p->crc_length; i++) {
        unsigned int j;
        for (j=0; j<8; j++) {
            // flip bit
            unsigned char mask = 1 << (8-j-1);
            _p->buffer_0[i] ^= mask;

            // strip crc, validate message
            key = 0;
            unsigned int k;
            for (k=0; k<_p->crc_length; k++) {
                key <<= 8;

                key |= _p->buffer_0[_p->msg_len+k];
            }

            // compute crc validity
            crc_pass = crc_validate_message(_p->check,
                                            _p->buffer_0,
                                            _p->msg_len,
                                            key);

            // check validity
            if (crc_pass) {
                // copy result to output and return
                memmove(_msg, _p->buffer_0, _p->msg_len);
                printf("persistent decoding worked!\n");
                return crc_pass;
            } else {
                // flip bit back
                _p->buffer_0[i] ^= mask;
            }
        }
    }
    
    // copy result to output and return
    memmove(_msg, _p->buffer_0, _p->msg_len);
    return crc_pass;
}
    
// double bit errors in a single byte
//  nchoosek(8,2) = 28
unsigned char packetizer_persistent_mask2[28] = {
    0x03,   // 00000011
    0x06,   // 00000110
    0x0C,   // 00001100
    0x18,   // 00011000
    0x30,   // 00110000
    0x60,   // 01100000
    0xC0,   // 11000000

    0x05,   // 00000101
    0x0A,   // 00001010
    0x14,   // 00010100
    0x28,   // 00101000
    0x50,   // 01010000
    0xA0,   // 10100000

    0x09,   // 00001001
    0x12,   // 00010010
    0x24,   // 00100100
    0x48,   // 01001000
    0x90,   // 10010000

    0x11,   // 00010001
    0x22,   // 00100010
    0x44,   // 01000100
    0x88,   // 10001000

    0x21,   // 00100001
    0x42,   // 01000010
    0x84,   // 10000100

    0x41,   // 01000001
    0x82,   // 10000010

    0x81,   // 10000001
};

// Execute the packetizer to decode an input message, return validity
// check of resulting data; persistent attempts to find error(s)
//
//  _p      :   packetizer object
//  _pkt    :   input message (coded bytes)
//  _msg    :   decoded output message
int packetizer_decode_persistent2(packetizer _p,
                                  unsigned char * _pkt,
                                  unsigned char * _msg)
{
    // try first-order persistent decoding
    int crc_pass = packetizer_decode_persistent(_p, _pkt, _msg);

    // return if decoding was successful
    if (crc_pass)
        return crc_pass;

    unsigned int i;
    unsigned int key=0;

    // result is stored in _p->buffer_0; flip bits to try to get
    // CRC to pass
    for (i=0; i<_p->msg_len + _p->crc_length; i++) {
        unsigned int j;
        for (j=0; j<28; j++) {
            // apply mask
            _p->buffer_0[i] ^= packetizer_persistent_mask2[j];

            // strip crc, validate message
            key = 0;
            unsigned int k;
            for (k=0; k<_p->crc_length; k++) {
                key <<= 8;

                key |= _p->buffer_0[_p->msg_len+k];
            }

            // compute crc validity
            crc_pass = crc_validate_message(_p->check,
                                            _p->buffer_0,
                                            _p->msg_len,
                                            key);

            // check validity
            if (crc_pass) {
                // copy result to output and return
                memmove(_msg, _p->buffer_0, _p->msg_len);
                printf("persistent decoding worked! (2)\n");
                return crc_pass;
            } else {
                // flip bit back
                _p->buffer_0[i] ^= packetizer_persistent_mask2[j];
            }
        }
    }
    
    // copy result to output and return
    memmove(_msg, _p->buffer_0, _p->msg_len);
    return crc_pass;
}

