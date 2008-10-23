//
// Cyclic redundancy check
// 

#include "fec_internal.h"

// slow implementation
unsigned char reverse_byte(unsigned char _x)
{
    unsigned char y = 0x00;
    unsigned int i;
    for (i=0; i<8; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}

unsigned int reverse_uint32(unsigned int _x)
{
    unsigned int i, y=0;
    for (i=0; i<32; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}

