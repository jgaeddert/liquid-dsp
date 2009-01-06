//
// Packetizer
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "framing_internal.h"

#define MAX(X,Y) ( ((X)>(Y)) ? (X) : (Y) )

#define PACKETIZER_SET_BUFFER(PTR,SW,B0,B1) \
    (PTR) = (SW) ? (B0) : (B1);             \
    SW = !SW;

packetizer packetizer_create(
    unsigned int _n,
    fec_scheme _fec0,
    fec_scheme _fec1)
{
    packetizer p = (packetizer) malloc(sizeof(struct packetizer_s));

    bool buffer_switch = true;

    // fec (inner)
    p->fec0_scheme = _fec0;
    p->fec0_dec_msg_len = _n + 4;
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

    // allocate memory for buffers
    p->buffer_len = p->intlv1_len;
    p->buffer_0 = (unsigned char*) malloc(p->buffer_len);
    p->buffer_1 = (unsigned char*) malloc(p->buffer_len);

    // set buffers
    PACKETIZER_SET_BUFFER(p->fec0_src, buffer_switch, p->buffer_0, p->buffer_1)
    PACKETIZER_SET_BUFFER(p->fec0_dst, buffer_switch, p->buffer_0, p->buffer_1)

    PACKETIZER_SET_BUFFER(p->intlv0_src, buffer_switch, p->buffer_0, p->buffer_1)
    PACKETIZER_SET_BUFFER(p->intlv0_dst, buffer_switch, p->buffer_0, p->buffer_1)

    PACKETIZER_SET_BUFFER(p->fec1_src, buffer_switch, p->buffer_0, p->buffer_1)
    PACKETIZER_SET_BUFFER(p->fec1_dst, buffer_switch, p->buffer_0, p->buffer_1)

    PACKETIZER_SET_BUFFER(p->intlv1_src, buffer_switch, p->buffer_0, p->buffer_1)
    PACKETIZER_SET_BUFFER(p->intlv1_dst, buffer_switch, p->buffer_0, p->buffer_1)

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
    printf("    fec (outer)     %-16u\n",0);
    printf("    intlv (outer)   %-16u\n",0);
    printf("    fec (inner)     %-16u\n",0);
    printf("    intlv (inner)   %-16u\n",0);
}

unsigned int packetizer_get_packet_length(packetizer _p)
{
    return _p->enc_msg_len;
}

void packetizer_encode(packetizer _p, unsigned char * _msg, unsigned char *_pkt)
{
    // 
    memmove(_p->buffer_0, _msg, _p->dec_msg_len);

    // compute crc
}

void packetizer_decode(packetizer _p, unsigned char * _pkt, unsigned char * _msg)
{

}

