//
// FEC, none/pass
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fec_internal.h"

fec fec_pass_create(unsigned int _msg_len, void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = FEC_NONE;
    q->dec_msg_len = _msg_len;
    q->enc_msg_len = fec_get_enc_msg_length(q->scheme,_msg_len);
    q->rate = fec_get_rate(q->scheme);

    q->encode_func = &fec_pass_encode;
    q->decode_func = &fec_pass_decode;

    return q;
}

void fec_pass_destroy(fec _q)
{
    free(_q);
}

void fec_pass_print(fec _q)
{
    printf("fec_pass [len: %u, r: %3.2f]\n", _q->dec_msg_len, _q->rate);
}

void fec_pass_encode(fec _q, unsigned char *_msg_dec, unsigned char *_msg_enc)
{
    memmove(_msg_enc, _msg_dec, _q->dec_msg_len);
}

//unsigned int fec_pass_decode(fec _q, unsigned char *_msg_enc, unsigned char *_msg_dec)
void fec_pass_decode(fec _q, unsigned char *_msg_enc, unsigned char *_msg_dec)
{
    memmove(_msg_dec, _msg_enc, _q->enc_msg_len);
}

