//
// 1/2-rate (8,4) Hamming code
//

#include <stdio.h>

#include "fec_internal.h"
#include "../../utility/src/utility.h"

#define HAMMING74_H0    0x55
#define HAMMING74_H1    0x33
#define HAMMING74_H2    0x0f

static unsigned char hamming74_enc[] = {
    0x00,   0x69,   0x2a,   0x43,
    0x4c,   0x25,   0x66,   0x0f,
    0x70,   0x19,   0x5a,   0x33,
    0x3c,   0x55,   0x16,   0x7f
};

static unsigned char hamming74_bflip[] = {
    0x00,   // 0
    0x40,   // 1
    0x20,   // 
    0x10,   // 
    0x08,   // 
    0x04,   // 
    0x02,   // 
    0x01,   // 
};

void fec_hamming74_encode(unsigned char *_msg_dec, unsigned int _msg_len, unsigned char *_msg_enc)
{
    unsigned int i, j=0;
    unsigned char s0, s1;
    for (i=0; i<_msg_len; i++) {
        s0 = (_msg_dec[i] >> 4) & 0x0f;
        s1 = (_msg_dec[i] >> 0) & 0x0f;
        _msg_enc[j+0] = hamming74_enc[s0];
        _msg_enc[j+1] = hamming74_enc[s1];
        j+=2;
    }
}

unsigned int
fec_hamming74_decode(unsigned char *_msg_enc, unsigned int _msg_len, unsigned char *_msg_dec)
{
    unsigned int i, j=0, num_errors=0;
    unsigned char r0, r1, z0, z1, s0, s1;
    for (i=0; i<_msg_len; i++) {
        r0 = _msg_enc[2*i+0];
        r1 = _msg_enc[2*i+1];

        printf("%u :\n", i);

        // compute syndromes
        z0 = fec_hamming74_compute_syndrome(r0);
        z1 = fec_hamming74_compute_syndrome(r1);

        //if (z0 || z0)
        printf("  syndrome[%u]          : %d, %d\n", i, (int)z0, (int)z1);
        printf("  input symbols[%u]     : 0x%.2x, 0x%.2x\n", i, r0, r1);

        if (z0) r0 ^= hamming74_bflip[z0];
        if (z1) r1 ^= hamming74_bflip[z1];

        printf("  corrected symbols[%u] : 0x%.2x, 0x%.2x\n", i, r0, r1);

        s0 = fec_hamming74_decode_symbol(r0);
        s1 = fec_hamming74_decode_symbol(r1);

        printf("  decoded symbols[%u]   : 0x%.1x%.1x\n", i, s0, s1);

        _msg_dec[i] = (s0 << 4) | s1;

        j += 2;
    }
    return 0;
}

// internal

#define bdotprod(x,y) (((c_ones[(x)&(y)]&0xff) % 2) & 0x01)
unsigned char fec_hamming74_compute_syndrome(unsigned char _r)
{
    printf("  r : 0x%.2X\n", (int)_r);
    return
        (bdotprod(_r,HAMMING74_H0) << 0) |
        (bdotprod(_r,HAMMING74_H1) << 1) |
        (bdotprod(_r,HAMMING74_H2) << 2);
}

unsigned char fec_hamming74_decode_symbol(unsigned char _s)
{
    return ((0x10 & _s) >> 1) | ((0x07 & _s) >> 0);
}
