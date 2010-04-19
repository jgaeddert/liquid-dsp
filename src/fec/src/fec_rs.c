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
    case FEC_RS_P8: fec_rs_init_p8(q);   break;
    default:
        printf("error: fec_rs_create(), invalid type\n");
        exit(0);
    }

    // initialize basic parameters
    q->nn = (1 << q->symsize) - 1;
    q->kk = q->nn - q->nroots;

    // Reed-Solomon specific decoding
    q->rs = init_rs_char(q->symsize,
                         q->genpoly,
                         q->fcs,
                         q->prim,
                         q->nroots,
                         0);

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
    // TODO : make length variable
    if (_dec_msg_len != 223) {
        fprintf(stderr,"error: fec_rs_encode(), _dec_msg_len must be 223\n");
        exit(1);
    }

    // copy sequence
    memmove(_msg_enc, _msg_dec, _q->nn*sizeof(unsigned char));

    // encode data, appending parity bits to end of sequence
    encode_rs_char(_q->rs, _msg_enc, &_msg_enc[_q->kk]);
}

//unsigned int
void fec_rs_decode(fec _q,
                   unsigned int _dec_msg_len,
                   unsigned char *_msg_enc,
                   unsigned char *_msg_dec)
{
    // TODO : make length variable
    if (_dec_msg_len != 223) {
        fprintf(stderr,"error: fec_rs_decode(), _dec_msg_len must be 223\n");
        exit(1);
    }

    // set erasures, error locations to zero
    memset(_q->errlocs,  0x00, _q->nn*sizeof(unsigned char));
    memset(_q->derrlocs, 0x00, _q->nn*sizeof(unsigned char));
    _q->erasures = 0;

    // copy sequence to input array
    memmove(_q->tblock, _msg_enc, _q->nn*sizeof(unsigned char));

    // decode block
    int derrors = decode_rs_char(_q->rs, _q->tblock, _q->derrlocs, _q->erasures);

    // copy output
    memmove(_msg_dec, _q->tblock, _dec_msg_len*sizeof(unsigned char));
}

void fec_rs_setlength(fec _q,
                      unsigned int _dec_msg_len)
{
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
    //_q->ntrials = 10;
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

