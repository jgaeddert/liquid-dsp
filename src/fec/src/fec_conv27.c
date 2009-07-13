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
// 1/2-rate K=7 convolutional code
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

#if HAVE_FEC_H  // (config.h)

fec fec_conv27_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = FEC_CONV_V27;
    q->rate = fec_get_rate(q->scheme);

    q->encode_func = &fec_conv27_encode;
    q->decode_func = &fec_conv27_decode;

    return q;
}

void fec_conv27_destroy(fec _q)
{
    free(_q);
}

void fec_conv27_encode(fec _q,
                       unsigned int _dec_msg_len,
                       unsigned char *_msg_dec,
                       unsigned char *_msg_enc)
{
    unsigned int i,j,sr=0;
    unsigned char byte_in, byte_out, bit;
    for (i=0; i<_dec_msg_len; i++) {
        byte_in  = _msg_dec[i];
        byte_out = 0;
        for (j=0; j<8; j++) {
            bit = (byte_in >> (7-j)) & 0x01;

        }
    }
}

//unsigned int
void fec_conv27_decode(fec _q,
                       unsigned int _dec_msg_len,
                       unsigned char *_msg_enc,
                       unsigned char *_msg_dec)
{
}



#else   // HAVE_FEC_H (config.h)

fec fec_conv27_create(void * _opts)
{
    return NULL;
}

void fec_conv27_destroy(fec _q)
{
}

void fec_conv27_encode(fec _q, unsigned int _dec_msg_len, unsigned char *_msg_dec, unsigned char *_msg_enc)
{
}

//unsigned int
void fec_conv27_decode(fec _q, unsigned int _dec_msg_len, unsigned char *_msg_enc, unsigned char *_msg_dec)
{
}

#endif  // HAVE_FEC_H (config.h)

