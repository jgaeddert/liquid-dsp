//
//
//

#include "sequence.h"

binary_sequence * binary_sequence_create(unsigned int _num_bits)
{
    binary_sequence * bs;

    // allocate memory for binary sequence
    bs = (binary_sequence*) malloc( sizeof(binary_sequence) );

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

void free_binary_sequence(binary_sequence * bs) {
    free( bs->s );
    free( bs );
}

void binary_sequence_print(binary_sequence * bs)
{
    unsigned int i, j;
    unsigned int byte;

    printf("\nbuffer : ");
    for (i=0; i<bs->s_len; i++) {
        byte = (bs->s)[i];
        for (j=0; j<8; j++)
            printf("%d", (byte >> (8-j-1)) & 0x01);
        printf(" ");
    }
    printf("\n");
}

void binary_sequence_push(binary_sequence * bs, unsigned int b)
{
    unsigned int overflow;
    int i;

    // shift first block
    (bs->s)[0] <<= 1;
    (bs->s)[0] &= bs->bit_mask_msb;

    for (i=1; i<bs->s_len; i++) {
        // overflow for i-th block is its MSB
        overflow = ( (bs->s)[i] & 0x90 ) >> 7;

        // shift block 1 bit
        (bs->s)[i] <<= 1;

        // apply overflow to (i-1)-th block's LSB
        (bs->s)[i-1] |= overflow;
    }

    // apply input bit to LSB of last block
    (bs->s)[bs->s_len-1] |= ( b & 0x01 );
}

signed int binary_sequence_correlate(binary_sequence * bs1, binary_sequence * bs2)
{
    signed int rxy = 0;
    unsigned int i;
    
    if ( bs1->s_len != bs2->s_len ) {
        printf("error: binary_sequence_correlate(), binary sequences must be the same length!\n");
        exit(-1);
    }
    
    unsigned int byte;

    for (i=0; i<bs1->s_len; i++) {
        //
        byte = (bs1->s)[i] ^ (bs2->s)[i];
        byte = ~byte;

        rxy += 2*c_ones[byte & 0xFF] - 8;
    }

    // compensate for most-significant block and return
    rxy -= 8 - bs1->num_bits_msb;
    return rxy;
}

void  binary_sequence_add(binary_sequence * bs1, binary_sequence * bs2, binary_sequence * bs3)
{
    // test lengths of all sequences
    if ( bs1->s_len != bs2->s_len ||
         bs1->s_len != bs3->s_len ||
         bs2->s_len != bs3->s_len ) {
        // throw error
        perror("error: binary_sequence_add(), binary sequences must be same length!\n");
        exit(-1);
    }

    // b3 = b1 + b2
    unsigned int i;
    for (i=0; i<bs1->s_len; i++)
        (bs3->s)[i] = (bs1->s)[i] ^ (bs2->s)[i];

    // mask most-significant byte
    //(bs3->s)[bs3->s_len-1] &= (1 << bs3->num_bits_msb) - 1;
}

unsigned int binary_sequence_accumulate(binary_sequence * bs)
{
    unsigned int i;
    unsigned int r=0;

    for (i=0; i<bs->s_len; i++)
        r += c_ones[(bs->s)[i] & 0xFF];

    return r;
}

msequence msequence_default[13] = {
//   m,     g,      a,      n,      v,      b
    {0,     0,      1,      0,      1,      0}, // dummy placeholder
    {0,     0,      1,      0,      1,      0}, // dummy placeholder
    {2,     0x0003, 0x0002, 3,      0x0002, 0},
    {3,     0x0005, 0x0004, 7,      0x0004, 0},
    {4,     0x0009, 0x0008, 15,     0x0008, 0},
    {5,     0x0012, 0x0010, 31,     0x0010, 0},
    {6,     0x0021, 0x0020, 63,     0x0020, 0},
    {7,     0x0044, 0x0040, 127,    0x0040, 0},
    {8,     0x008E, 0x0080, 255,    0x0080, 0},
    {9,     0x0108, 0x0100, 511,    0x0100, 0},
    {10,    0x0204, 0x0200, 1023,   0x0200, 0},
    {11,    0x0402, 0x0400, 2047,   0x0400, 0},
    {12,    0x0829, 0x0800, 4095,   0x0800, 0}
};

void msequence_init(msequence *_ms, unsigned int _m, unsigned int _g, unsigned int _a)
{
    _ms->m = _m;
    _ms->g = _g >> 1;
    // reverse initial state register:
    // 0001 -> 1000
    unsigned int i;
    _ms->a = 0;
    for (i=0; i<_ms->m; i++) {
        _ms->a <<= 1;
        _ms->a |= (_a & 0x01);
        _a >>= 1;
    }

    _ms->n = (1<<_m)-1;
    _ms->v = _ms->a;
    _ms->b = 0;
}

unsigned int msequence_generate_symbol(msequence *_ms, unsigned int _bps)
{
    unsigned int i, s = 0;
    for (i=0; i<_bps; i++) {
        s <<= 1;
        s |= msequence_advance(_ms);
    }
    return s;
}

void binary_sequence_init_msequence(
    binary_sequence* _bs,
    msequence* _ms)
{
    if (_ms->n > SIGPROCC_MAX_MSEQUENCE_LENGTH) {
        perror("error: binary_sequence_init_msequence(), msequence length exceeds maximum\n");
        exit(-1);
    }

    // clear binary sequence
    binary_sequence_clear(_bs);

    unsigned int i;
    for (i=0; i<(_ms->n); i++)
        binary_sequence_push(_bs, msequence_advance(_ms));
}

