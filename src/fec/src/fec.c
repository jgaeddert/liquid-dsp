// 
// FEC (generic functions)
//

#include <stdio.h>
#include <stdlib.h>

#include "fec_internal.h"

// object-independent methods

const char * fec_scheme_str[5] = {
    "unknown",
    "none",
    "repeat(3)",
    "hamming(7,4)",
    "hamming(8,4)"
};

unsigned int fec_get_enc_msg_length(fec_scheme _scheme, unsigned int _msg_len)
{
    switch (_scheme) {
    case FEC_UNKNOWN:   return 0;
    case FEC_NONE:      return _msg_len;
    case FEC_REP3:      return 3*_msg_len;
    case FEC_HAMMING74: return 2*_msg_len;
    case FEC_HAMMING84: return 2*_msg_len;
    default:
        printf("error: fec_get_enc_msg_length(), unknown/unsupported scheme: %d\n", _scheme);
        exit(0);
    }
    return 0;
}

float fec_get_rate(fec_scheme _scheme)
{
    switch (_scheme) {
    case FEC_UNKNOWN:   return 0;
    case FEC_NONE:      return 1.;
    case FEC_REP3:      return 1./3.;
    case FEC_HAMMING74: return 1./2.;
    case FEC_HAMMING84: return 1./2.;
    default:
        printf("error: fec_get_rate(), unknown/unsupported scheme: %d\n", _scheme);
        exit(0);
    }
    return 0;
}

fec fec_create(fec_scheme _scheme, void *_opts)
{
    switch (_scheme) {
    case FEC_UNKNOWN:
        return NULL;
    case FEC_NONE:
        return fec_pass_create(NULL);
    case FEC_REP3:
        return fec_rep3_create(_opts);
    case FEC_HAMMING74:
        return fec_hamming74_create(_opts);
    case FEC_HAMMING84:
        //return fec_hamming84_create(_opts);
    default:
        printf("error: fec_create(), unknown/unsupported scheme: %d\n", _scheme);
        exit(0);
    }
    return NULL;
}

void fec_destroy(fec _q)
{
    free(_q);
}

void fec_print(fec _q)
{
    printf("fec: %s [rate: %4.3f]\n",
        fec_scheme_str[_q->scheme],
        _q->rate);
}

void fec_encode(fec _q, unsigned int _dec_msg_len, unsigned char * _msg_dec, unsigned char * _msg_enc)
{
    _q->encode_func(_q, _dec_msg_len, _msg_dec, _msg_enc);
}

void fec_decode(fec _q, unsigned int _dec_msg_len, unsigned char * _msg_enc, unsigned char * _msg_dec)
{
    _q->decode_func(_q, _dec_msg_len, _msg_enc, _msg_dec);
}


