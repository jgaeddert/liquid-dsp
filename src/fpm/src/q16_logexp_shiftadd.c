// auto-generated file : do not edit
// invoked as : ./gentab/qtype_logexp_shiftadd_gentab -n 16 

#include "liquidfpm.internal.h"

// Pre-computed look-up table: A[k] = log2( 1 + 2^-k )
const q16_t q16_log2_shiftadd_Ak_tab[16] = {
    0x0200,
    0x012c,
    0x00a5,
    0x0057,
    0x002d,
    0x0017,
    0x000b,
    0x0006,
    0x0003,
    0x0001,
    0x0001,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
};

// Maximum number of iterations, given the shiftadd_Ak_table
// above.  The shift|add algorithm will hit an infinite loop
// condition for values in the table equal to zero, hence this
// limitation.
const unsigned int q16_log2_shiftadd_nmax = 10;

// constants for logarithm base conversions
const q16_t q16_ln2     = 0x0163; // log(2)
const q16_t q16_log10_2 = 0x009a; // log(10)
const q16_t q16_log2_e  = 0x02e3; // log2(e)
const q16_t q16_log2_10 = 0x06a5; // log2(10)

