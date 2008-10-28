//
// FEC, repeat code
// 

#include <string.h>
#include "fec_internal.h"

struct fec_rep3_s {
    int x;
};

void fec_rep3_encode(unsigned char *_msg_dec, unsigned int _msg_len, unsigned char *_msg_enc)
{
    unsigned int i;
    for (i=0; i<3; i++) {
        memcpy(&_msg_enc[i*_msg_len], _msg_dec, _msg_len);
    }
}

unsigned int fec_rep3_decode(unsigned char *_msg_enc, unsigned int _msg_len, unsigned char *_msg_dec)
{
    unsigned char s0, s1, s2, a, b, c, x, y;
    unsigned int i, num_errors=0;
    for (i=0; i<_msg_len; i++) {
        s0 = _msg_enc[i];
        s1 = _msg_enc[i + _msg_len];
        s2 = _msg_enc[i + _msg_len*2];

        a = s0 ^ s1;
        b = s0 ^ s2;
        c = s1 ^ s2;

        x = a | b | c;
        y = s0 ^ s1 ^ s2;
        num_errors += x ? 1 : 0;

        _msg_dec[i] = x ^ y;
    }
    return num_errors;
}
