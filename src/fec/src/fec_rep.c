//
// FEC, repeat code
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fec_internal.h"

struct fec_rep3_s {
    unsigned int msg_dec_len;   // decoded message length
    unsigned int msg_enc_len;   // encoded message length
    float rate;
};

unsigned int fec_rep3_get_enc_msg_length(unsigned int _msg_len)
{
    return 3*_msg_len;
}

float fec_rep3_get_rate()
{
    return 1/3.0f;
}

fec_rep3 fec_rep3_create(unsigned int _msg_len, void * _opts)
{
    fec_rep3 q = (fec_rep3) malloc(sizeof(struct fec_rep3_s));
    q->msg_dec_len = _msg_len;
    q->msg_enc_len = fec_rep3_get_enc_msg_length(_msg_len);
    q->rate = fec_rep3_get_rate();
    return q;
}

void fec_rep3_destroy(fec_rep3 _q)
{
    free(_q);
}

void fec_rep3_print(fec_rep3 _q)
{
    printf("fec_rep3 [len: %u, r: %3.2f]\n", _q->msg_dec_len, _q->rate);
}

void fec_rep3_encode(fec_rep3 _q, unsigned char *_msg_dec, unsigned char *_msg_enc)
{
    unsigned int i;
    for (i=0; i<3; i++) {
        memcpy(&_msg_enc[i*(_q->msg_dec_len)], _msg_dec, _q->msg_dec_len);
    }
}

//unsigned int fec_rep3_decode(fec_rep3 _q, unsigned char *_msg_enc, unsigned char *_msg_dec)
void fec_rep3_decode(fec_rep3 _q, unsigned char *_msg_enc, unsigned char *_msg_dec)
{
    unsigned char s0, s1, s2, a, b, c, x, y;
    unsigned int i, num_errors=0;
    for (i=0; i<_q->msg_dec_len; i++) {
        s0 = _msg_enc[i];
        s1 = _msg_enc[i +   (_q->msg_dec_len)];
        s2 = _msg_enc[i + 2*(_q->msg_dec_len)];

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

