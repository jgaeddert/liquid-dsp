/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// FEC (generic functions)
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// object-independent methods

const char * fec_scheme_str[9] = {
    "unknown",
    "none",
    "repeat(3)",
    "hamming(7,4)",
    "hamming(8,4)",
    "convolutional r1/2 K=7",
    "convolutional r1/2 K=9",
    "convolutional r1/3 K=9",
    "convolutional r1/6 K=16"
};

unsigned int fec_get_enc_msg_length(fec_scheme _scheme, unsigned int _msg_len)
{
    switch (_scheme) {
    case FEC_UNKNOWN:   return 0;
    case FEC_NONE:      return _msg_len;
    case FEC_REP3:      return 3*_msg_len;
    case FEC_HAMMING74: return 2*_msg_len;
    case FEC_HAMMING84: return 2*_msg_len;
#if HAVE_FEC_H
    case FEC_CONV_V27:  return 2*_msg_len + 2;  // (K-1)/r=12, round up to 2 bytes
    case FEC_CONV_V29:  return 2*_msg_len + 2;  // (K-1)/r=16, 2 bytes
    case FEC_CONV_V39:  return 3*_msg_len + 3;  // (K-1)/r=24, 3 bytes
    case FEC_CONV_V615: return 6*_msg_len + 11; // (K-1)/r=84, round up to 11 bytes
#else
    case FEC_CONV_V27:
    case FEC_CONV_V29:
    case FEC_CONV_V39:
    case FEC_CONV_V615:
        printf("error: fec_get_enc_msg_length(), convolutional codes unavailable (install libfec)\n");
        exit(0);
#endif
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
#if HAVE_FEC_H
    case FEC_CONV_V27:  return 1./2.;
    case FEC_CONV_V29:  return 1./2.;
    case FEC_CONV_V39:  return 1./3.;
    case FEC_CONV_V615: return 1./6.;
#else
    case FEC_CONV_V27:
    case FEC_CONV_V29:
    case FEC_CONV_V39:
    case FEC_CONV_V615:
        printf("error: fec_get_rate(), convolutional codes unavailable (install libfec)\n");
        exit(0);
#endif
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
        printf("error: fec_create(), unsupported scheme: fec_hamming84\n");
        exit(0);
#if HAVE_FEC_H
    case FEC_CONV_V27:
        return fec_conv27_create(_opts);
    case FEC_CONV_V29:
        return fec_conv29_create(_opts);
    case FEC_CONV_V39:
        return fec_conv39_create(_opts);
    case FEC_CONV_V615:
        return fec_conv615_create(_opts);
#else
    case FEC_CONV_V27:
    case FEC_CONV_V29:
    case FEC_CONV_V39:
    case FEC_CONV_V615:
        printf("error: fec_create(), convolutional codes unavailable (install libfec)\n");
        exit(0);
#endif
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


