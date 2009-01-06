//
// Packetizer
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "framing_internal.h"

packetizer packetizer_create(
    unsigned int _n,
    fec_scheme _fec0,
    fec_scheme _fec1)
{
    packetizer p = (packetizer) malloc(sizeof(struct packetizer_s));

    bool buffer_switch = true;

    p->dec_msg_len = _n;

    // fec (inner)
    p->fec0_scheme = _fec0;
    p->fec0_dec_msg_len = p->dec_msg_len + 4;
    p->fec0_enc_msg_len = fec_get_enc_msg_length(p->fec0_scheme, p->fec0_dec_msg_len);
    p->fec0 = fec_create(p->fec0_scheme, p->fec0_dec_msg_len, NULL);

    // intlv (inner)
    p->intlv0_len = p->fec0_enc_msg_len;
    p->intlv0 = interleaver_create(p->intlv0_len, INT_BLOCK);

    // fec (outer)
    p->fec1_scheme = _fec1;
    p->fec1_dec_msg_len = p->fec0_enc_msg_len;
    p->fec1_enc_msg_len = fec_get_enc_msg_length(p->fec1_scheme, p->fec1_dec_msg_len);
    p->fec1 = fec_create(p->fec1_scheme, p->fec1_dec_msg_len, NULL);

    // intlv (outer)
    p->intlv1_len = p->fec1_enc_msg_len;
    p->intlv1 = interleaver_create(p->intlv1_len, INT_BLOCK);

    p->enc_msg_len = p->intlv1_len;

    // allocate memory for buffers
    p->buffer_len = p->enc_msg_len;
    p->buffer_0 = (unsigned char*) malloc(p->buffer_len);
    p->buffer_1 = (unsigned char*) malloc(p->buffer_len);

    // set buffers
    p->fec0_src = buffer_switch ? p->buffer_0 : p->buffer_1;
    p->fec0_dst = buffer_switch ? p->buffer_1 : p->buffer_0;
    buffer_switch = !buffer_switch;

    p->intlv0_src = buffer_switch ? p->buffer_0 : p->buffer_1;
    p->intlv0_dst = buffer_switch ? p->buffer_1 : p->buffer_0;
    buffer_switch = !buffer_switch;

    p->fec1_src = buffer_switch ? p->buffer_0 : p->buffer_1;
    p->fec1_dst = buffer_switch ? p->buffer_1 : p->buffer_0;
    buffer_switch = !buffer_switch;

    p->intlv1_src = buffer_switch ? p->buffer_0 : p->buffer_1;
    p->intlv1_dst = buffer_switch ? p->buffer_1 : p->buffer_0;
    buffer_switch = !buffer_switch;

    return p;
}

void packetizer_destroy(packetizer _p)
{
    // free fec objects
    fec_destroy(_p->fec0);
    fec_destroy(_p->fec1);

    // free interleaver objects
    interleaver_destroy(_p->intlv0);
    interleaver_destroy(_p->intlv1);

    // free buffers
    free(_p->buffer_0);
    free(_p->buffer_1);

    // free packetizer object
    free(_p);
}

void packetizer_print(packetizer _p)
{
    printf("packetizer [dec: %u, enc: %u]\n", _p->dec_msg_len, _p->enc_msg_len);
    printf("    crc32           %-16u\n",0); // crc-key
    printf("    fec (outer)     %-16u\n",_p->fec0_enc_msg_len);
    printf("    intlv (outer)   %-16u\n",_p->intlv0_len);
    printf("    fec (inner)     %-16u\n",_p->fec1_enc_msg_len);
    printf("    intlv (inner)   %-16u\n",_p->intlv1_len);
}

unsigned int packetizer_get_packet_length(packetizer _p)
{
    return _p->enc_msg_len;
}

void packetizer_encode(packetizer _p, unsigned char * _msg, unsigned char *_pkt)
{
    // 
    memmove(_p->buffer_0, _msg, _p->dec_msg_len);

    // compute crc32, append to buffer
    _p->crc32_key = crc32_generate_key(_p->buffer_0, _p->dec_msg_len);
    unsigned int crc32_key = _p->crc32_key;
    _p->buffer_0[_p->dec_msg_len+0] = (crc32_key & 0x000000ff) >> 0;
    _p->buffer_0[_p->dec_msg_len+1] = (crc32_key & 0x0000ff00) >> 4;
    _p->buffer_0[_p->dec_msg_len+2] = (crc32_key & 0x00ff0000) >> 8;
    _p->buffer_0[_p->dec_msg_len+3] = (crc32_key & 0xff000000) >> 12;

    fec_encode(_p->fec0, _p->fec0_src, _p->fec0_dst);

    interleaver_interleave(_p->intlv0, _p->intlv0_src, _p->intlv0_dst);

    fec_encode(_p->fec1, _p->fec1_src, _p->fec1_dst);

    interleaver_interleave(_p->intlv1, _p->intlv1_src, _p->intlv1_dst);

    memmove(_pkt, _p->intlv1_dst, _p->enc_msg_len);
}

void packetizer_decode(packetizer _p, unsigned char * _pkt, unsigned char * _msg)
{

    memmove(_p->intlv1_dst, _pkt, _p->enc_msg_len);

    interleaver_deinterleave(_p->intlv1, _p->intlv1_dst, _p->intlv1_src);

    fec_decode(_p->fec1, _p->fec1_dst, _p->fec1_src);

    interleaver_deinterleave(_p->intlv0, _p->intlv0_dst, _p->intlv0_src);

    fec_decode(_p->fec0, _p->fec0_dst, _p->fec1_src);

    // TODO strip crc32, validate

    memmove(_msg, _p->fec1_src, _p->dec_msg_len);
}

