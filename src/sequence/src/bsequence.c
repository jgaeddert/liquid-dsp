//
// Binary sequence (generic)
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "sequence_internal.h"
#include "../../utility/src/utility.h" // for count_ones

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
    div_t d = div( bs->num_bits, sizeof(uint8_t)*8 );
    bs->s_len = d.quot;
    bs->s_len += (d.rem > 0) ? 1 : 0;

    // number of bits in MSB block
    bs->num_bits_msb = (d.rem == 0) ? sizeof(uint8_t)*8 : (unsigned int) d.rem;

    // bit mask for MSB block
    bs->bit_mask_msb = 0xff;
    unsigned int i;
    for (i=0; i<8-bs->num_bits_msb; i++)
        bs->bit_mask_msb >>= 1;

    // initialze array with zeros
    bs->s = (uint8_t*) calloc( bs->s_len, sizeof(uint8_t) );

    return bs;
}

void free_bsequence(bsequence _bs) {
    free( _bs->s );
    free( _bs );
}

void bsequence_clear(bsequence _bs)
{
    memset( _bs->s, 0x00, (_bs->s_len)*sizeof(uint8_t) );
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

