//
//
//

#include "modem_common.h"

const char* modulation_scheme_str[10] = {
    "unknown",
    "psk",
    "bpsk",
    "qpsk",
    "dpsk",
    "pam",
    "qam",
    "arb",
    "arb (mirrored)",
    "arb (rotated)"};

unsigned int gray_encode(unsigned int symbol_in)
{
    return symbol_in ^ (symbol_in >> 1);
}

unsigned int gray_decode(unsigned int symbol_in)
{
    unsigned int mask = symbol_in;
    unsigned int symbol_out = symbol_in;
    unsigned int i;

    // Run loop in blocks of 4 to reduce number of comparisons. Running
    // loop more times than MAX_MOD_BITS_PER_SYMBOL will not result in
    // decoding errors.
    for (i=0; i<MAX_MOD_BITS_PER_SYMBOL; i+=4) {
        symbol_out ^= (mask >> 1);
        symbol_out ^= (mask >> 2);
        symbol_out ^= (mask >> 3);
        symbol_out ^= (mask >> 4);
        mask >>= 4;
    }

    return symbol_out;
}

unsigned int count_bit_errors(
    unsigned int _s1,
    unsigned int _s2)
{
    //return count_ones(_s1^_s2);
    return 0;
}


