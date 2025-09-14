// auto-generated file : do not edit
// invoked as : ./gentab/qtype_logexp_shiftadd_gentab -n 32 

#include "liquidfpm.internal.h"

// Pre-computed look-up table: A[k] = log2( 1 + 2^-k )
const q32_t q32_log2_shiftadd_Ak_tab[32] = {
    0x02000000,
    0x012b8034,
    0x00a4d3c2,
    0x00570069,
    0x002cc7ee,
    0x0016bad4,
    0x000b73cb,
    0x0005bf94,
    0x0002e139,
    0x000170f8,
    0x0000b893,
    0x00005c4f,
    0x00002e29,
    0x00001715,
    0x00000b8b,
    0x000005c5,
    0x000002e3,
    0x00000171,
    0x000000b9,
    0x0000005c,
    0x0000002e,
    0x00000017,
    0x0000000c,
    0x00000006,
    0x00000003,
    0x00000001,
    0x00000001,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};

// Maximum number of iterations, given the shiftadd_Ak_table
// above.  The shift|add algorithm will hit an infinite loop
// condition for values in the table equal to zero, hence this
// limitation.
const unsigned int q32_log2_shiftadd_nmax = 26;

// constants for logarithm base conversions
const q32_t q32_ln2     = 0x0162e430; // log(2)
const q32_t q32_log10_2 = 0x009a209c; // log(10)
const q32_t q32_log2_e  = 0x02e2a8ec; // log2(e)
const q32_t q32_log2_10 = 0x06a4d3c0; // log2(10)

