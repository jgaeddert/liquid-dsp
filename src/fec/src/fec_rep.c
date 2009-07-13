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
// FEC, repeat code
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liquid.internal.h"

fec fec_rep3_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = FEC_REP3;
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
    printf("fec_rep3 [r: %3.2f]\n", _q->rate);
}

void fec_rep3_encode(fec _q, unsigned int _dec_msg_len, unsigned char *_msg_dec, unsigned char *_msg_enc)
{
    unsigned int i;
    for (i=0; i<3; i++) {
        memcpy(&_msg_enc[i*_dec_msg_len], _msg_dec, _dec_msg_len);
    }
}

void fec_rep3_decode(fec _q, unsigned int _dec_msg_len, unsigned char *_msg_enc, unsigned char *_msg_dec)
{
    unsigned char s0, s1, s2, a, b, c, x, y;
    unsigned int i, num_errors=0;
    for (i=0; i<_dec_msg_len; i++) {
        s0 = _msg_enc[i];
        s1 = _msg_enc[i +   _dec_msg_len];
        s2 = _msg_enc[i + 2*_dec_msg_len];

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

