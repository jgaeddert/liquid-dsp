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
// punctured convolutional code (macros)
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define VERBOSE_FEC_CONV    1

#define FEC_CONV_ERASURE    127

#if HAVE_FEC_H  // (config.h)
#include "fec.h"

fec FEC_CONV(_create)(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = FEC_CONV(_mode);
    q->rate = fec_get_rate(q->scheme);

    q->encode_func = &FEC_CONV(_encode);
    q->decode_func = &FEC_CONV(_decode);

    // convolutional-specific decoding
    q->num_dec_bytes = 0;
    q->enc_bits = NULL;
    q->vp = NULL;

    return q;
}

void FEC_CONV(_destroy)(fec _q)
{
    // delete viterbi decoder
    if (_q->vp != NULL)
        delete_viterbi(_q->vp);

    free(_q);
}

void FEC_CONV(_encode)(fec _q,
                       unsigned int _dec_msg_len,
                       unsigned char *_msg_dec,
                       unsigned char *_msg_enc)
{
    unsigned int i,j,r; // bookkeeping
    unsigned int sr=0;  // convolutional shift register
    unsigned int n=0;   // output bit counter
    unsigned int p=0;   // puncturing matrix column index

    unsigned char bit;
    unsigned char byte_in;
    unsigned char byte_out=0;

    for (i=0; i<_dec_msg_len; i++) {
        byte_in = _msg_dec[i];

        // break byte into individual bits
        for (j=0; j<8; j++) {
            // shift bit starting with most significant
            bit = (byte_in >> (7-j)) & 0x01;
            sr = (sr << 1) | bit;

            // compute parity bits for each polynomial
            for (r=0; r<FEC_CONV(_R); r++) {
                // enable output determined by puncturing matrix
                if (FEC_CONV(_puncturing_matrix)[p][r]) {
                    byte_out = (byte_out<<1) | parity(sr & FEC_CONV(_poly)[r]);
                    _msg_enc[n/8] = byte_out;
                    n++;
                } else {
                }
            }

            // update puncturing matrix column index
            p = (p+1) % FEC_CONV(_P);
        }
    }

    // tail bits
    for (i=0; i<FEC_CONV(_K)-1; i++) {
        // shift register: push zeros
        sr = (sr << 1);

        // compute parity bits for each polynomial
        for (r=0; r<FEC_CONV(_R); r++) {
            if (FEC_CONV(_puncturing_matrix)[p][r]) {
                byte_out = (byte_out<<1) | parity(sr & FEC_CONV(_poly)[r]);
                _msg_enc[n/8] = byte_out;
                n++;
            }
        }

        // update puncturing matrix column index
        p = (p+1) % FEC_CONV(_P);
    }

    // ensure even number of bytes
    while (n%8) {
        // shift zeros
        byte_out <<= 1;
        _msg_enc[n/8] = byte_out;
        n++;
    }

    assert(n == 8*fec_get_enc_msg_length(FEC_CONV(_mode),_dec_msg_len));
}

//unsigned int
void FEC_CONV(_decode)(fec _q,
                       unsigned int _dec_msg_len,
                       unsigned char *_msg_enc,
                       unsigned char *_msg_dec)
{
    // re-allocate resources if necessary
    FEC_CONV(_setlength)(_q, _dec_msg_len);

    // unpack bytes, adding erasures at punctured indices
    unsigned int num_dec_bits = _q->num_dec_bytes * 8 + FEC_CONV(_K) - 1;
    unsigned int num_enc_bits = num_dec_bits * FEC_CONV(_R);
    unsigned int i,r;
    unsigned int n=0;   // input byte index
    unsigned int k=0;   // intput bit index (0<=k<8)
    unsigned int p=0;   // puncturing matrix column index
    unsigned char bit;
    unsigned char byte_in = _msg_enc[n];
    for (i=0; i<num_enc_bits; i+=FEC_CONV(_R)) {
        //
        for (r=0; r<FEC_CONV(_R); r++) {
            if (FEC_CONV(_puncturing_matrix)[p][r]) {
                // push bit from input
                bit = (byte_in >> (7-k)) & 0x01;
                _q->enc_bits[i+r] = bit ? 255 : 0;
                k++;
                if (k==8) {
                    k = 0;
                    n++;
                    byte_in = _msg_enc[n];
                }
            } else {
                // push erasure
                _q->enc_bits[i+r] = 127; //FEC_CONV_ERASURE;
            }
        }
        p = (p+1)%FEC_CONV(_P);
    }

#if VERBOSE_FEC_CONV
    unsigned int ii;
    printf("msg encoded (bits):\n");
    for (ii=0; ii<num_enc_bits; ii++) {
        printf("%3u ", _q->enc_bits[ii]);
        if (((ii+1)%8)==0)
            printf("\n");
    }
    printf("\n");
#endif

    // run decoder
    init_viterbi(_q->vp,0);
    update_viterbi_blk(_q->vp, _q->enc_bits, 8*_q->num_dec_bytes+FEC_CONV(_K)-1);
    chainback_viterbi(_q->vp, _msg_dec, 8*_q->num_dec_bytes, 0);

#if VERBOSE_FEC_CONV
    for (ii=0; ii<_dec_msg_len; ii++)
        printf("%.2x ", _msg_dec[ii]);
    printf("\n");
#endif
}

void FEC_CONV(_setlength)(fec _q, unsigned int _dec_msg_len)
{
    // re-allocate resources as necessary
    unsigned int num_dec_bytes = _dec_msg_len;

    // return if length has not changed
    if (num_dec_bytes == _q->num_dec_bytes)
        return;

    // reset number of framebits
    _q->num_dec_bytes = num_dec_bytes;
    _q->num_enc_bytes = fec_get_enc_msg_length(FEC_CONV(_mode),
                                               _dec_msg_len);

    // puncturing: need to expand to full length (decoder
    //             injects erasures at punctured values)
    unsigned int num_dec_bits = 8*_q->num_dec_bytes;
    unsigned int n = num_dec_bits + FEC_CONV(_K) - 1;
    unsigned int num_enc_bits = n*FEC_CONV(_R);
#if VERBOSE_FEC_CONV
    printf("(re)creating viterbi decoder, %u frame bytes\n", num_dec_bytes);
    printf("  num decoded bytes         :   %u\n", _q->num_dec_bytes);
    printf("  num encoded bytes         :   %u\n", _q->num_enc_bytes);
    printf("  num decoded bits          :   %u\n", num_dec_bits);
    printf("  num decoded bits (padded) :   %u\n", n);
    printf("  num encoded bits (full)   :   %u\n", num_enc_bits);
#endif

    // delete old decoder if necessary
    if (_q->vp != NULL)
        delete_viterbi(_q->vp);

    // re-create / re-allocate memory buffers
    _q->vp = create_viterbi(8*_q->num_dec_bytes);
    _q->enc_bits = (unsigned char*) realloc(_q->enc_bits,
                                            num_enc_bits*sizeof(unsigned char));
}



#else   // HAVE_FEC_H (config.h)

fec FEC_CONV(_create)(void * _opts)
{
    return NULL;
}

void FEC_CONV(_destroy)(fec _q)
{
}

void FEC_CONV(_encode)(fec _q,
                       unsigned int _dec_msg_len,
                       unsigned char *_msg_dec,
                       unsigned char *_msg_enc)
{
}

//unsigned int
void FEC_CONV(_decode)(fec _q,
                       unsigned int _dec_msg_len,
                       unsigned char *_msg_enc,
                       unsigned char *_msg_dec)
{
}

#endif  // HAVE_FEC_H (config.h)

