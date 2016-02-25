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
// bpacketsync
//
// binary packet synchronizer/decoder
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_BPACKETSYNC   0

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
    unsigned int num_bytes_received;
    unsigned int num_bits_received;
    unsigned char byte_rx;
    unsigned char byte_mask;

    // flags
    int header_valid;
    int payload_valid;

    // user-defined parameters
    bpacketsync_callback callback;
    void * userdata;
    framesyncstats_s framestats;
};

bpacketsync bpacketsync_create(unsigned int _m,
                               bpacketsync_callback _callback,
                               void * _userdata)
{
    // create bpacketsync object
    bpacketsync q = (bpacketsync) malloc(sizeof(struct bpacketsync_s));
    q->callback = _callback;
    q->userdata = _userdata;

    // default values
    q->dec_msg_len  = 1;
    q->crc          = LIQUID_CRC_NONE;
    q->fec0         = LIQUID_FEC_NONE;
    q->fec1         = LIQUID_FEC_NONE;

    // implied values
    q->g = 0;
    q->pnsequence_len = 8;

    // derived values
    q->enc_msg_len = packetizer_compute_enc_msg_len(q->dec_msg_len,
                                                    q->crc,
                                                    q->fec0,
                                                    q->fec1);
    q->header_len = packetizer_compute_enc_msg_len(6, LIQUID_CRC_16, LIQUID_FEC_NONE, LIQUID_FEC_HAMMING128);

    // arrays
    q->pnsequence  = malloc((q->pnsequence_len)*sizeof(unsigned char));
    q->payload_enc = malloc((q->enc_msg_len)*sizeof(unsigned char));
    q->payload_dec = malloc((q->dec_msg_len)*sizeof(unsigned char));

    // create m-sequence generator
    // TODO : configure sequence from generator polynomial
    q->ms = msequence_create_default(6);

    // create header packet encoder
    q->p_header = packetizer_create(6, LIQUID_CRC_16, LIQUID_FEC_NONE, LIQUID_FEC_HAMMING128);
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
    free(_q->payload_enc);
    free(_q->payload_dec);

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
    _q->num_bytes_received  = 0;
    _q->num_bits_received   = 0;
    _q->byte_rx             = 0;
    _q->byte_mask           = 0x00;

    // reset state
    _q->state = BPACKETSYNC_STATE_SEEKPN;
}

// run synchronizer on array of input bytes
//  _q      :   bpacketsync object
//  _bytes  :   input data array [size: _n x 1]
//  _n      :   input array size
void bpacketsync_execute(bpacketsync _q,
                         unsigned char * _bytes,
                         unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        bpacketsync_execute_byte(_q, _bytes[i]);
}

// run synchronizer on input byte
//  _q      :   bpacketsync object
//  _byte   :   input byte
void bpacketsync_execute_byte(bpacketsync _q,
                              unsigned char _byte)
{
    unsigned int j;
    for (j=0; j<8; j++) {
        // strip bit from byte
        unsigned char bit = (_byte >> (8-j-1)) & 1;

        // run synchronizer on bit
        bpacketsync_execute_bit(_q, bit);
    }
}

// run synchronizer on input symbol
//  _q      :   bpacketsync object
//  _sym    :   input symbol with _bps significant bits
//  _bps    :   number of bits in input symbol
void bpacketsync_execute_sym(bpacketsync _q,
                             unsigned char _sym,
                             unsigned int _bps)
{
    // validate input
    if (_bps > 8) {
        fprintf(stderr,"error: bpacketsync_execute_sym(), bits per symbol must be in [0,8]\n");
        exit(1);
    }

    unsigned int j;
    for (j=0; j<_bps; j++) {
        // strip bit from byte
        unsigned char bit = (_sym >> (_bps-j-1)) & 1;

        // run synchronizer on bit
        bpacketsync_execute_bit(_q, bit);
    }
}


// execute one bit at a time
void bpacketsync_execute_bit(bpacketsync _q,
                             unsigned char _bit)
{
    // mask input to ensure one bit of resolution
    _bit = _bit & 0x01;

    // execute state-specific methods
    switch (_q->state) {
    case BPACKETSYNC_STATE_SEEKPN:
        bpacketsync_execute_seekpn(_q, _bit);
        break;
    case BPACKETSYNC_STATE_RXHEADER:
        bpacketsync_execute_rxheader(_q, _bit);
        break;
    case BPACKETSYNC_STATE_RXPAYLOAD:
        bpacketsync_execute_rxpayload(_q, _bit);
        break;
    default:
        fprintf(stderr,"error: bpacketsync_execute(), invalid state\n");
        exit(1);
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
    float r = 2.0f*(float)rxy / (float)(_q->pnsequence_len*8) - 1.0f;

    // check threshold
    if ( fabsf(r) > 0.8f ) {
#if DEBUG_BPACKETSYNC
        printf("p/n sequence found!, rxy = %8.4f\n", r);
#endif

        // flip polarity of bits if correlation is negative
        _q->byte_mask = r > 0 ? 0x00 : 0xff;

        // switch operational mode
        _q->state = BPACKETSYNC_STATE_RXHEADER;
    }
}

void bpacketsync_execute_rxheader(bpacketsync _q,
                                  unsigned char _bit)
{
    // push bit into accumulated byte
    _q->byte_rx <<= 1;
    _q->byte_rx |= (_bit & 1);
    _q->num_bits_received++;
    
    if (_q->num_bits_received == 8) {
        // append byte to encoded header array
        _q->header_enc[_q->num_bytes_received] = _q->byte_rx ^ _q->byte_mask;

        _q->num_bits_received=0;
        _q->num_bytes_received++;

        if (_q->num_bytes_received == _q->header_len) {
            
            _q->num_bits_received  = 0;
            _q->num_bytes_received = 0;

            // decode header
            bpacketsync_decode_header(_q);

            // TODO : invoke header callback now

            if (_q->header_valid) {
                // re-allocate memory for arrays
                bpacketsync_reconfig(_q);

                // switch operational mode
                _q->state = BPACKETSYNC_STATE_RXPAYLOAD;
            } else {
                // reset synchronizer
                bpacketsync_reset(_q);
            }
        }
    }
}

void bpacketsync_execute_rxpayload(bpacketsync _q,
                                   unsigned char _bit)
{
    // push bit into accumulated byte
    _q->byte_rx <<= 1;
    _q->byte_rx |= (_bit & 1);
    _q->num_bits_received++;
    
    if (_q->num_bits_received == 8) {
        // append byte to encoded payload array
        _q->payload_enc[_q->num_bytes_received] = _q->byte_rx ^ _q->byte_mask;

        _q->num_bits_received=0;
        _q->num_bytes_received++;

        if (_q->num_bytes_received == _q->enc_msg_len) {
            
            _q->num_bits_received  = 0;
            _q->num_bytes_received = 0;

            // decode payload data
            bpacketsync_decode_payload(_q);

            // invoke callback
            if (_q->callback != NULL) {
                // set frame stats
                framesyncstats_init_default(&_q->framestats);
                _q->framestats.check = _q->crc;
                _q->framestats.fec0  = _q->fec0;
                _q->framestats.fec1  = _q->fec1;

                _q->callback(_q->payload_dec,
                             _q->payload_valid,
                             _q->dec_msg_len,
                             _q->framestats,
                             _q->userdata);
            }

            // reset synchronizer
            bpacketsync_reset(_q);
        }
    }
}

void bpacketsync_decode_header(bpacketsync _q)
{
    // decode header array
    _q->header_valid = packetizer_decode(_q->p_header,
                                         _q->header_enc,
                                         _q->header_dec);

    // return unconditionally if header failed
    if (!_q->header_valid)
        return;

    // strip header info
    int version = _q->header_dec[0];
    _q->crc  = (crc_scheme) _q->header_dec[1];
    _q->fec0 = (fec_scheme) _q->header_dec[2];
    _q->fec1 = (fec_scheme) _q->header_dec[3];
    _q->dec_msg_len = (_q->header_dec[4] << 8) |
                      (_q->header_dec[5]     );

    // check version number
    if (version != BPACKET_VERSION)
        fprintf(stderr,"warning: bpacketsync, version mismatch!\n");

    // TODO : check crc, fec0, fec1 schemes
}

void bpacketsync_decode_payload(bpacketsync _q)
{
    // decode payload
    _q->payload_valid = packetizer_decode(_q->p_payload,
                                          _q->payload_enc,
                                          _q->payload_dec);
}

void bpacketsync_reconfig(bpacketsync _q)
{
    // reconfigure packetizer
    _q->p_payload = packetizer_recreate(_q->p_payload,
                                        _q->dec_msg_len,
                                        _q->crc,
                                        _q->fec0,
                                        _q->fec1);

    // re-compute encoded message (packet) length
    _q->enc_msg_len = packetizer_get_enc_msg_len(_q->p_payload);

    // re-allocate memory for encoded packet
    _q->payload_enc = (unsigned char*) realloc(_q->payload_enc,
                                               _q->enc_msg_len*sizeof(unsigned char));

    // re-allocate memory for decoded packet
    _q->payload_dec = (unsigned char*) realloc(_q->payload_dec,
                                               _q->dec_msg_len*sizeof(unsigned char));
}

