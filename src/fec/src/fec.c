/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
#include <string.h>

#include "liquid.internal.h"

// object-independent methods

const char * fec_scheme_str[LIQUID_NUM_FEC_SCHEMES] = {
    "[unknown]",
    "[none]",
    "[r3] repeat(3)",
    "[h74] hamming(7,4)",
    "[h84] hamming(8,4)",
    "[v27] convolutional r1/2 K=7",
    "[v29] convolutional r1/2 K=9",
    "[v39] convolutional r1/3 K=9",
    "[v615] convolutional r1/6 K=15",
    "[v27p23] convolutional r2/3 K=7 (punctured)",
    "[v27p34] convolutional r3/4 K=7 (punctured)",
    "[v27p45] convolutional r4/5 K=7 (punctured)",
    "[v27p56] convolutional r5/6 K=7 (punctured)",
    "[v27p67] convolutional r6/7 K=7 (punctured)",
    "[v27p78] convolutional r7/8 K=7 (punctured)",
    "[v29p23] convolutional r2/3 K=9 (punctured)",
    "[v29p34] convolutional r3/4 K=9 (punctured)",
    "[v29p45] convolutional r4/5 K=9 (punctured)",
    "[v29p56] convolutional r5/6 K=9 (punctured)",
    "[v29p67] convolutional r6/7 K=9 (punctured)",
    "[v29p78] convolutional r7/8 K=9 (punctured)",
    "[rs8] Reed-solomon"
};

fec_scheme liquid_getopt_str2fec(const char * _str)
{
    if (strcmp(_str,"unknown")==0) {
        return FEC_UNKNOWN;
    } else if (strcmp(_str,"none")==0) {
        return FEC_NONE;
    } else if (strcmp(_str, "v27")==0) {
        return FEC_CONV_V27;
    } else if (strcmp(_str, "v29")==0) {
        return FEC_CONV_V29;
    } else if (strcmp(_str, "v39")==0) {
        return FEC_CONV_V39;
    } else if (strcmp(_str, "v615")==0) {
        return FEC_CONV_V615;
    } else if (strcmp(_str, "v27p23")==0) {
        return FEC_CONV_V27P23;
    } else if (strcmp(_str, "v27p34")==0) {
        return FEC_CONV_V27P34;
    } else if (strcmp(_str, "v27p45")==0) {
        return FEC_CONV_V27P45;
    } else if (strcmp(_str, "v27p56")==0) {
        return FEC_CONV_V27P56;
    } else if (strcmp(_str, "v27p67")==0) {
        return FEC_CONV_V27P67;
    } else if (strcmp(_str, "v27p78")==0) {
        return FEC_CONV_V27P78;
    } else if (strcmp(_str, "v29p23")==0) {
        return FEC_CONV_V29P23;
    } else if (strcmp(_str, "v29p34")==0) {
        return FEC_CONV_V29P34;
    } else if (strcmp(_str, "v29p45")==0) {
        return FEC_CONV_V29P45;
    } else if (strcmp(_str, "v29p56")==0) {
        return FEC_CONV_V29P56;
    } else if (strcmp(_str, "v29p67")==0) {
        return FEC_CONV_V29P67;
    } else if (strcmp(_str, "v29p78")==0) {
        return FEC_CONV_V29P78;

    } else if (strcmp(_str, "rs8")==0) {
        return FEC_RS_P8;
    } else if (strcmp(_str, "r3")==0) {
        return FEC_REP3;
    } else if (strcmp(_str, "h74")==0) {
        return FEC_HAMMING74;
    }
    fprintf(stderr,"warning: liquid_getopt_str2fec(), unknown/unsupported fec scheme : %s\n", _str);
    return FEC_UNKNOWN;
}


unsigned int fec_get_enc_msg_length(fec_scheme _scheme, unsigned int _msg_len)
{
    switch (_scheme) {
    case FEC_UNKNOWN:   return 0;
    case FEC_NONE:      return _msg_len;
    case FEC_REP3:      return 3*_msg_len;
    case FEC_HAMMING74: return 2*_msg_len;
    case FEC_HAMMING84: return 2*_msg_len;

    // convolutional codes
#if HAVE_FEC_H
    case FEC_CONV_V27:  return 2*_msg_len + 2;  // (K-1)/r=12, round up to 2 bytes
    case FEC_CONV_V29:  return 2*_msg_len + 2;  // (K-1)/r=16, 2 bytes
    case FEC_CONV_V39:  return 3*_msg_len + 3;  // (K-1)/r=24, 3 bytes
    case FEC_CONV_V615: return 6*_msg_len + 11; // (K-1)/r=84, round up to 11 bytes
    case FEC_CONV_V27P23:   return fec_conv_get_enc_msg_len(_msg_len,7,2);
    case FEC_CONV_V27P34:   return fec_conv_get_enc_msg_len(_msg_len,7,3);
    case FEC_CONV_V27P45:   return fec_conv_get_enc_msg_len(_msg_len,7,4);
    case FEC_CONV_V27P56:   return fec_conv_get_enc_msg_len(_msg_len,7,5);
    case FEC_CONV_V27P67:   return fec_conv_get_enc_msg_len(_msg_len,7,6);
    case FEC_CONV_V27P78:   return fec_conv_get_enc_msg_len(_msg_len,7,7);

    case FEC_CONV_V29P23:   return fec_conv_get_enc_msg_len(_msg_len,9,2);
    case FEC_CONV_V29P34:   return fec_conv_get_enc_msg_len(_msg_len,9,3);
    case FEC_CONV_V29P45:   return fec_conv_get_enc_msg_len(_msg_len,9,4);
    case FEC_CONV_V29P56:   return fec_conv_get_enc_msg_len(_msg_len,9,5);
    case FEC_CONV_V29P67:   return fec_conv_get_enc_msg_len(_msg_len,9,6);
    case FEC_CONV_V29P78:   return fec_conv_get_enc_msg_len(_msg_len,9,7);
#else
    case FEC_CONV_V27:
    case FEC_CONV_V29:
    case FEC_CONV_V39:
    case FEC_CONV_V615:
    case FEC_CONV_V27P23:
    case FEC_CONV_V27P34:
    case FEC_CONV_V27P45:
    case FEC_CONV_V27P56:
    case FEC_CONV_V27P67:
    case FEC_CONV_V27P78:

    case FEC_CONV_V29P23:
    case FEC_CONV_V29P34:
    case FEC_CONV_V29P45:
    case FEC_CONV_V29P56:
    case FEC_CONV_V29P67:
    case FEC_CONV_V29P78:
        fprintf(stderr, "error: fec_get_enc_msg_length(), convolutional codes unavailable (install libfec)\n");
        exit(-1);
#endif

    // Reed-Solomon codes
#if HAVE_FEC_H
    case FEC_RS_P8:          return 255; // TODO : use subroutine to calculate this length
#else
    case FEC_RS_P8:
        fprintf(stderr, "error: fec_get_enc_msg_length(), Reed-Solomon codes unavailable (install libfec)\n");
        exit(-1);
#endif
    default:
        printf("error: fec_get_enc_msg_length(), unknown/unsupported scheme: %d\n", _scheme);
        exit(-1);
    }

    return 0;
}

// compute encoded message length for convolutional codes
//  _dec_msg_len    :   decoded message length
//  _K              :   constraint length
//  _p              :   puncturing rate, r = _p / (_p+1)
unsigned int fec_conv_get_enc_msg_len(unsigned int _dec_msg_len,
                                      unsigned int _K,
                                      unsigned int _p)
{
    unsigned int num_bits_in = _dec_msg_len*8;
    unsigned int n = num_bits_in + _K - 1;
    unsigned int num_bits_out = n + (n+_p-1)/_p;
    unsigned int num_bytes_out = num_bits_out/8 + (num_bits_out%8 ? 1 : 0);
#if 0
    printf("msg len :       %3u\n", _dec_msg_len);
    printf("num bits in :   %3u\n", num_bits_in);
    printf("n (constraint): %3u\n", n);
    printf("num bits out:   %3u", num_bits_out);
    printf(" = n+(n+p-1)/p = %u+(%u+%u-1)/%u\n", n,n,_p,_p);
    printf("num bytes out:  %3u\n", num_bytes_out);
#endif
    return num_bytes_out;
}


float fec_get_rate(fec_scheme _scheme)
{
    switch (_scheme) {
    case FEC_UNKNOWN:   return 0;
    case FEC_NONE:      return 1.;
    case FEC_REP3:      return 1./3.;
    case FEC_HAMMING74: return 1./2.;
    case FEC_HAMMING84: return 1./2.;

    // convolutional codes
#if HAVE_FEC_H
    case FEC_CONV_V27:  return 1./2.;
    case FEC_CONV_V29:  return 1./2.;
    case FEC_CONV_V39:  return 1./3.;
    case FEC_CONV_V615: return 1./6.;
    case FEC_CONV_V27P23:   return 2./3.;
    case FEC_CONV_V27P34:   return 3./4.;
    case FEC_CONV_V27P45:   return 4./5.;
    case FEC_CONV_V27P56:   return 5./6.;
    case FEC_CONV_V27P67:   return 6./7.;
    case FEC_CONV_V27P78:   return 7./8.;
    case FEC_CONV_V29P23:   return 2./3.;
    case FEC_CONV_V29P34:   return 3./4.;
    case FEC_CONV_V29P45:   return 4./5.;
    case FEC_CONV_V29P56:   return 5./6.;
    case FEC_CONV_V29P67:   return 6./7.;
    case FEC_CONV_V29P78:   return 7./8.;
#else
    case FEC_CONV_V27:
    case FEC_CONV_V29:
    case FEC_CONV_V39:
    case FEC_CONV_V615:
    case FEC_CONV_V27P23:
        fprintf(stderr,"error: fec_get_rate(), convolutional codes unavailable (install libfec)\n");
        exit(-1);
#endif

    // Reed-Solomon codes
#if HAVE_FEC_H
    case FEC_RS_P8:          return 223./255.;
#else
    case FEC_RS_P8:
        fprintf(stderr,"error: fec_get_rate(), Reed-Solomon codes unavailable (install libfec)\n");
        exit(-1);
#endif

    default:
        printf("error: fec_get_rate(), unknown/unsupported scheme: %d\n", _scheme);
        exit(-1);
    }
    return 0;
}

fec fec_create(fec_scheme _scheme, void *_opts)
{
    switch (_scheme) {
    case FEC_UNKNOWN:
        printf("error: fec_create(), cannot create fec object of type \"UNKNOWN\"\n");
        exit(-1);
    case FEC_NONE:
        return fec_pass_create(NULL);
    case FEC_REP3:
        return fec_rep3_create(_opts);
    case FEC_HAMMING74:
        return fec_hamming74_create(_opts);
    case FEC_HAMMING84:
        //return fec_hamming84_create(_opts);
        printf("error: fec_create(), unsupported scheme: fec_hamming84\n");
        exit(-1);

    // convolutional codes
#if HAVE_FEC_H
    case FEC_CONV_V27:
    case FEC_CONV_V29:
    case FEC_CONV_V39:
    case FEC_CONV_V615:
        return fec_conv_create(_scheme);
    case FEC_CONV_V27P23:
    case FEC_CONV_V27P34:
    case FEC_CONV_V27P45:
    case FEC_CONV_V27P56:
    case FEC_CONV_V27P67:
    case FEC_CONV_V27P78:

    case FEC_CONV_V29P23:
    case FEC_CONV_V29P34:
    case FEC_CONV_V29P45:
    case FEC_CONV_V29P56:
    case FEC_CONV_V29P67:
    case FEC_CONV_V29P78:
        return fec_conv_punctured_create(_scheme);
#else
    case FEC_CONV_V27:
    case FEC_CONV_V29:
    case FEC_CONV_V39:
    case FEC_CONV_V615:
    case FEC_CONV_V27P23:
    case FEC_CONV_V27P34:
    case FEC_CONV_V27P45:
    case FEC_CONV_V27P56:
    case FEC_CONV_V27P67:
    case FEC_CONV_V27P78:
        fprintf(stderr,"error: fec_create(), convolutional codes unavailable (install libfec)\n");
        exit(-1);
#endif

    // Reed-Solomon codes
#if HAVE_FEC_H
    case FEC_RS_P8:
        return fec_rs_create(_scheme);
#else
    case FEC_RS_P8:
        fprintf(stderr,"error: fec_create(), Reed-Solomon codes unavailable (install libfec)\n");
        exit(-1);
#endif

    default:
        printf("error: fec_create(), unknown/unsupported scheme: %d\n", _scheme);
        exit(-1);
    }

    // should never get to this point, but return NULL to keep
    // compiler happy
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


