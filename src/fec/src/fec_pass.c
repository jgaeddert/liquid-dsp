/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// FEC, none/pass
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liquid.internal.h"

fec fec_pass_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = LIQUID_FEC_NONE;
    q->rate = fec_get_rate(q->scheme);

    q->encode_func      = &fec_pass_encode;
    q->decode_func      = &fec_pass_decode;
    q->decode_soft_func = NULL;

    return q;
}

void fec_pass_destroy(fec _q)
{
    free(_q);
}

void fec_pass_print(fec _q)
{
    printf("fec_pass [r: %3.2f]\n", _q->rate);
}

void fec_pass_encode(fec _q, unsigned int _dec_msg_len, unsigned char *_msg_dec, unsigned char *_msg_enc)
{
    memmove(_msg_enc, _msg_dec, _dec_msg_len);
}

void fec_pass_decode(fec _q, unsigned int _dec_msg_len, unsigned char *_msg_enc, unsigned char *_msg_dec)
{
    memmove(_msg_dec, _msg_enc, _dec_msg_len);
}

