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
// bpacketsync
//
// binary packet synchronizer/decoder
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "liquid.internal.h"

// bpacketsync object structure
struct bpacketsync_s {
    // options
    unsigned int g;                 // p/n sequence genpoly
    unsigned int pnsequence_len;    // p/n sequence length (bytes)
    unsigned int dec_msg_len;       // payload length
    crc_scheme crc;                 // payload check
    fec_scheme fec0;                // payload fec (inner)
    fec_scheme fec1;                // payload fec (outer)
    
    // derived values
    unsigned int enc_msg_len;       // encoded mesage length
    unsigned int header_len;        // header length (12 bytes encoded)
    unsigned int packet_len;        // total packet length

    // arrays
    unsigned char * pnsequence;     // p/n sequence
    unsigned char * payload_enc;    // payload (encoded)
    unsigned char * payload_dec;    // payload (decoded)

    // bpacket header
    //  0   :   version number
    //  1   :   crc
    //  2   :   fec0
    //  3   :   fec1
    //  4:5 :   payload length
    unsigned char header_dec[6];    // uncoded bytes
    unsigned char header_enc[12];   // 12 = 6 + crc16 at hamming(12,8)

    // objects
    msequence ms;
    packetizer p_header;
    packetizer p_payload;
    bsequence bpn;          // binary p/n sequence
    bsequence brx;          // binary received sequence

    // status variables
    enum {
        BPACKETSYNC_STATE_SEEKPN=0,     // seek p/n sequence
        BPACKETSYNC_STATE_RXHEADER,     // receive header data
        BPACKETSYNC_STATE_RXPAYLOAD     // receive payload data
    } state;

    // counters
    unsigned int num_payload_bytes;
    unsigned int num_payload_bits;
    unsigned char byte_rx;

    // flags
    int header_pass;
    int payload_pass;

    // user-defined parameters
    // callback
    // userdata
};

bpacketsync bpacketsync_create(unsigned int _m,
                               bpacketsync_callback _callback,
                               void * _userdata)
{
    // create bpacketsync object
    bpacketsync q = (bpacketsync) malloc(sizeof(struct bpacketsync_s));

    // default values
    q->dec_msg_len  = 1;
    q->crc          = CRC_NONE;
    q->fec0         = FEC_NONE;
    q->fec1         = FEC_NONE;

    // implied values
    q->g = 0;
    q->pnsequence_len = 8;

    // derived values
    q->enc_msg_len = packetizer_compute_enc_msg_len(q->dec_msg_len,
                                                    q->crc,
                                                    q->fec0,
                                                    q->fec1);
    q->header_len = packetizer_compute_enc_msg_len(6, CRC_16, FEC_NONE, FEC_HAMMING128);

    // arrays
    q->pnsequence = (unsigned char*) malloc((q->pnsequence_len)*sizeof(unsigned char*));

    // create m-sequence generator
    // TODO : configure sequence from generator polynomial
    q->ms = msequence_create(6);

    // create header packet encoder
    q->p_header = packetizer_create(6, CRC_16, FEC_NONE, FEC_HAMMING128);
    assert(q->header_len == packetizer_get_enc_msg_len(q->p_header));

    // create payload packet encoder
    q->p_payload = packetizer_create(q->dec_msg_len,
                                     q->crc,
                                     q->fec0,
                                     q->fec1);

    // create binary sequence objects
    q->bpn = bsequence_create(q->pnsequence_len*8);
    q->brx = bsequence_create(q->pnsequence_len*8);

    // assemble semi-static framing structures
    bpacketsync_assemble_pnsequence(q);

    // reset synchronizer
    bpacketsync_reset(q);

    return q;
}

void bpacketsync_destroy(bpacketsync _q)
{
    // free arrays
    free(_q->pnsequence);

    // destroy internal objects
    msequence_destroy(_q->ms);
    packetizer_destroy(_q->p_header);
    packetizer_destroy(_q->p_payload);
    bsequence_destroy(_q->bpn);
    bsequence_destroy(_q->brx);

    // free main object memory
    free(_q);
}

void bpacketsync_print(bpacketsync _q)
{
    printf("bpacketsync:\n");
    printf("    p/n poly    :   0x%.4x\n", _q->g);
    printf("    p/n len     :   %u bytes\n", _q->pnsequence_len);
    printf("    header len  :   %u bytes\n", _q->header_len);
    printf("    payload len :   %u bytes\n", _q->dec_msg_len);
    printf("    crc         :   %s\n", crc_scheme_str[_q->crc][1]);
    printf("    fec (inner) :   %s\n", fec_scheme_str[_q->fec0][1]);
    printf("    fec (outer) :   %s\n", fec_scheme_str[_q->fec1][1]);
    printf("    packet len  :   %u bytes\n", _q->packet_len);
    printf("    efficiency  :   %8.2f %%\n", 100.0f*(float)_q->dec_msg_len/(float)_q->packet_len);
}

void bpacketsync_reset(bpacketsync _q)
{
    // clear received sequence buffer
    bsequence_clear(_q->brx);

    // reset counters
    _q->num_payload_bytes = 0;
    _q->num_payload_bits  = 0;
    _q->byte_rx           = 0;

    // reset state
    _q->state = BPACKETSYNC_STATE_SEEKPN;
}

void bpacketsync_execute(bpacketsync _q,
                         unsigned char _b)
{
    unsigned int j;
    for (j=0; j<8; j++) {
        // strip bit from byte
        unsigned char bit = (_b >> (8-j-1)) & 1;

        // execute state-specific methods
        switch (_q->state) {
        case BPACKETSYNC_STATE_SEEKPN:
            bpacketsync_execute_seekpn(_q, bit);
            break;
        case BPACKETSYNC_STATE_RXHEADER:
            bpacketsync_execute_rxheader(_q, bit);
            break;
        case BPACKETSYNC_STATE_RXPAYLOAD:
            bpacketsync_execute_rxpayload(_q, bit);
            break;
        default:
            fprintf(stderr,"error: bpacketsync_execute(), invalid state\n");
            exit(1);
        }
    }
}


// 
// internal methods
//

void bpacketsync_assemble_pnsequence(bpacketsync _q)
{
    // reset m-sequence generator
    msequence_reset(_q->ms);

    unsigned int i;
    for (i=0; i<8*_q->pnsequence_len; i++)
        bsequence_push(_q->bpn, msequence_advance(_q->ms));
}


void bpacketsync_execute_seekpn(bpacketsync _q,
                                unsigned char _bit)
{
    // push bit into correlator
    bsequence_push(_q->brx, _bit);

    // compute p/n sequence correlation
    int rxy = bsequence_correlate(_q->bpn, _q->brx);
    float r = (float)rxy / (float)(_q->pnsequence_len*8);
    printf("rxy = %d\n", rxy);

    // check threshold
    if ( r > 0.8f ) {
        printf("p/n sequence found!\n");

        // TODO : check polarity or rxy
        // TODO : switch operational mode
    }
}

void bpacketsync_execute_rxheader(bpacketsync _q,
                                  unsigned char _bit)
{
    // push bit into accumulated byte
    _q->byte_rx <<= 1;
    _q->byte_rx |= (_bit & 1);
    _q->num_payload_bits++;
    
    if (_q->num_payload_bits == 8) {
        _q->num_payload_bits=0;
        _q->num_payload_bytes++;

        if (_q->num_payload_bytes == _q->header_len) {
            printf("header received\n");
            
            _q->num_payload_bits  = 0;
            _q->num_payload_bytes = 0;

            // TODO : decode header

            // switch operational mode
            _q->state = BPACKETSYNC_STATE_RXPAYLOAD;
        }
    }
}

void bpacketsync_execute_rxpayload(bpacketsync _q,
                                   unsigned char _bit)
{
    // push bit into accumulated byte
    _q->byte_rx <<= 1;
    _q->byte_rx |= (_bit & 1);
    _q->num_payload_bits++;
    
    if (_q->num_payload_bits == 8) {
        _q->num_payload_bits=0;
        _q->num_payload_bytes++;

        if (_q->num_payload_bytes == _q->enc_msg_len) {
            printf("payload received\n");
            
            _q->num_payload_bits  = 0;
            _q->num_payload_bytes = 0;

            // TODO : decode payload

            // switch operational mode
            _q->state = BPACKETSYNC_STATE_SEEKPN;
        }
    }
}

