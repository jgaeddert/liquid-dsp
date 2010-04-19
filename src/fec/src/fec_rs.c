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
// Reed-Solomon (macros)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "liquid.internal.h"

#define VERBOSE_FEC_RS    0

#if HAVE_FEC_H == 1 // (config.h)
#include "fec.h"

fec fec_rs_create(fec_scheme _fs)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = _fs;
    q->rate = fec_get_rate(q->scheme);

    q->encode_func = &fec_rs_encode;
    q->decode_func = &fec_rs_decode;

    switch (q->scheme) {
    case FEC_RS_M8: fec_rs_init_p8(q);   break;
    default:
        printf("error: fec_rs_create(), invalid type\n");
        exit(0);
    }

    // initialize basic parameters
    q->nn = (1 << q->symsize) - 1;
    q->kk = q->nn - q->nroots;

    // lengths
    q->num_dec_bytes = 0;
    q->rs = NULL;

    // allocate memory for arrays
    q->tblock   = (unsigned char*) malloc(q->nn*sizeof(unsigned char));
    q->errlocs  = (int *) malloc(q->nn*sizeof(int));
    q->derrlocs = (int *) malloc(q->nn*sizeof(int));

    return q;
}

void fec_rs_destroy(fec _q)
{
    // delete internal Reed-Solomon decoder object
    free_rs_char(_q->rs);

    // delete internal memory arrays
    free(_q->tblock);
    free(_q->errlocs);
    free(_q->derrlocs);

    // delete fec object
    free(_q);
}

void fec_rs_encode(fec _q,
                   unsigned int _dec_msg_len,
                   unsigned char *_msg_dec,
                   unsigned char *_msg_enc)
{
    // validate input
    if (_dec_msg_len == 0) {
        fprintf(stderr,"error: fec_rs_encode(), input lenght must be > 0\n");
        exit(1);
    }

    // re-allocate resources if necessary
    fec_rs_setlength(_q, _dec_msg_len);

    unsigned int i;
    unsigned int n0=0;  // input index
    unsigned int n1=0;  // output index
    unsigned int block_size=0;
    for (i=0; i<_q->num_blocks; i++) {
        if (i < _q->num_blocks-1)
            block_size = _q->dec_block_len;
        else
            block_size = _q->dec_block_len - _q->res_block_len;

        // copy sequence
        memmove(_q->tblock, &_msg_dec[n0], block_size*sizeof(unsigned char));

        // TODO : pad end with zeros (if necessary)

        // encode data, appending parity bits to end of sequence
        encode_rs_char(_q->rs, _q->tblock, &_q->tblock[_q->dec_block_len]);

        // copy result to output
        memmove(&_msg_enc[n1], _q->tblock, _q->enc_block_len*sizeof(unsigned char));

        // increment counters
        n0 += _q->dec_block_len;
        n1 += _q->enc_block_len;
    }
}

//unsigned int
void fec_rs_decode(fec _q,
                   unsigned int _dec_msg_len,
                   unsigned char *_msg_enc,
                   unsigned char *_msg_dec)
{
    // validate input
    if (_dec_msg_len == 0) {
        fprintf(stderr,"error: fec_rs_encode(), input lenght must be > 0\n");
        exit(1);
    }

    // re-allocate resources if necessary
    fec_rs_setlength(_q, _dec_msg_len);

    // set erasures, error locations to zero
    memset(_q->errlocs,  0x00, _q->nn*sizeof(unsigned char));
    memset(_q->derrlocs, 0x00, _q->nn*sizeof(unsigned char));
    _q->erasures = 0;

    unsigned int i;
    unsigned int n0=0;
    unsigned int n1=0;
    unsigned int block_size=0;
    for (i=0; i<_q->num_blocks; i++) {
        if (i < _q->num_blocks-1)
            block_size = _q->dec_block_len;
        else
            block_size = _q->dec_block_len - _q->res_block_len;

        // copy sequence
        memmove(_q->tblock, &_msg_enc[n0], _q->enc_block_len*sizeof(unsigned char));

        // decode block
        int derrors = decode_rs_char(_q->rs,
                                     _q->tblock,
                                     _q->derrlocs,
                                     _q->erasures);

        // copy result
        memmove(&_msg_dec[n1], _q->tblock, block_size*sizeof(unsigned char));

        // increment counters
        n0 += _q->enc_block_len;
        n1 += _q->dec_block_len;
    }
}

// set dec_msg_len, re-allocating resources as necessary
void fec_rs_setlength(fec _q, unsigned int _dec_msg_len)
{
    // return if length has not changed
    if (_dec_msg_len == _q->num_dec_bytes)
        return;

    // reset lengths
    _q->num_dec_bytes = _dec_msg_len;

    // example : if we are using the 8-bit code,
    //      nroots  = 32
    //      nn      = 255
    //      kk      = 223
    // Let _dec_msg_len = 1024, then
    //      num_blocks = ceil(1024/223)
    //                 = ceil(4.5919)
    //                 = 5
    //      dec_block_len = ceil(1024/num_blocks)
    //                    = ceil(204.8)
    //                    = 205
    //      enc_block_len = dec_block_len + nroots
    //                    = 237
    //      res_block_len = mod(num_blocks*dec_block_len,_dec_msg_len)
    //                    = mod(5*205,1024)
    //                    = mod(1025,1024)
    //                    = 1
    //      pad = kk - dec_block_len
    //          = 223 - 205
    //          = 18
    div_t d;
    d = div(_q->num_dec_bytes, _q->kk);
    _q->num_blocks = d.quot + (d.rem==0 ? 0 : 1);
    d = div(_dec_msg_len, _q->num_blocks);
    _q->dec_block_len = d.quot + (d.rem == 0 ? 0 : 1);
    _q->enc_block_len = _q->dec_block_len + _q->nroots;
    _q->res_block_len = (_q->num_blocks*_q->dec_block_len) % _q->num_dec_bytes;
    _q->pad = _q->kk - _q->dec_block_len;

    _q->num_enc_bytes = _q->enc_block_len * _q->num_blocks;
    
#if VERBOSE_FEC_RS
    printf("dec_msg_len     :   %u\n", _q->num_dec_bytes);
    printf("num_blocks      :   %u\n", _q->num_blocks);
    printf("dec_block_len   :   %u\n", _q->dec_block_len);
    printf("enc_block_len   :   %u\n", _q->enc_block_len);
    printf("res_block_len   :   %u\n", _q->res_block_len);
    printf("pad             :   %u\n", _q->pad);
    printf("enc_msg_len     :   %u\n", _q->num_enc_bytes);
#endif

    // delete old decoder if necessary
    if (_q->rs != NULL)
        free_rs_char(_q->rs);

    // Reed-Solomon specific decoding
    _q->rs = init_rs_char(_q->symsize,
                          _q->genpoly,
                          _q->fcs,
                          _q->prim,
                          _q->nroots,
                          _q->pad);
}

// 
// internal
//

void fec_rs_init_p8(fec _q)
{
    _q->symsize = 8;
    _q->genpoly = 0x11d;
    _q->fcs = 1;
    _q->prim = 1;
    _q->nroots = 32;
}

#else   // HAVE_FEC_H (config.h)

fec fec_rs_create(fec_scheme _fs)
{
    return NULL;
}

void fec_rs_destroy(fec _q)
{
}

void fec_rs_encode(fec _q,
                   unsigned int _dec_msg_len,
                   unsigned char *_msg_dec,
                   unsigned char *_msg_enc)
{
}

//unsigned int
void fec_rs_decode(fec _q,
                   unsigned int _dec_msg_len,
                   unsigned char *_msg_enc,
                   unsigned char *_msg_dec)
{
}

#endif  // HAVE_FEC_H (config.h)

