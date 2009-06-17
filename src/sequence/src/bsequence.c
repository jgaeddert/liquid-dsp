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
// Binary sequence (generic)
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

bsequence bsequence_create(unsigned int _num_bits)
{
    bsequence bs;

    // allocate memory for binary sequence
    bs = (bsequence) malloc( sizeof(struct bsequence_s) );

    // initialize variables
    bs->s_len = 0;
    bs->s = NULL;
    bs->num_bits = 0;

    //
    bs->num_bits = _num_bits;
    
    // initialize array length
    div_t d = div( bs->num_bits, sizeof(unsigned char)*8 );
    bs->s_len = d.quot;
    bs->s_len += (d.rem > 0) ? 1 : 0;

    // number of bits in MSB block
    bs->num_bits_msb = (d.rem == 0) ? sizeof(unsigned char)*8 : (unsigned int) d.rem;

    // bit mask for MSB block
    bs->bit_mask_msb = 0xff;
    unsigned int i;
    for (i=0; i<8-bs->num_bits_msb; i++)
        bs->bit_mask_msb >>= 1;

    // initialze array with zeros
    bs->s = (unsigned char*) calloc( bs->s_len, sizeof(unsigned char) );

    return bs;
}

void bsequence_destroy(bsequence _bs) {
    free( _bs->s );
    free( _bs );
}

void bsequence_clear(bsequence _bs)
{
    memset( _bs->s, 0x00, (_bs->s_len)*sizeof(unsigned char) );
}

void bsequence_print(bsequence _bs)
{
    unsigned int i, j;
    unsigned int byte;

    printf("\nbuffer : ");
    for (i=0; i<_bs->s_len; i++) {
        byte = (_bs->s)[i];
        for (j=0; j<8; j++)
            printf("%d", (byte >> (8-j-1)) & 0x01);
        printf(" ");
    }
    printf("\n");
}

void bsequence_push(bsequence _bs, unsigned int b)
{
    unsigned int overflow;
    int i;

    // shift first block
    (_bs->s)[0] <<= 1;
    (_bs->s)[0] &= _bs->bit_mask_msb;

    for (i=1; i<_bs->s_len; i++) {
        // overflow for i-th block is its MSB
        overflow = ( (_bs->s)[i] & 0x90 ) >> 7;

        // shift block 1 bit
        (_bs->s)[i] <<= 1;

        // apply overflow to (i-1)-th block's LSB
        (_bs->s)[i-1] |= overflow;
    }

    // apply input bit to LSB of last block
    (_bs->s)[_bs->s_len-1] |= ( b & 0x01 );
}

signed int bsequence_correlate(bsequence _bs1, bsequence _bs2)
{
    signed int rxy = 0;
    unsigned int i;
    
    if ( _bs1->s_len != _bs2->s_len ) {
        printf("error: bsequence_correlate(), binary sequences must be the same length!\n");
        exit(-1);
    }
    
    unsigned int byte;

    for (i=0; i<_bs1->s_len; i++) {
        //
        byte = (_bs1->s)[i] ^ (_bs2->s)[i];
        byte = ~byte;

        rxy += 2*c_ones[byte & 0xFF] - 8;
    }

    // compensate for most-significant block and return
    rxy -= 8 - _bs1->num_bits_msb;
    return rxy;
}

void  bsequence_add(bsequence _bs1, bsequence _bs2, bsequence _bs3)
{
    // test lengths of all sequences
    if ( _bs1->s_len != _bs2->s_len ||
         _bs1->s_len != _bs3->s_len ||
         _bs2->s_len != _bs3->s_len ) {
        // throw error
        perror("error: bsequence_add(), binary sequences must be same length!\n");
        exit(-1);
    }

    // b3 = b1 + b2
    unsigned int i;
    for (i=0; i<_bs1->s_len; i++)
        (_bs3->s)[i] = (_bs1->s)[i] ^ (_bs2->s)[i];

    // mask most-significant byte
    //(_bs3->s)[_bs3->s_len-1] &= (1 << _bs3->num_bits_msb) - 1;
}

unsigned int bsequence_accumulate(bsequence _bs)
{
    unsigned int i;
    unsigned int r=0;

    for (i=0; i<_bs->s_len; i++)
        r += c_ones[(_bs->s)[i] & 0xFF];

    return r;
}

unsigned int bsequence_get_length(bsequence _bs)
{
    return _bs->num_bits;
}

void bsequence_create_ccodes(bsequence _a, bsequence _b)
{
    // make sure sequences are the same length
    if (_a->num_bits != _b->num_bits) {
        printf("error: bsequence_create_ccodes(), sequence lengths must match\n");
        exit(1);
    } else if (_a->num_bits < 8) {
        printf("error: bsequence_create_ccodes(), sequence too short\n");
        exit(1);
    } else if ( (_a->num_bits)%8 != 0 ) {
        printf("error: bsequence_create_ccodes(), sequence must be multiple of 8\n");
        exit(1);
    }

    bsequence_clear(_a);
    bsequence_clear(_b);

    _a->s[ _a->s_len - 1 ] = 0xb8;
    _b->s[ _b->s_len - 1 ] = 0xb7;

    unsigned int i, n=1;
    unsigned int i_n1, i_n0, s_len = _a->s_len;
    while (8*n < _a->num_bits) {

        i_n1 = s_len - n;
        i_n0 = s_len - 2*n;

        // a -> [a  b]
        // b -> [a ~b]
        memmove(&(_a->s[i_n0]), &(_a->s[i_n1]), n*sizeof(unsigned char));
        memmove(&(_b->s[i_n0]), &(_a->s[i_n1]), n*sizeof(unsigned char));

        memmove(&(_a->s[i_n1]), &(_b->s[i_n1]), n*sizeof(unsigned char));
        memmove(&(_b->s[i_n1]), &(_b->s[i_n1]), n*sizeof(unsigned char));

        // complement lower half
        for (i=0; i<n; i++)
            _b->s[s_len-i-1] ^= 0xff;

        n += n;
    }
}
