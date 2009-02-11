//
// binary sequence
//

#ifndef __LIQUID_SEQUENCE_INTERNAL_H__
#define __LIQUID_SEQUENCE_INTERNAL_H__

#include "liquid.internal.h"

struct bsequence_s {
    unsigned char * s;          // sequence array, memory pointer
    unsigned int num_bits;      // number of bits in sequence
    unsigned int num_bits_msb;  // number of bits in most-significant block
    unsigned char bit_mask_msb; // bit mask for most-significant block
    unsigned int s_len;         // length of array, number of allocated blocks
};


struct msequence_s {
    unsigned int m;     // length generator polynomial, shift register
    unsigned int g;     // generator polynomial
    unsigned int a;     // initial shift register state, default: 1

    unsigned int n;     // length of sequence, \f$ n=2^m-1 \f$
    unsigned int v;     // shift register
    unsigned int b;     // return bit
};

// Default msequence generator objects
extern struct msequence_s msequence_default[13];

#endif  // __LIQUID_SEQUENCE_INTERNAL_H__

