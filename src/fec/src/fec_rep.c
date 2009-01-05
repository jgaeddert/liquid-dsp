//
// FEC, repeat code
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fec_internal.h"

fec fec_rep3_create(unsigned int _msg_len, void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = FEC_REP3;
    q->dec_msg_len = _msg_len;
    q->enc_msg_len = fec_get_enc_msg_length(q->scheme,_msg_len);
    q->rate = fec_get_rate(q->scheme);

    q->encode_func = &fec_rep3_encode;
    q->decode_func = &fec_rep3_decode;

    return q;
}

void fec_rep3_destroy(fec _q)
{
    free(_q);
}

void fec_rep3_print(fec _q)
{
    printf("fec_rep3 [len: %u, r: %3.2f]\n", _q->dec_msg_len, _q->rate);
}

void fec_rep3_encode(fec _q, unsigned char *_msg_dec, unsigned char *_msg_enc)
{
    unsigned int i;
    for (i=0; i<3; i++) {
        memcpy(&_msg_enc[i*(_q->dec_msg_len)], _msg_dec, _q->dec_msg_len);
    }
}

//unsigned int fec_rep3_decode(fec _q, unsigned char *_msg_enc, unsigned char *_msg_dec)
void fec_rep3_decode(fec _q, unsigned char *_msg_enc, unsigned char *_msg_dec)
{
    unsigned char s0, s1, s2, a, b, c, x, y;
    unsigned int i, num_errors=0;
    for (i=0; i<_q->dec_msg_len; i++) {
        s0 = _msg_enc[i];
        s1 = _msg_enc[i +   (_q->dec_msg_len)];
        s2 = _msg_enc[i + 2*(_q->dec_msg_len)];

        a = s0 ^ s1;
        b = s0 ^ s2;
        c = s1 ^ s2;

        x = a | b | c;
        y = s0 ^ s1 ^ s2;
        num_errors += x ? 1 : 0;

        _msg_dec[i] = x ^ y;
    }
    //return num_errors;
}

