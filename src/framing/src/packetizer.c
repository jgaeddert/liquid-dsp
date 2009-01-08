//
// Packetizer
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "framing_internal.h"

#include "../../fec/src/fec.h"

unsigned int packetizer_get_packet_length(unsigned int _n, int _fec0, int _fec1)
{
    return fec_get_enc_msg_length(_fec1, fec_get_enc_msg_length(_fec0, _n+4));
}

packetizer packetizer_create(
    unsigned int _n,
    int _fec0,
    int _fec1)
{
    packetizer p = (packetizer) malloc(sizeof(struct packetizer_s));

    p->dec_msg_len = _n;

    // set schemes
    p->fec0_scheme = _fec0;
    p->intlv0_scheme = INT_BLOCK;
    p->fec1_scheme = _fec1;
    p->intlv1_scheme = INT_BLOCK;

    // compute plan: lengths of buffers, etc.
    p->fec0_dec_msg_len = p->dec_msg_len + 4;
    p->fec0_enc_msg_len = fec_get_enc_msg_length(p->fec0_scheme, p->fec0_dec_msg_len);

    p->intlv0_len = p->fec0_enc_msg_len;

    p->fec1_dec_msg_len = p->fec0_enc_msg_len;
    p->fec1_enc_msg_len = fec_get_enc_msg_length(p->fec1_scheme, p->fec1_dec_msg_len);

    p->intlv1_len = p->fec1_enc_msg_len;

    // create objects
    p->fec0 = fec_create(p->fec0_scheme, p->fec0_dec_msg_len, NULL);
    p->intlv0 = interleaver_create(p->intlv0_len, p->intlv0_scheme);
    p->fec1 = fec_create(p->fec1_scheme, p->fec1_dec_msg_len, NULL);
    p->intlv1 = interleaver_create(p->intlv1_len, p->intlv0_scheme);

    p->enc_msg_len = p->intlv1_len;

    // allocate memory for buffers
    p->buffer_len = p->enc_msg_len;
    p->buffer_0 = (unsigned char*) malloc(p->buffer_len);
    p->buffer_1 = (unsigned char*) malloc(p->buffer_len);

    packetizer_set_buffers(p);

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
    printf("    crc32           +%-9u %-16s\n",4,"crc32"); // crc-key
    printf("    fec (outer)     %-10u %-16s\n",_p->fec0_enc_msg_len,fec_scheme_str[_p->fec0_scheme]);
    printf("    intlv (outer)   %-10u %-16s\n",_p->intlv0_len,"?");
    printf("    fec (inner)     %-10u %-16s\n",_p->fec1_enc_msg_len,fec_scheme_str[_p->fec1_scheme]);
    printf("    intlv (inner)   %-10u %-16s\n",_p->intlv1_len,"?");
}

void packetizer_encode(packetizer _p, unsigned char * _msg, unsigned char *_pkt)
{
    // 
    memmove(_p->buffer_0, _msg, _p->dec_msg_len);

    // compute crc32, append to buffer
    _p->crc32_key = crc32_generate_key(_p->buffer_0, _p->dec_msg_len);
    unsigned int crc32_key = _p->crc32_key;
    _p->buffer_0[_p->dec_msg_len+0] = (crc32_key & 0x000000ff) >> 0;
    _p->buffer_0[_p->dec_msg_len+1] = (crc32_key & 0x0000ff00) >> 8;
    _p->buffer_0[_p->dec_msg_len+2] = (crc32_key & 0x00ff0000) >> 16;
    _p->buffer_0[_p->dec_msg_len+3] = (crc32_key & 0xff000000) >> 24;

    fec_encode(_p->fec0, _p->fec0_src, _p->fec0_dst);

    interleaver_interleave(_p->intlv0, _p->intlv0_src, _p->intlv0_dst);

    fec_encode(_p->fec1, _p->fec1_src, _p->fec1_dst);

    interleaver_interleave(_p->intlv1, _p->intlv1_src, _p->intlv1_dst);

    memmove(_pkt, _p->intlv1_dst, _p->enc_msg_len);
}

bool packetizer_decode(packetizer _p, unsigned char * _pkt, unsigned char * _msg)
{

    memmove(_p->intlv1_dst, _pkt, _p->enc_msg_len);

    interleaver_deinterleave(_p->intlv1, _p->intlv1_dst, _p->intlv1_src);

    fec_decode(_p->fec1, _p->fec1_dst, _p->fec1_src);

    interleaver_deinterleave(_p->intlv0, _p->intlv0_dst, _p->intlv0_src);

    fec_decode(_p->fec0, _p->fec0_dst, _p->fec0_src);

    // strip crc32, validate message
    unsigned int crc32_key = 0;
    crc32_key |= _p->fec0_src[_p->dec_msg_len+0] << 0;
    crc32_key |= _p->fec0_src[_p->dec_msg_len+1] << 8;
    crc32_key |= _p->fec0_src[_p->dec_msg_len+2] << 16;
    crc32_key |= _p->fec0_src[_p->dec_msg_len+3] << 24;

    memmove(_msg, _p->fec0_src, _p->dec_msg_len);

    return crc32_validate_message(_p->fec0_src, _p->dec_msg_len, crc32_key);
}

void packetizer_set_scheme(packetizer _p, int _fec0, int _fec1)
{
    //
}

// 
// internal methods
//

void packetizer_set_buffers(packetizer _p)
{
    bool buffer_switch = true;

    // fec (outer)
    _p->fec0_src = buffer_switch ? _p->buffer_0 : _p->buffer_1;
    _p->fec0_dst = buffer_switch ? _p->buffer_1 : _p->buffer_0;
    buffer_switch = !buffer_switch;

    // interleaver (outer)
    _p->intlv0_src = buffer_switch ? _p->buffer_0 : _p->buffer_1;
    _p->intlv0_dst = buffer_switch ? _p->buffer_1 : _p->buffer_0;
    buffer_switch = !buffer_switch;

    // fec (inner)
    _p->fec1_src = buffer_switch ? _p->buffer_0 : _p->buffer_1;
    _p->fec1_dst = buffer_switch ? _p->buffer_1 : _p->buffer_0;
    buffer_switch = !buffer_switch;

    // interleaver (inner)
    _p->intlv1_src = buffer_switch ? _p->buffer_0 : _p->buffer_1;
    _p->intlv1_dst = buffer_switch ? _p->buffer_1 : _p->buffer_0;
    buffer_switch = !buffer_switch;
}

void packetizer_realloc_buffers(packetizer _p, unsigned int _len)
{
    _p->buffer_len = _len;
    _p->buffer_0 = (unsigned char*) realloc(_p->buffer_0, _p->buffer_len);
    _p->buffer_1 = (unsigned char*) realloc(_p->buffer_1, _p->buffer_len);
}

